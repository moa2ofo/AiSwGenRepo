#include "getHandlersForReadDataById.h"
#include "diagnostic_cfg.h"

uint8 g_errCode_u8 = 0;
uint16 g_did_cu16 = 0;
/* FUNCTION TO TEST */


Std_ReturnType getHandlersForReadDataById(uint8 * l_errCode_u8, uint16 l_did_cu16,  uint8 *l_diagBufSize_u8, Std_ReturnType *l_didSupported_,  uint8 *l_diagBuf_pu8) {
    diagHandler_t l_handler_ = &SubfunctionRequestOutOfRange_;

    
    switch (l_did_cu16)
    {
    /* IS_OVERVOLT_FLAG */
    case 0xF308:
        *l_diagBufSize_u8 = DID_F308_SIZE;
        l_handler_ = &RdbiVhitOverVoltageFaultDiag_;
        break;

    default:
        *l_didSupported_  = E_NOT_OK;
        *l_errCode_u8 = kLinDiagNrcRequestOutOfRange;
        break;
    }
    
    return l_handler_(l_diagBuf_pu8, l_diagBufSize_u8, &l_errCode_u8);
}
