#ifndef GRBL_CTRL_SD_MANAGER_H
#define GRBL_CTRL_SD_MANAGER_H

#include <SDFS.h>

class GrblCtrl_SDManager
{
public:    
    GrblCtrl_SDManager();
    void process();
    void init();
    bool isInserted() const;
    long long getTotalSize() const;
    long long getFreeSpace() const;
protected:
    bool getSDDetPinState(); 
    void createSDFS();   
protected:
    bool            m_isGuard; 
    unsigned long   m_GuardStTime;
    unsigned long   m_TotalSize;
    unsigned long   m_FreeSpace;
    sdfs::SDFSImpl* m_SDFs; 
};

#endif