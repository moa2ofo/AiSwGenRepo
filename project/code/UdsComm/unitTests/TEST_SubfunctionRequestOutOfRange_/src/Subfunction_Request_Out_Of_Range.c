#include "Subfunction_Request_Out_Of_Range.h"

/* FUNCTION TO TEST */

Std_ReturnType Subfunction_Request_Out_Of_Range(uint8 *const output_pu8, uint8 *const size_pu8, uint8 *const errCode_pu8) {
  (void)output_pu8;
  (void)size_pu8;
  if(NULL != errCode_pu8) { *errCode_pu8 = 0x12; }
  return E_NOT_OK;
}