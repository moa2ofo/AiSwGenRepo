#include "EDD_Crc8ComputeInternal.h"

/* FUNCTION TO TEST */

uint8_t EDD_Crc8ComputeInternal(const uint8_t *data, uint8_t length) {
  uint8_t crc = EDD_CRC8_INIT;
  uint8_t i;

  for(i = 0u; i < length; i++) { crc = EDD_Crc8Table[crc ^ data[i]]; }

  return crc;
}
