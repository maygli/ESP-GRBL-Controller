#ifndef GRBL_WIFI
#define GRBL_WIFI

#include "grbl_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

void GRBL_WiFiStart(GRBLServer* theServer);

#ifdef __cplusplus
}
#endif

#endif