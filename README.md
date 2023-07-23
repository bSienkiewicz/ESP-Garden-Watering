# PlantWise
PlantWise is a plant monitoring and watering system designed for the ESP32. It uses a moisture sensor to monitor the moisture level of the soil and an electric pump to water the plants when needed. The system can handle multiple plants and each plant can be configured individually.

# Getting Started
To get started with PlantWise, you'll need to clone this repository to your local machine.

`git clone https://github.com/username/PlantWise.git`

# Prerequisites
You'll need the Arduino IDE installed on your machine as well as the libraries listed below:

- DNSServer
- ArduinoJson
- WiFi
- WiFiClient
- AsyncTCP
- ESPAsyncWebServer
- PubSubClient

You can install these libraries through the Arduino IDE Library Manager.

# Configuration
You'll need to create an env.h file in the root directory of the project. This file should define the following environment variables:

SSID: The SSID of your WiFi network.
PASSWORD: The password of your WiFi network.
MQTT_SERVER: The address of your MQTT server.
MQTT_PORT: The port of your MQTT server.
Here's an example of what your env.h file should look like:
```
#define SSID "your_wifi_ssid"
#define PASSWORD "your_wifi_password"
#define MQTT_SERVER "your_mqtt_server"
#define MQTT_PORT your_mqtt_port
```
# Usage
Upload the code to your ESP32 using the Arduino IDE. The system will start monitoring the moisture level of the soil and water the plants when needed. You can view the system logs in the Arduino IDE Serial Monitor.

# Contributing
This project is currently not accepting contributions, as it is a thesis project. However, feel free to fork the repository and make your own changes.