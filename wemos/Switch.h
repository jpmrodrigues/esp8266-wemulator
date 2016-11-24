#ifndef SWITCH_H
#define SWITCH_H
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUDP.h>
 
class Switch {
private:
        String serial;
        String persistent_uuid;
        String device_name;
        unsigned int localPort;
        ESP8266WebServer *server = NULL;
        WiFiUDP UDP;                
        void startWebServer();
        void handleEventservice();
        void handleUpnpControl();
        void handleRoot();
        void handleSetupXml();
public:
        Switch();
        Switch(String alexaInvokeName, unsigned int port);
        ~Switch();
        void respondToSearch(IPAddress& senderIP, unsigned int senderPort);
};
 
#endif