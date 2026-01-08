#include "diagnostic_cfg.h"
#define NULL ((void *)0)


void checkCurrentNad(uint8 currentNad, Std_ReturnType *result)
{
    (void)currentNad;
    *result = E_OK;
}

/* Check if message data length is valid */
void checkMsgDataLength(uint16_t dataLength, Std_ReturnType *result)
{
    if (dataLength > 0u && dataLength <= 32u) {
        *result = E_OK;
    } else {
        *result = E_NOT_OK;
    }
}

Std_ReturnType RdbiVhitOverVoltageFaultDiag_(uint8*const  output_pu8,
    uint8*const  size_pu8, uint8* const errCode_pu8)
{
  (void)size_pu8;
  (void)errCode_pu8;
  output_pu8[0] = 0x01; /* Example data */
  return E_OK;
}

Std_ReturnType SubfunctionRequestOutOfRange_(uint8*const  output_pu8,
    uint8*const  size_pu8, uint8* const errCode_pu8)
{
  (void)output_pu8;
  (void)size_pu8;
  if(NULL != errCode_pu8)
  {
    *errCode_pu8 = 0x12;
  }
  return E_NOT_OK;
}



