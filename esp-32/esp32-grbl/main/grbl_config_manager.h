#ifndef GRBL_CONFIG_MANAGER_H
#define GRBL_CONFIG_MANAGER_H

#include <stdint.h>
#include "esp_err.h"

#define MAX_SSID_SIZE           32
#define MAX_PASSWORD_SIZE       32
#define MAX_IP_SIZE             16
#define GRBL_INT_PARS_SIZE      7
#define GRBL_BOOL_PARS_SIZE     8
#define GRBL_DOUBLE_PARS_SIZE   39
#define GRBL_PARS_MAX_INDEX     140

typedef enum _GRBL_ParameterType{ GPT_INT, GPT_BOOL, GPT_DOUBLE, GPT_INVALID} GRBL_ParameterType;
typedef struct _GRBLBoardModeConfig{
    char    m_SSID[MAX_SSID_SIZE];
    char    m_Password[MAX_PASSWORD_SIZE];
    char    m_IP[MAX_IP_SIZE];
    char    m_NetMask[MAX_IP_SIZE];
    char    m_Gateway[MAX_IP_SIZE];
} GRBLBoardModeConfig;

typedef struct _GRBLBoardConfig{
    bool                m_isAccessPoint;
    GRBLBoardModeConfig m_StConfig;
    GRBLBoardModeConfig m_APConfig;
    int                 m_StAttemptsCount;
    unsigned int        m_ParsInt[GRBL_INT_PARS_SIZE];
    bool                m_ParsBool[GRBL_BOOL_PARS_SIZE];
    double              m_ParsDouble[GRBL_DOUBLE_PARS_SIZE];
} GRBLBoardConfig;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_InitConfigManager( GRBLBoardConfig* theConfig );

esp_err_t GRBL_ConfigUpdateByINI( GRBLBoardConfig* theConfig, char* theFileName);
esp_err_t GRBL_ConfigSaveINI( GRBLBoardConfig* theConfig, char* theFileName);

esp_err_t GRBL_SetConfigStringParameter(char* theTarget, char* theValue, int theSize);

int GRBL_GetSDMOSIPin(  GRBLBoardConfig* theConfig );
int GRBL_GetSDMISOPin(  GRBLBoardConfig* theConfig );
int GRBL_GetSDSCKPin(  GRBLBoardConfig* theConfig );
int GRBL_GetSDCSPin(  GRBLBoardConfig* theConfig );
int GRBL_GetSDDetPin(  GRBLBoardConfig* theConfig );

char* GRBL_GetSSID( GRBLBoardConfig* theConfig, bool isAP );
esp_err_t GRBL_SetSSID( GRBLBoardConfig* theConfig, char* theValue, bool isAP );
char* GRBL_GetPassword( GRBLBoardConfig* theConfig, bool isAP );
esp_err_t GRBL_SetPassword( GRBLBoardConfig* theConfig, char* theValue, bool isAP );
char* GRBL_GetIP( GRBLBoardConfig* theConfig, bool isAP );
esp_err_t GRBL_SetIP( GRBLBoardConfig* theConfig, char* theValue, bool isAP );
char* GRBL_GetNetMask(  GRBLBoardConfig* theConfig, bool isAP );
esp_err_t GRBL_SetNetMask( GRBLBoardConfig* theConfig, char* theValue, bool isAP );
char* GRBL_GetGateway(  GRBLBoardConfig* theConfig, bool isAP );
esp_err_t GRBL_SetGateway( GRBLBoardConfig* theConfig, char* theValue, bool isAP );
int GRBL_GetStAttempts( GRBLBoardConfig* theConfig );
esp_err_t GRBL_SetStAttempts( GRBLBoardConfig* theConfig, int theAttempts);
bool GRBL_IsWiFiAP( GRBLBoardConfig* theConfig );
void GRBL_SetIsWiFiAP( GRBLBoardConfig* theConfig, bool isAP );

GRBL_ParameterType GRBL_GetGRBLParamType(int theIndx, int* theRealIndx);
esp_err_t GRBL_GetGRBLParamBool(GRBLBoardConfig* theConfig, int theIndx, bool* theRes);
esp_err_t GRBL_GetGRBLParamInt(GRBLBoardConfig* theConfig, int theIndx, unsigned int* theRes);
esp_err_t GRBL_GetGRBLParamDouble(GRBLBoardConfig* theConfig, int theIndx, double* theRes);
esp_err_t GRBL_SetGRBLParameterValue(GRBLBoardConfig* theConfig, int theIndx, char* theString);
esp_err_t GRBL_GetGRBLParameterValue( GRBLBoardConfig* theConfig, int theIndx, char* theBuffer);

char* GRBL_GetInternalFlashPrefix(  GRBLBoardConfig* theConfig );
char* GRBL_GetSDPrefix(  GRBLBoardConfig* theConfig );

#ifdef __cplusplus
}
#endif

#endif