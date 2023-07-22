#ifndef UTILS_H
#define UTILS_H

void setup_wifi(String ssid, String password, DNSServer dnsServer, AsyncWebServer server);
void setup_ap(String ap_name, String ap_password);

#endif