

#ifndef EDD_Crc8ComputeInternal_H
#define EDD_Crc8ComputeInternal_H

#include "errorDataDetection.h"
#include <stdint.h>

EDD_ReturnType EDD_VerifyCrc8(const uint8_t *data, uint8_t length, uint8_t expected_crc, uint8_t *result_out);

#endif