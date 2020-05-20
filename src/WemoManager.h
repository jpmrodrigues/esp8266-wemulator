#ifndef WemoManager_h
#define WemoManager_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "WemoSwitch.h"

#define MAX_SWITCHES 14
#define PKT_BUF_MAX  512


class WemoManager
{
public:
	WemoManager();
	~WemoManager();

	bool start();

	void serverLoop();

	bool addDevice(String alexaInvokeName, uint32_t port, WemoCallback cb);


private:
	uint32_t     m_switch_cnt;
	WemoSwitch * m_switches[MAX_SWITCHES];
    WiFiUDP      m_udp;
    char         m_buffer[PKT_BUF_MAX];
};

#endif
