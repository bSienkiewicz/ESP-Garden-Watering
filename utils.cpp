#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "index.h"

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print(MAIN_page);
    request->send(response);
  }
};

void setup_wifi(String ssid, String password, DNSServer dnsServer, AsyncWebServer server) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println("Connecting to " + ssid + " ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to " + ssid + " with IP address: " + WiFi.localIP().toString());
  digitalWrite(2, LOW);
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  server.begin();
}

void setup_ap(String ap_name, String ap_password){
  WiFi.softAP(ap_name.c_str(), ap_password.c_str());
  Serial.println("AP started with name: " + WiFi.softAPSSID() + " and IP: " + WiFi.softAPIP().toString());
}
