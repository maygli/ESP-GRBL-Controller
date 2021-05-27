#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>   // Include the SPIFFS library
#include "ArduinoJson.h"
#include "config.h"

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)
void onBoardName();

void setup() {
  ControllerConfig aCtrlCfg;
  DynamicJsonDocument aDoc(1024);
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  bool isAP = aCtrlCfg.isAcessPoint();
  Serial.println("isAp=");
  Serial.println(isAP);
  if( !isAP ){
    for( int i = 0 ; i < aCtrlCfg.getSSIDCount() ; i++ ){
      Serial.print(aCtrlCfg.getSSID(i));
      Serial.print(":");
      Serial.println(aCtrlCfg.getSTPassword(i));  
      wifiMulti.addAP(aCtrlCfg.getSSID(i).c_str(), aCtrlCfg.getSTPassword(i).c_str());   // add Wi-Fi networks you want to connect to
    }
    Serial.println("Try to connect in station mode ...");
    unsigned long aConnStart = millis();
    unsigned long aConnTimeOut = (unsigned long)aCtrlCfg.getConnectionTimeout();
    while(wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
      Serial.print(".");
      unsigned long aCurrTime = millis();
      if( (aCurrTime - aConnStart) > aConnTimeOut ){
        isAP = true;
        break;
      }
      delay(250);
    }
    if( !isAP ){
      Serial.println('\n');
      Serial.print("Connected in station mode ");
      Serial.println(WiFi.SSID());              // Tell us what network we're connected to
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
    }
  }
  Serial.println("");
  if( isAP ){
    Serial.println("Try to connect in AP mode");
    IPAddress local_ip(192,168,1,120);
    IPAddress gateway(192,168,1,120);
    IPAddress subnet(255,255,255,0);
    if(!WiFi.softAPConfig(local_ip,gateway,subnet)){
      Serial.println("Set  AP config failed");
    }
//      WiFi.softAPConfig(local_IP, gateway, subnet);
    String anAccessPointName = aCtrlCfg.getAccessPointName();
    String anAccessPointPwd = aCtrlCfg.getAccessPointPassword();
    if( WiFi.softAP(anAccessPointName,anAccessPointPwd)){
      Serial.println("Connected in AP mode:");
      Serial.println(WiFi.softAPIP());
    }
    else{
      Serial.println("Connection failed");
    }
  }
  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  LittleFS.begin();                           // Start the SPI Flash Files System
  
  server.on("/boardname",onBoardName);
  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}

void onBoardName()
{
  DynamicJsonDocument aDoc(256);
  aDoc["board_name"] = "MantaRay";
  aDoc["hardware_version"] = "v3.0";
  aDoc["software_version"] = "v1.0";
  aDoc["fabricant"] = "Maygli mmaygli@gmail.com";
  String anOut; 
  serializeJson(aDoc, anOut);
  server.send(200, "application/json", anOut);
}

void loop(void) {
  server.handleClient();
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (LittleFS.exists(path)) {                            // If the file exists
    File file = LittleFS.open(path, "r");                 // Open it
    server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}
