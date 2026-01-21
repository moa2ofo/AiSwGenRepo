#include "errorDataDetection.h"
#include "errorDataDetection_priv.h"

/**
 * @file errorDataDetection.c
 * @brief Error Data Detection module implementation.
 *
 * @details
 * This source file provides the implementation of the CRC-8 services exposed
 * by the module public API.
 *
 * The CRC computation is optimized through the use of a static precomputed table
 * stored in ROM/Flash memory. The main public services are:
 * - CRC computation on a complete buffer
 * - CRC verification against an expected CRC value
 * - CRC update byte-by-byte (streaming)
 *
 * This module is standalone and suitable for embedded systems with strict
 * performance requirements.
 */

/* =============================
 *  Static Helpers Implementation
 * ============================= */

/**
 * @brief Compute CRC-8 over a data buffer (private).
 *
 * @details
 * Performs an optimized CRC-8 computation using a lookup table.
 * It assumes input parameters have already been validated by the caller.
 *
 * @param data   Pointer to input buffer.
 * @param length Size of input buffer in bytes.
 *
 * @return CRC-8 computed value.
 */
static uint8_t EDD_Crc8ComputeInternal(const uint8_t *data, uint8_t length) {
  uint8_t crc = EDD_CRC8_INIT;
  uint8_t i;

  for(i = 0u; i < length; i++) { crc = EDD_Crc8Table[crc ^ data[i]]; }

  return crc;
}

/* =============================
 *  Public API Implementation
 * ============================= */

EDD_ReturnType EDD_CalcCrc8(const uint8_t *data, uint8_t length, uint8_t *crc_out) {
  if((data == NULL) || (crc_out == NULL)) { return EDD_NULL_PTR; }

  if(length == 0u) { return EDD_INVALID_LENGTH; }

  *crc_out = EDD_Crc8ComputeInternal(data, length);

  return EDD_OK;
}

EDD_ReturnType EDD_VerifyCrc8(const uint8_t *data, uint8_t length, uint8_t expected_crc, uint8_t *result_out) {
  uint8_t computed;

  if((data == NULL) || (result_out == NULL)) { return EDD_NULL_PTR; }

  if(length == 0u) { return EDD_INVALID_LENGTH; }

  computed = EDD_Crc8ComputeInternal(data, length);

  *result_out = (computed == expected_crc) ? 1u : 0u;

  return EDD_OK;
}

EDD_ReturnType EDD_Crc8Update(uint8_t *crc_in_out, uint8_t byte) {
  if(crc_in_out == NULL) { return EDD_NULL_PTR; }

  *crc_in_out = EDD_Crc8Table[(*crc_in_out) ^ byte];

  return EDD_OK;
}
