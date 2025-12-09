
#ifndef DIAGNOSTIC_CFG_H
#define DIAGNOSTIC_CFG_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint8_t Std_ReturnType;

extern Std_ReturnType checkCurrentNad_return_stub ;
extern Std_ReturnType checkMsgDataLength_return_stub ;
extern Std_ReturnType getHandlersForReadDataById_return_stub;
extern bool ckCurrentNad_called;
extern bool checkMsgDataLength_called;
extern bool getHandlersForReadDataById_called;

#define E_OK                               ((Std_ReturnType)0x00u)
#define E_NOT_OK                           ((Std_ReturnType)0x01u)
#define kLinDiagNrcRequestOutOfRange       ((uint8)0x31u)

void checkCurrentNad(uint8 currentNad, Std_ReturnType *result);

void checkMsgDataLength(uint16_t dataLength, Std_ReturnType *result);

Std_ReturnType getHandlersForReadDataById(uint8 l_did_u8, uint16 l_did_cu16,  uint8 *l_diagBufSize_u8, Std_ReturnType *l_didSupported_,  uint8 *l_diagBuf_pu8);

#endif