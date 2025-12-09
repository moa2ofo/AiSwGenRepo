// CEEDLING NOTICE: This generated file only to be consumed by CMock

#ifndef _DIAGNOSTIC_CFG_H_ // Ceedling-generated include guard
#define _DIAGNOSTIC_CFG_H_

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint8_t Std_ReturnType;

void checkCurrentNad(uint8 currentNad, Std_ReturnType *result);

void checkMsgDataLength(uint16_t dataLength, Std_ReturnType *result);

Std_ReturnType getHandlersForReadDataById(uint8 l_did_u8, uint16 l_did_cu16, uint8 *l_diagBufSize_u8, Std_ReturnType *l_didSupported_, uint8 *l_diagBuf_pu8);

#endif // _DIAGNOSTIC_CFG_H_
