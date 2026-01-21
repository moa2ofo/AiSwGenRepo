#include "checkCurrentNad.h"

/* FUNCTION TO TEST */

void checkCurrentNad(uint8 currentNad, Std_ReturnType *result) {
  (void)currentNad;
  if(currentNad != 0x7F && currentNad != 0x7E) {
    *result = E_OK;
  } else {
    *result = E_NOT_OK;
  }
}
