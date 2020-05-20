#ifndef WemoManager_h
#define WemoManager_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <map>
#include "WemoSwitch.h"

#define PKT_BUF_MAX  512

class WemoManager
{
public:
	WemoManager();
	~WemoManager();

	bool start();
	void stop();

	void serverLoop();

	bool isRunning();

	bool addDevice(String name, uint32_t port, WemoCallback cb);


private:
	uint32_t     m_switch_cnt;
    WiFiUDP      m_udp;
    char         m_buffer[PKT_BUF_MAX];
    bool         m_running;

	std::map<uint32_t, WemoSwitch *> m_switches;
};

#endif
