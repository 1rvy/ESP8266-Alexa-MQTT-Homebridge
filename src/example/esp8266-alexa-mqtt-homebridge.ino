//This sketch will enable web server for UPnP/SOAP for Wemo discovery for Alexa.
//Added OTA for updates once deployed
//Added MQTT for relay state info
//Edited to use "witnessmenow's" code for Wemo discovery (https://github.com/witnessmenow/esp8266-alexa-wemo-emulator)
//Tested on  12/03/17

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>   
#include <PubSubClient.h>
#include "WemoSwitch.h"
#include "WemoManager.h"
#include "CallbackFunction.h"

#define mqtt_clientid "wemos1"
#define mqtt_server "192.168.6.12"
#define mqtt_user "aimqtt"
#define mqtt_password "mqttuser1" 

const int relayPin = 5;    
const char* wifi_ssid = "bobthebuilder";
const char* wifi_password = "scoopmuckanddizzy";
const char* friendlyName = "Conservatory Heater";             
const unsigned int webserverPort = 9876;
const char* stateTopic = "sensor2/control";
const char* stateTopic2 = "sensor2/state";

boolean connectWifi();

void lightOn();
void lightOff();

WemoManager wemoManager;
WemoSwitch *light = NULL;

WiFiServer TelnetServer(8266);
WiFiClient espClient;
PubSubClient client(espClient);

void setup(){ 
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.print("Connecting Wifi: ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  wemoManager.begin();
  light = new WemoSwitch(friendlyName, webserverPort, lightOn, lightOff);
  wemoManager.addDevice(*light);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
  
  pinMode(relayPin, OUTPUT);
  delay(10);
  digitalWrite(relayPin, LOW);

  Serial.print("Configuring OTA device...");
  TelnetServer.begin();   
  ArduinoOTA.onStart([]() {Serial.println("OTA starting...");});
  ArduinoOTA.onEnd([]() {Serial.println("OTA update finished!");Serial.println("Rebooting...");});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {Serial.printf("OTA in progress: %u%%\r\n", (progress / (total / 100)));});  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setPassword((const char *)"Sivtec22"); 
  ArduinoOTA.begin();
  Serial.println("OK");
} 

void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);
  if (receivedChar == '1'){
  client.publish(stateTopic2, "1");
  digitalWrite(relayPin, HIGH);
 } else {
  if (receivedChar == '0')
  client.publish(stateTopic2, "0");
  digitalWrite(relayPin, LOW);
 }
  }
  Serial.println();
}

void mqtt_reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_clientid, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(stateTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void lightOn() {
    Serial.print("Switch relay on ...");
    digitalWrite(relayPin, HIGH);
    client.publish(stateTopic2, "1");
}

void lightOff() {
    Serial.print("Switch relay off ...");
    digitalWrite(relayPin, LOW);
    client.publish(stateTopic2, "0");
}

void loop()
{
  wemoManager.serverLoop();
  
  client.loop();
      if (!client.connected()) {
    mqtt_reconnect();
    }
  ArduinoOTA.handle();
  
  }
