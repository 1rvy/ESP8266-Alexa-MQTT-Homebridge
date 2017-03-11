//This sketch will enable web server on port 80 for Homebridge communication to ESP8266 as well as UPnP/SOAP for Wemo discovery for Alexa.
//Web server is working, access local web server to show button to control Heater, Homebridge integration working with homebridge-http
//Added OTA for updates once deployed
//Added MQTT for relay state info
//Edited to use "witnessmenow's" code for Wemo discovery (https://github.com/witnessmenow/esp8266-alexa-wemo-emulator)
//Tested on  11/03/17

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>   //For OTA
#include <PubSubClient.h>

#include "WemoSwitch.h"
#include "WemoManager.h"
#include "CallbackFunction.h"

#define mqtt_clientid ""					// Your MQTT client ID
#define mqtt_server ""						// Your MQTT broker IP
#define mqtt_user ""						// Your MQTT username
#define mqtt_password "" 					// Your MQTT password

const int relayPin = 5;    
const char* wifi_ssid = "";					// Your SSID
const char* wifi_password = "";				// Your PSK
const char* friendlyName = "";             	// Alexa will use this name to identify your device
const unsigned int webserverPort = 9876;
const char* stateTopic = "";				// MQTT Subscribe Topic
const char* stateTopic2 = "";				// MQTT Publush Topic

boolean connectWifi();
//on/off callbacks
void lightOn();
void lightOff();
WemoManager wemoManager;
WemoSwitch *light = NULL;

ESP8266WebServer webserver(80);
WiFiServer TelnetServer(8266);
WiFiClient espClient;
PubSubClient client(espClient);

String webPage = "";

void setup(){ 
   // Open serial communications and wait for port to open:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
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
  // Format: Alexa invocation name, local port no, on callback, off callback
  
  light = new WemoSwitch(friendlyName, webserverPort, lightOn, lightOff);
  wemoManager.addDevice(*light);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
  
  // preparing GPIOs
  pinMode(relayPin, OUTPUT);
  delay(10);
  digitalWrite(relayPin, LOW);

  // Configure OTA service
  
  Serial.print("Configuring OTA device...");
  TelnetServer.begin();   //Necesary to make Arduino Software autodetect OTA device  
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
  ArduinoOTA.setPassword((const char *)"Sivtec22"); // for password prompt insert between " "
  ArduinoOTA.begin();
  Serial.println("OK");
  
  // Basic web page used for Apple Homekit via homebridge, I will change this to MQTT eventually
  
  webPage += "<h1>Web Server</h1><p>Test 1<a href=\"On\"><button>ON</button></a>&nbsp;<a href=\"Off\"><button>OFF</button></a></p>";

  webserver.on("/", [](){
  webserver.send(200, "text/html", webPage);
  });
  webserver.on("/On", [](){
    webserver.send(200, "text/html", webPage);
    digitalWrite(relayPin, HIGH);
    client.publish(stateTopic2, "1");
  });
  webserver.on("/Off", [](){
    webserver.send(200, "text/html", webPage);
    digitalWrite(relayPin, LOW);
    client.publish(stateTopic2, "0");
  });
  webserver.begin();
  Serial.println("HTTP server started");
} 

// Listen for incoming MQTT publish

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

// MQTT process start

void mqtt_reconnect() {						
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_clientid, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // ... and subscribe to topic
      client.subscribe(stateTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
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
    
  webserver.handleClient();   //Webserver on port 80
  
  ArduinoOTA.handle();
  
  }
