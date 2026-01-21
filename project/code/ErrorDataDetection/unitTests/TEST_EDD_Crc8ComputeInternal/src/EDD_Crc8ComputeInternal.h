

#ifndef EDD_Crc8ComputeInternal_H
#define EDD_Crc8ComputeInternal_H

#include "errorDataDetection_priv.h"
#include <stdint.h>

uint8_t EDD_Crc8ComputeInternal(const uint8_t *data, uint8_t length);

#endif