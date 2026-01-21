
#ifndef DIAGNOSTIC_CFG_H
#define DIAGNOSTIC_CFG_H

#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint8_t Std_ReturnType;

#define E_OK ((Std_ReturnType)0x00u)
#define E_NOT_OK ((Std_ReturnType)0x01u)
#define kLinDiagNrcRequestOutOfRange ((uint8)0x31u)

void checkCurrentNad(uint8 currentNad, Std_ReturnType *result);

void checkMsgDataLength(uint16_t dataLength, Std_ReturnType *result);

#define DID_F308_SIZE 1U

typedef Std_ReturnType (*diagHandler_t)(uint8 *const output_pu8, uint8 *const size_pu8, uint8 *const errCode_pu8);

Std_ReturnType RdbiVhitOverVoltageFaultDiag_(uint8 *const output_pu8, uint8 *const size_pu8, uint8 *const errCode_pu8);

Std_ReturnType SubfunctionRequestOutOfRange_(uint8 *const output_pu8, uint8 *const size_pu8, uint8 *const errCode_pu8);

#endif