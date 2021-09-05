#include "GrblCtrl_SDManager.h"
#include "GrblCtrl_Debug.h"

#define  SDFAT_FILE_TYPE 3

#include <SdFat.h>
#include <SDFS.h>
#include <Arduino.h>

#define SD_DET_PIN 4
#define GUARD_TIME 500

GrblCtrl_SDManager::GrblCtrl_SDManager()
{
    m_isGuard = false;
    m_SDFs = NULL;
    m_TotalSize = 0;
    m_FreeSpace = 0;
    pinMode(SD_DET_PIN, INPUT);
}
    
void GrblCtrl_SDManager::init()
{
    createSDFS();
}

void GrblCtrl_SDManager::process()
{
    if( m_isGuard ){
        unsigned long aCurrTime = millis();
        if( (aCurrTime - m_GuardStTime) > GUARD_TIME ){
            m_isGuard = false;
            bool isIns = getSDDetPinState();
            if( isIns != isInserted() ){
                if( isIns ){
                    createSDFS();
                }
            }
        }
        return;
    }
    bool isIns = getSDDetPinState();
    if( isIns != isInserted() ){
        m_isGuard = true;
        m_GuardStTime = millis();
        if( !isIns ){
            DEBUG_PRINT_LN("SDFS stop");
            m_SDFs->end();
            if( m_SDFs ){
                delete m_SDFs;
                m_SDFs = NULL;
            }
        }
    }
}

bool GrblCtrl_SDManager::getSDDetPinState()
{
    bool isIns = false;
    int aPortState = digitalRead(SD_DET_PIN);
    if( aPortState == LOW ){
        isIns = true; 
    }
    return isIns;
}

bool GrblCtrl_SDManager::isInserted() const
{
    return m_SDFs != NULL;
}

long long GrblCtrl_SDManager::getTotalSize() const 
{
    if( isInserted() )
        return m_TotalSize;
    return 0;
}

long long GrblCtrl_SDManager::getFreeSpace() const
{
    if( isInserted() )
        return m_FreeSpace;
    return 0;
}

void GrblCtrl_SDManager::createSDFS()
{
    m_SDFs = new sdfs::SDFSImpl();
    sdfs::SDFSConfig aConfig(5,SD_SCK_MHZ(10));
    m_SDFs->setConfig(aConfig);
    bool isOk = m_SDFs->begin();
    if( !isOk ){
        delete m_SDFs;
        m_SDFs = NULL;
        DEBUG_PRINT_LN("Error mount SDFS");
        return;
    }
    DEBUG_PRINT_LN("Mount OK");
    fs::FSInfo64 anInfo;
    anInfo.totalBytes = 0;
    anInfo.usedBytes = 0;
    isOk  = m_SDFs->info64(anInfo);
    if( !isOk ){
        DEBUG_PRINT_LN("Error read SDCard info");
    }
    else{
        m_TotalSize = (unsigned long)(anInfo.totalBytes/1024/1024);
        m_FreeSpace = m_TotalSize - (unsigned long)(anInfo.usedBytes/1024/1024);
        DEBUG_PRINT("Total size:");
        Serial.println(m_TotalSize);
        DEBUG_PRINT("Free space:");
        DEBUG_PRINT_LN((unsigned int)(m_FreeSpace));
    }
    FileImplPtr aFile = m_SDFs->open("/links.txt",OM_DEFAULT,AM_READ);
    if( !aFile ){
        DEBUG_PRINT_LN("Error open file");
        return;
    }
    DEBUG_PRINT_LN("File is opened");
    uint8_t aData[101];
    aFile->read(&aData[0],100);
    aFile->close();
    aData[100] = 0;
    DEBUG_PRINT_LN((char*)aData);
}   
