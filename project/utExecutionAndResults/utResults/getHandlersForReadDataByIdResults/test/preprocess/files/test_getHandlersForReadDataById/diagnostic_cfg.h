// CEEDLING NOTICE: This generated file only to be consumed by CMock

#ifndef _DIAGNOSTIC_CFG_H_ // Ceedling-generated include guard
#define _DIAGNOSTIC_CFG_H_

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint8_t Std_ReturnType;

void checkCurrentNad(uint8 currentNad, Std_ReturnType *result);

void checkMsgDataLength(uint16_t dataLength, Std_ReturnType *result);

typedef Std_ReturnType (*diagHandler_t)(uint8*const output_pu8, uint8*const size_pu8,
                                        uint8* const errCode_pu8);

Std_ReturnType RdbiVhitOverVoltageFaultDiag_(uint8*const output_pu8,
    uint8*const size_pu8, uint8* const errCode_pu8);

Std_ReturnType SubfunctionRequestOutOfRange_(uint8*const output_pu8,
    uint8*const size_pu8, uint8* const errCode_pu8);

#endif // _DIAGNOSTIC_CFG_H_
