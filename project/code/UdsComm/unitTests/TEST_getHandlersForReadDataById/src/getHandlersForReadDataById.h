
#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include "diagnostic_cfg.h"
#include <stdint.h>

extern uint8 g_errCode_u8;
extern uint16 g_did_cu16;

Std_ReturnType getHandlersForReadDataById(uint8 *l_errCode_u8, uint16 l_did_cu16, uint8 *l_diagBufSize_u8, Std_ReturnType *l_didSupported_, uint8 *l_diagBuf_pu8);

#endif