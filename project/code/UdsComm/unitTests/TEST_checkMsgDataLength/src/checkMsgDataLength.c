#include "checkMsgDataLength.h"

/* FUNCTION TO TEST */

void checkMsgDataLength(uint16_t dataLength, Std_ReturnType *result) {
  if(dataLength > 0u && dataLength <= 32u) {
    *result = E_OK;
  } else {
    *result = E_NOT_OK;
  }
}
