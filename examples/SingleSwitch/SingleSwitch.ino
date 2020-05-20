#include <ESP8266WiFi.h>

#include "WemoSwitch.h"
#include "WemoManager.h"


//on/off callbacks
void lightSwitch(bool status);

//------- Replace the following! ------
char ssid[] = "xxx";       // your network SSID (name)
char password[] = "yyyy";  // your network key

WemoManager wemoManager;

const int ledPin = BUILTIN_LED;

void setup()
{
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously
    // connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);

    // Format: Alexa invocation name, local port no, switch callback
    wemoManager.addDevice("test lights", 80, lightSwitch);
    wemoManager.start();

    pinMode(ledPin, OUTPUT);    // Initialize digital ledPin as an output.
    delay(10);
    digitalWrite(ledPin, HIGH); // Wemos BUILTIN_LED is active Low, so high is off
}

void loop()
{
  wemoManager.serverLoop();
}

void lightSwitch(bool status)
{
    if ( true == status )
    {
        Serial.print("Switch 1 turn on ...");
        digitalWrite(ledPin, LOW);
    }
    else
    {
        Serial.print("Switch 1 turn off ...");
        digitalWrite(ledPin, HIGH);
    }
}
