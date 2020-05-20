#Arduino Esp8266 Alexa Wemo switch emulator

This project is completly based on the [forked repo](https://github.com/witnessmenow/esp8266-alexa-wemo-emulator), all the hard work was done by kakopappa and witnessmenow. I just reworked their code to make it more in line with C++ guidelines.

This library enables your esp8266 to simulate a Belkin Wemo switch. It can be discovered as a device by your Amazon Echo/Dot on the Smart home section. It supports calling the emulated device a custom name e.g. "Alexa, turn off test lights", where test lights is the custom name

The library supports emulating any number of switches using 1 ESP8266 chip.

## Installing

The downloaded code can be included as a new library into the IDE selecting the menu:

     Sketch / include Library / Add .Zip library

Restart the Arduino IDE and follow the examples located at

     File -> Examples -> Esp8266AlexaWemoEmulator
