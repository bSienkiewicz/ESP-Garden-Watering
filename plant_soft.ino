#include <DNSServer.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <PubSubClient.h>
#include "utils.h"
#include "esp_sleep.h"
#include "env.h"

#define LED 2
#define CALIBRATION_BUTTON 18
#define TIME_TO_SLEEP 60
#define uS_TO_S_FACTOR 1000000

DNSServer dnsServer;
AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

long lastReading = 0;
char msg[50];

const bool SLEEP_MODE = true;
const String MQTT_USER = "plantwis2e";
const String AP_NAME = "PlantWise";
const String AP_PASSWORD = "";

//0%: 1496 100%: 1042

class Plant {
private:
  int sensorPin;
  int pumpPin;
  int moisture0;
  int moisture100;
  int moisturePercentage;
  int readingAccuracy = 2;
  int wateringTreshold;
  int wateringTime;
  int wateringCycles = 0;

public:
  int id;
  Plant(int id, int sensorPin, int pumpPin, int moisture0, int moisture100, int wateringTreshold, int wateringTime) {
    this->id = id;
    this->sensorPin = sensorPin;
    this->pumpPin = pumpPin;
    this->moisture0 = moisture0;
    this->moisture100 = moisture100;
    this->wateringTreshold = wateringTreshold;
    this->wateringTime = wateringTime;
  }

  int getSensorData() {
    return analogRead(sensorPin);
  }

  int readMoisturePercentage() {
    int reading = 0;
    for (int i = 0; i < readingAccuracy; i++) {
      reading += getSensorData();
      delay(10);
    }
    reading /= readingAccuracy;
    if (moisture0 - moisture100 > 0) {
      if (reading > moisture0) moisturePercentage = 0;
      else if (reading < moisture100) moisturePercentage = 100;
      else
        moisturePercentage = (int)((static_cast<float>(moisture0 - reading) / static_cast<float>(moisture0 - moisture100)) * 100);
    } else {
      Serial.println("Awaiting calibration");
    }
    return moisturePercentage;
  }

  int getMoisturePercentage() {
    return moisturePercentage;
  }

  void startPump() {
    digitalWrite(pumpPin, LOW);
  }

  void stopPump() {
    digitalWrite(pumpPin, HIGH);
  }

  bool isWateringNeeded() {
    return readMoisturePercentage() < wateringTreshold;
  }

  void loop() {
    while (isWateringNeeded() && wateringCycles < 4) {
      wateringCycles++;
      Serial.println("Watering...");
      startPump();
      delay(wateringTime * 1000);
      stopPump();
      delay(3000);
    }
  }

  void calibration() {
    digitalWrite(LED, HIGH);
    Serial.println("Put the sensor in the dry soil and press the button");
    while (digitalRead(CALIBRATION_BUTTON) == HIGH) {
      delay(100);
    }
    Serial.println("Calibrating...");
    int sum = 0;
    for (int i = 0; i < 5; i++) {
      sum += analogRead(sensorPin);
      delay(1000);
    }
    moisture0 = sum / 5;
    moisture0 -= 30;
    Serial.println("Put the sensor in the water and press the button");
    while (digitalRead(CALIBRATION_BUTTON) == HIGH) {
      delay(100);
    }
    Serial.println("Calibrating...");
    sum = 0;
    for (int i = 0; i < 5; i++) {
      sum += analogRead(sensorPin);
      delay(1000);
    }
    moisture100 = sum / 5;
    moisture100 += 30;
    Serial.println("Calibration finished");
    Serial.print("0%: " + String(moisture0) + " 100%: " + String(moisture100));
    digitalWrite(LED, LOW);
    delay(2000);
  }
};

Plant plants[] = {
  Plant(1, 35, 21, 1496, 1042, 30, 2),
  Plant(2, 35, 21, 1496, 1042, 30, 2),
  Plant(3, 35, 21, 1496, 1042, 30, 2),
  Plant(4, 35, 21, 1496, 1042, 30, 2),
};

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(CALIBRATION_BUTTON, INPUT_PULLUP);
  digitalWrite(21, HIGH);

  setup_wifi(SSID, PASSWORD, dnsServer, server);
  setup_ap(AP_NAME, AP_PASSWORD);
  mqtt_config(MQTT_SERVER, MQTT_PORT);
}

void mqtt_reconnect() {
  client.disconnect();
  String un = MQTT_USER + String(random(0xffff), HEX);
  if (client.connect(un.c_str())) {
    Serial.println("Connected to MQTT broker as " + un);
  } else {
    Serial.print("Failed to connect to MQTT broker as ");
    Serial.print(un.c_str());
    Serial.print(" rc = ");
    Serial.print(client.state());
    Serial.println(" Retrying in 5 seconds...");
    delay(5000);
  }
}

void mqtt_config(String MQTT_SERVER, int MQTT_PORT) {
  client.setServer(MQTT_SERVER.c_str(), MQTT_PORT);
  mqtt_reconnect();
  client.setCallback(callback);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
}

void loop() {
  long now = millis();
  delay(10);
  
  dnsServer.processNextRequest();
  
  if (WiFi.status() != WL_CONNECTED) setup_wifi(SSID, PASSWORD, dnsServer, server);
  if (!client.connected()) mqtt_reconnect();
  client.loop();

  if (digitalRead(CALIBRATION_BUTTON) == LOW) {
    plants[0].calibration();
  }

  if (now - lastReading > 5000) {
    lastReading = now;

    for (Plant plant : plants) {
      plant.loop();
      String topic = "esp/plant/" + String(plant.id) + "/moisture";
      if (client.publish(topic.c_str(), String(plant.getMoisturePercentage()).c_str()))
          Serial.println(String(plant.getMoisturePercentage()) + "% published");
      else{ 
        Serial.println("Failed to publish");
        for (int i = 0; i < 3; i++) {
          if (client.connected()) break;
          mqtt_reconnect();
          client.loop();
          delay(5000);
          if (client.publish(topic.c_str(), String(plant.getMoisturePercentage()).c_str())) {
            Serial.println(String(plant.getMoisturePercentage()) + "% published");
            break;
          }
        }
      }

    }
    Serial.println("===============================================");

    if (SLEEP_MODE) {
      delay(200);
      WiFi.mode(WIFI_OFF);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      Serial.println("Going to sleep for " + String(TIME_TO_SLEEP) + " seconds");
      esp_deep_sleep_start();
    }
  }
}