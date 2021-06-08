#include "GrblCtrl_Http.h"
#include "GrblCtrl_Debug.h"
#include "ArduinoJson.h"

GrblCtrl_Http::GrblCtrl_Http() :
    m_HtmlFS(NULL), m_Config(NULL)
{

}

GrblCtrl_Http::GrblCtrl_Http(GrblCtrl_Config* theConfig, FS* theFS) : 
     m_HtmlFS(theFS),m_Config(theConfig)
{

}

void GrblCtrl_Http::setConfig(GrblCtrl_Config* theConfig)
{
    m_Config = theConfig;
}

void GrblCtrl_Http::setFileSystem(FS* theFS)
{
    m_HtmlFS = theFS;
}

void GrblCtrl_Http::init()
{
  if( m_Config == NULL ){
      DEBUG_PRINT_LN("Error. GrblCtrl_Http. GrblCtrl_Http::init() calls before config is set");
      return;
  }  
  bool isAP = m_Config->isAcessPoint();
  DEBUG_PRINT("isAp=");
  DEBUG_PRINT_LN(isAP);
  if( !isAP ){
    for( int i = 0 ; i < m_Config->getSSIDCount() ; i++ ){
      DEBUG_PRINT(m_Config->getSSID(i));
      DEBUG_PRINT(":");
      DEBUG_PRINT_LN(m_Config->getSTPassword(i));  
      m_WifiMulti.addAP(m_Config->getSSID(i).c_str(), m_Config->getSTPassword(i).c_str());   // add Wi-Fi networks you want to connect to
    }
    DEBUG_PRINT_LN("Try to connect in station mode ...");
    unsigned long aConnStart = millis();
    unsigned long aConnTimeOut = (unsigned long)m_Config->getConnectionTimeout();
    while(m_WifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
      DEBUG_PRINT(".");
      unsigned long aCurrTime = millis();
      if( (aCurrTime - aConnStart) > aConnTimeOut ){
        isAP = true;
        break;
      }
      delay(250);
    }
    if( !isAP ){
      DEBUG_PRINT_LN('\n');
      DEBUG_PRINT("Connected in station mode ");
      DEBUG_PRINT_LN(WiFi.SSID());              // Tell us what network we're connected to
      DEBUG_PRINT("IP address:\t");
      DEBUG_PRINT_LN(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
    }
  }
  DEBUG_PRINT("");
  if( isAP ){
    DEBUG_PRINT_LN("Try to connect in AP mode");
    IPAddress local_ip(192,168,1,120);
    IPAddress gateway(192,168,1,120);
    IPAddress subnet(255,255,255,0);
    if(!WiFi.softAPConfig(local_ip,gateway,subnet)){
      DEBUG_PRINT_LN("Set  AP config failed");
    }
//      WiFi.softAPConfig(local_IP, gateway, subnet);
    String anAccessPointName = m_Config->getAccessPointName();
    String anAccessPointPwd = m_Config->getAccessPointPassword();
    if( WiFi.softAP(anAccessPointName,anAccessPointPwd)){
      DEBUG_PRINT_LN("Connected in AP mode:");
      DEBUG_PRINT_LN(WiFi.softAPIP());
    }
    else{
      DEBUG_PRINT_LN("Connection failed");
    }
  }
  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    DEBUG_PRINT_LN("mDNS responder started");
  } else {
    DEBUG_PRINT_LN("Error setting up MDNS responder!");
  }
  m_Server.on("/boardinfo",std::bind(&GrblCtrl_Http::onBoardInfo,this));
  m_Server.onNotFound(std::bind(&GrblCtrl_Http::onNotFound, this));

  m_Server.begin();                           // Actually start the server
  DEBUG_PRINT_LN("HTTP server started");

}

void GrblCtrl_Http::process()
{
    m_Server.handleClient();
}

void GrblCtrl_Http::onNotFound()
{
    if (!handleFileRead(m_Server.uri()))                  // send it if it exists
      m_Server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
}

void GrblCtrl_Http::onBoardInfo()
{
  DynamicJsonDocument aDoc(256);
  aDoc["board_name"] = "MantaRay";
  aDoc["hardware_version"] = "v3.0";
  aDoc["software_version"] = "v1.0";
  aDoc["fabricant"] = "Maygli mmaygli@gmail.com";
  String anOut; 
  serializeJson(aDoc, anOut);
  m_Server.send(200, "application/json", anOut);
}

String GrblCtrl_Http::getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool GrblCtrl_Http::handleFileRead(String thePath) { // send the right file to the client (if it exists)
  DEBUG_PRINT_LN("handleFileRead: " + thePath);
  if( m_HtmlFS == NULL ){
      DEBUG_PRINT_LN("Warning. GrblCtrl_Http::handleFileRead calls before file system is set.");
      return false;
  }
  if (thePath.endsWith("/")) thePath += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(thePath);            // Get the MIME type
  if (LittleFS.exists(thePath)) {                            // If the file exists
    File file = m_HtmlFS->open(thePath, "r");                 // Open it
    m_Server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  DEBUG_PRINT_LN("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}