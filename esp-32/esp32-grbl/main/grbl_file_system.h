#ifndef GRBL_FILE_SYSTEM
#define GRBL_FILE_SYSTEM

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_FCopy(char* theSrcPath, char* theDstPath);
esp_err_t GRBL_RmTree(char* thePath);

#ifdef __cplusplus
extern }
#endif

#endif