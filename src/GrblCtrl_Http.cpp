#define ARDUINOJSON_USE_LONG_LONG  1

#include "GrblCtrl_Http.h"
#include "GrblCtrl_Debug.h"
#include "GrblCtrl_BoardInfo.h"

#include <ArduinoJson.h>

GrblCtrl_Http::GrblCtrl_Http() :
    m_isClear(false), m_HtmlFS(NULL), m_Config(NULL) 
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

void GrblCtrl_Http::setSDManager(GrblCtrl_SDManager* theManager)
{
  m_SDManager = theManager;
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
  m_Server.on("/board_info",std::bind(&GrblCtrl_Http::onBoardInfo,this));
  m_Server.on("/system_info",std::bind(&GrblCtrl_Http::onSystemInfo,this));
  m_Server.onNotFound(std::bind(&GrblCtrl_Http::onNotFound, this));

  m_Server.on("/upload", HTTP_POST, std::bind(&GrblCtrl_Http::onUploadFirmwareFinished, this), std::bind(&GrblCtrl_Http::onUploadFirmware, this) );
  m_Server.on("/upload_parameters", HTTP_POST, std::bind(&GrblCtrl_Http::onUploadFiremwareParameters, this));

  m_Server.begin();                           // Actually start the server
  DEBUG_PRINT_LN("HTTP server started");

}

void GrblCtrl_Http::process()
{
    m_Server.handleClient();
}

void GrblCtrl_Http::onUploadFirmwareFinished()
{
    String aMsg = "File uploaded";
    m_Server.send(200, "text/plain", aMsg);
}

void GrblCtrl_Http::onUploadFiremwareParameters()
{
  DEBUG_PRINT_LN("GrblCtrl_Http::onUploadFiremwareParameters");
  m_isClear = false;
  if( m_Server.hasArg("clear")){
    if( m_Server.arg("clear") == "true"){
      m_isClear = true;
      DEBUG_PRINT_LN("Set clear");
    }
  }
  for( int i = 0 ; i < m_Server.args() ; i++ ){
    DEBUG_PRINT(m_Server.argName(i));
    DEBUG_PRINT("=");
    DEBUG_PRINT_LN(m_Server.arg(i));
  }
  String aMsg = "OK";
  m_Server.send(200, "text/plain", aMsg);
}

void GrblCtrl_Http::onUploadFirmware()
{
    DEBUG_PRINT_LN("Upload called");
    DEBUG_PRINT_LN(m_Server.hostHeader());
    HTTPUpload& upload = m_Server.upload();
    if(upload.status == UPLOAD_FILE_START){
        if( m_isClear ){
          m_isClear = false;
          DEBUG_PRINT_LN("Format FS");
          m_HtmlFS->format();
        }
        String aFileName = upload.filename;
        if(!aFileName.startsWith("/")) aFileName = "/"+ aFileName;
           DEBUG_PRINT("handleFileUpload Name: "); DEBUG_PRINT_LN(aFileName);
        m_UploadedFile = m_HtmlFS->open(aFileName, "w");
        if(!m_UploadedFile ){
            m_Server.send(413, "text/plain", "413: couldn't create file");
        }
    } else if(upload.status == UPLOAD_FILE_WRITE){
        if(m_UploadedFile){
            size_t aWrCnt = m_UploadedFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
            if( aWrCnt != upload.currentSize ){

            }
        }
    } else if(upload.status == UPLOAD_FILE_END){
        if(m_UploadedFile) {                                    // If the file was successfully created
            m_UploadedFile.close();                               // Close the file again
            DEBUG_PRINT("handleFileUpload Size: "); DEBUG_PRINT_LN(upload.totalSize);
//      m_Server.sendHeader("Location","/success.html");      // Redirect the client to the success page
//      m_Server.send(303);
        } else {
            m_Server.send(500, "text/plain", "500: couldn't create file");
        }
    }    
}

void GrblCtrl_Http::onNotFound()
{
    if (!handleFileRead(m_Server.uri()))                  // send it if it exists
      m_Server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
}

void GrblCtrl_Http::onBoardInfo()
{
  DynamicJsonDocument aDoc(384);
  aDoc["board_name"] = BOARD_NAME;
  aDoc["board_hw_version"] = BOARD_HW_VERSION;
  aDoc["board_sw_version"] = BOARD_SW_VERSION;
  aDoc["board_desc"] = BOARD_DESC;
  aDoc["manufacturer"] = BOARD_MANUFACTURER;
  aDoc["copyright"] = BOARD_COPYRIGHT;

  String anOut; 
  serializeJson(aDoc, anOut);
  m_Server.send(200, "application/json", anOut);
}

void GrblCtrl_Http::onSystemInfo()
{
  StaticJsonDocument<96> aDoc;
  aDoc["grbl_connected"] = true;

  JsonObject aSdCard = aDoc.createNestedObject("sd_card");
  aSdCard["mounted"] = m_SDManager->isInserted();
  aSdCard["total_size"] = m_SDManager->getTotalSize();
  aSdCard["free_space"] = m_SDManager->getFreeSpace();
  aDoc["done"] = 100;
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
  bool isRoot = false;
  if (thePath.endsWith("/")){
    DEBUG_PRINT_LN("Root requested"); 
    if( thePath == "/" ){
      DEBUG_PRINT_LN("Root set");
      isRoot = true;
    }
    thePath += "index.html";
  }
  String contentType = getContentType(thePath);            // Get the MIME type
  if (LittleFS.exists(thePath)) {                            // If the file exists
    File file = m_HtmlFS->open(thePath, "r");                 // Open it
    m_Server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  else{
    DEBUG_PRINT_LN("File not found");
    if( isRoot == true ){
      DEBUG_PRINT_LN("Send defualt page");
      m_Server.send(200, "text/html", m_sDefaultPage);
      return true;
    }
  }
  DEBUG_PRINT_LN("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}