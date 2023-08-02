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
#define TIME_TO_SLEEP 1200  // seconds
#define uS_TO_S_FACTOR 1000000

DNSServer dnsServer;
AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

long lastReading = 0;
char msg[50];

const bool SLEEP_MODE = true;
const String MQTT_USER = "plantwise";
const String AP_NAME = "PlantWise";
const String AP_PASSWORD = "";
const int MAX_PLANTS = 4;
const int SENSOR_PINS[] = { 35, 34, 39, 36 };
const int VALVE_PINS[] = { 21, 22, 23, 19 };

StaticJsonDocument<1024> config;

//0%: 1496 100%: 1042

class Plant {
private:
  int sensorPin;
  int valvePin;
  int moisture0;
  int moisture100;
  int moisturePercentage;
  int readingAccuracy = 2;
  int lowerTreshold;
  int upperTreshold;
  int wateringTime;
  int wateringCycles = 0;

public:
  int id;
  bool configured;
  Plant() {
    this->configured = false;
  }

  Plant(int id, int sensorPin, int valvePin, int moisture0, int moisture100, int lowerTreshold, int upperTreshold, int wateringTime) {
    this->id = id;
    this->sensorPin = sensorPin;
    this->valvePin = valvePin;
    this->moisture0 = moisture0;
    this->moisture100 = moisture100;
    this->lowerTreshold = lowerTreshold;
    this->upperTreshold = upperTreshold;
    this->wateringTime = wateringTime;
    this->configured = true;
    Serial.print("Configured plant #");
    Serial.println(this->id);
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
    digitalWrite(valvePin, LOW);
  }

  void stopPump() {
    digitalWrite(valvePin, HIGH);
  }

  bool isWateringNeeded() {
    return lowerTreshold > readMoisturePercentage();
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

Plant plants[MAX_PLANTS];

void createPlantsFromConfig() {
  JsonObject configObject = config.as<JsonObject>();

  // Remove old plants
  for (int i = 0; i < 4; i++) {
    plants[i] = Plant();
  }
  int plantsConfigured = 0;

  for (JsonObject::iterator it = configObject.begin(); it != configObject.end(); ++it) {
    int plantId = atoi(it->key().c_str());
    JsonObject plantData = it->value().as<JsonObject>();

    // Check if any value is 0, skip creating the plant
    if (plantData["moistureMin"].as<int>() == 0 ||
        plantData["moistureMax"].as<int>() == 0 ||
        plantData["lowerTreshold"].as<int>() == 0 ||
        plantData["upperTreshold"].as<int>() == 0 ||
        plantData["wateringTime"].as<int>() == 0) {
      continue;
    }

    plants[plantId - 1] = Plant(
      plantId,
      SENSOR_PINS[plantId - 1],
      VALVE_PINS[plantId - 1],
      plantData["moistureMin"].as<int>(),
      plantData["moistureMax"].as<int>(),
      plantData["lowerTreshold"].as<int>(),
      plantData["upperTreshold"].as<int>(),
      plantData["wateringTime"].as<int>()
    );
    plantsConfigured++;
  }

  Serial.print(plantsConfigured);
  Serial.println(" plants initialized from configuration.");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(CALIBRATION_BUTTON, INPUT_PULLUP);
  digitalWrite(21, HIGH);

  
  for (int i = 0; i < MAX_PLANTS; i++) {
    plants[i] = Plant();
  }

  setup_wifi(SSID, PASSWORD, dnsServer, server);
  setup_ap(AP_NAME, AP_PASSWORD);
  mqtt_config(MQTT_SERVER, MQTT_PORT);
}

void mqtt_reconnect() {
  client.disconnect();
  if (WiFi.status() != WL_CONNECTED) setup_wifi(SSID, PASSWORD, dnsServer, server);
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
  client.setKeepAlive(240);
  client.setSocketTimeout(240);
  client.setBufferSize(500);
  mqtt_reconnect();

  client.subscribe("esp/#");
  client.setCallback(callback);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message on topic: ");
  Serial.println(topic);

  if (String(topic) == "esp/config") {
    deserializeJson(config, (char*)message);
    createPlantsFromConfig();
  }
}
void loop() {
  long now = millis();
  delay(10);

  dnsServer.processNextRequest();

  if (!client.connected()) mqtt_reconnect();
  client.loop();

  if (digitalRead(CALIBRATION_BUTTON) == LOW) {
    plants[0].calibration();
  }

  if (now - lastReading > 5000) {
    lastReading = now;

    bool anyPlantConfigured = false;

    for (Plant plant : plants) {
      if (plant.configured) {
        anyPlantConfigured = true;

        String topic = "esp/plant/" + String(plant.id) + "/moisture";
        if (client.publish(topic.c_str(), String(plant.readMoisturePercentage()).c_str()))
          Serial.println(String(plant.getMoisturePercentage()) + "% published");
        else {
          Serial.println("Failed to publish");
          for (int i = 0; i < 3; i++) {
            if (client.connected()) break;
            mqtt_reconnect();
            client.loop();
            delay(5000);
            if (client.publish(topic.c_str(), String(plant.readMoisturePercentage()).c_str())) {
              Serial.println(String(plant.getMoisturePercentage()) + "% published");
              break;
            }
          }
        }
      }
    }

    Serial.println("===============================================");

    // SLEEPING MODES
    if (!anyPlantConfigured) {
      Serial.println("No plants configured");
      client.publish("esp/error", "Failed to read config!");
      delay(200);
      WiFi.mode(WIFI_OFF);
      esp_sleep_enable_timer_wakeup(120 * uS_TO_S_FACTOR); // Sleep for 2 minutes
      Serial.println("Going to sleep for 2 minutes");
      esp_deep_sleep_start();
    } else if (SLEEP_MODE) {
      delay(200);
      WiFi.mode(WIFI_OFF);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      Serial.println("Going to sleep for " + String(TIME_TO_SLEEP) + " seconds");
      esp_deep_sleep_start();
    }
  }
}