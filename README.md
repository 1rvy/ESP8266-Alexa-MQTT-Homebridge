#Arduino Esp8266 Alexa Wemo switch emulator

This project is developed from witnessmenow's great fork (https://github.com/witnessmenow/esp8266-alexa-wemo-emulator) of kakopappa's original sketch (https://github.com/kakopappa/arduino-esp8266-alexa-multiple-wemo-switch.

I have simply added OTA, MQTT and HTTP and functionality from other sources to create one sketch that allows discovery and control from Amazon Alexa, Apple Home (via Homebridge with homebridge-http plugin running on Ubuntu server) and MQTT.

I have this controlling a Wemos D1 Pro with a relay inside a heater in my conservatory and working well, I will update this to use MQTT to include temperature control via MQTT broker and Node-Red.  

