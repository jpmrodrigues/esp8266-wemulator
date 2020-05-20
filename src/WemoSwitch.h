#ifndef __WEMUSWITCH_H_
#define __WEMUSWITCH_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

typedef std::function<void(bool)> WemoCallback;

class WemoSwitch
{

protected:
        WemoSwitch(String name, uint32_t port, WemoCallback cb);
        ~WemoSwitch();

        String getName();

        void start();
        void stop();
        void serverLoop();
        void respondToSearch(IPAddress& senderIP, uint32_t senderPort);

private:
        void handleEventservice();
        void handleUpnpControl();
        void handleRequestState(bool state);
        void handleRoot();
        void handleSetupXml();

        ESP8266WebServer * m_server;
        WiFiUDP            m_udp;
        String             m_serial;
        String             m_persistent_uuid;
        String             m_device_name;
        uint32_t           m_local_port;
        WemoCallback       m_cb;

        friend class WemoManager;
};

#endif /* __WEMUSWITCH_H_ */
