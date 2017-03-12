#ESP8266 (Wemos D1 Pro) Alexa (Wemo emulation) and MQTT control.

This Arduino sketch is developed from witnessmenow's great fork (https://github.com/witnessmenow/esp8266-alexa-wemo-emulator) of kakopappa's original work (https://github.com/kakopappa/arduino-esp8266-alexa-multiple-wemo-switch.

I have simply added OTA and MQTT functionality from other sources to create one sketch that allows discovery and control from Amazon Alexa, Apple Home (via Homebridge with homebridge-mqttswitch plugin (https://github.com/ilcato/homebridge-mqttswitch) running on Ubuntu server) and MQTT.  All other library contents remain untouched from the original witnessmenow fork.

I have this deployed on a Wemos D1 Pro controlling a relay switching on a heater in my conservatory and so far it is working well, I will update this to use MQTT to include temperature control via MQTT broker and Node-Red.  

State changes are reflected via MQTT publish, e.g if Alexa/Homebridge/MQTT instruction turns on the device it pushes MQTT publish, I use MQTT Dash on Android and this monitors the publish string for state changes.

I recommend downloading and experimenting with Node-Red (https://nodered.org/) if using MQTT control as well as MQTT Dash for Android (https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en_GB)

![Alt text](20170216_220844.jpg?raw=true "Heater control")

The Wemos is powered by a 240V to 5V power supply (old HTC USB charger!) connected to micro USB, the relay is the Wemos relay shield, very small and neat package that is easily concealable inside the heater base.

![Alt text](20170311_120629.jpg?raw=true "Hidden in base!")
