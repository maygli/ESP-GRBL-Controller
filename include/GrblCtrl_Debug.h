#ifndef GRBL_CTRL_DEBUG_H
#define GRBL_CTRL_DEBUG_H
#include "GrblCtrl_Config.h"

#ifdef DEBUG_UART0
    #define DEBUG_PRINT_LN Serial.println
    #define DEBUG_PRINT Serial.print
#else
    #define DEBUG_PRINT_LN
    #define DEBUG_PRINT
#endif

#endif