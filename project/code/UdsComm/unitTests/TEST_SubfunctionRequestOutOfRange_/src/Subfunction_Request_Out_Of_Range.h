
#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H


#include "diagnostic_cfg.h"
#include <stdint.h>

Std_ReturnType Subfunction_Request_Out_Of_Range(uint8*const  output_pu8,
    uint8*const  size_pu8, uint8* const errCode_pu8);
 
#endif