// CEEDLING NOTICE: This generated file only to be consumed by CMock

#ifndef _VOLTMONITORING_PRIV_H_ // Ceedling-generated include guard
#define _VOLTMONITORING_PRIV_H_

#include "voltMonRun.h"

typedef struct
{
    VoltMon_State_t state;

    uint16_t uvActivationTimer_ms;
    uint16_t ovActivationTimer_ms;

    uint16_t deactivationTimer_ms;

} VoltMon_Context_t;

uint16_t VoltMon_GetUnderOn_mV(void);

uint16_t VoltMon_GetUnderOff_mV(void);

uint16_t VoltMon_GetOverOn_mV(void);

uint16_t VoltMon_GetOverOff_mV(void);

extern VoltMon_Context_t VoltMon_Ctx;

#endif // _VOLTMONITORING_PRIV_H_
