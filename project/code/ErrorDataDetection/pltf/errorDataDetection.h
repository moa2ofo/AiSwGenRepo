#ifndef ERRORDATADETECTION_H
#define ERRORDATADETECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @file errorDataDetection.h
 * @brief Error Data Detection module public interface.
 *
 * @details
 * This module provides CRC-8 computation and verification services for embedded
 * applications. The CRC is computed using a static lookup table (256 entries),
 * ensuring optimized runtime performance compared to bitwise CRC computation.
 *
 * The module is designed to be fully standalone:
 * - No dynamic memory allocation.
 * - No dependency on external drivers or OS.
 * - Suitable for use in safety-related modules and runtime constrained ECU environments.
 *
 * The CRC implementation follows the CRC-8/ATM standard:
 * - Polynomial: 0x07
 * - Init value: 0x00
 * - RefIn/RefOut: false
 * - XOROut: 0x00
 */

/* =============================
 *  Error Codes
 * ============================= */

/**
 * @brief Return values used by Error Data Detection APIs.
 */
typedef enum {
  EDD_OK = 0,        /**< Operation completed successfully. */
  EDD_NULL_PTR,      /**< A required pointer argument was NULL. */
  EDD_INVALID_LENGTH /**< Provided length was invalid (e.g. 0). */
} EDD_ReturnType;

/* =============================
 *  Public API
 * ============================= */

/**
 * @brief Compute CRC-8 (lookup table based) over a given data buffer.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to compute an 8-bit CRC over a memory buffer
 * using a precomputed lookup table (static 256-byte table). The lookup table
 * approach reduces CPU cycles compared to polynomial bitwise computation.
 *
 * The processing logic:
 * - Validates the input pointers (`data`, `crc_out`).
 * - Validates that `length` is greater than zero.
 * - Initializes CRC accumulator with CRC init value (0x00).
 * - For each byte in the buffer:
 *   - Computes table index as `crc XOR data[i]`.
 *   - Updates CRC with `table[index]`.
 * - Writes final CRC into `*crc_out`.
 *
 * @par Interface summary
 *
 * | Interface  | In | Out | Data type / Signature        | Param | Data factor | Data offset | Data size | Data range     | Data unit |
 * |-----------|:--:|:---:|------------------------------|:-----:|------------:|------------:|----------:|----------------|----------|
 * | data      | X  |     | const uint8_t*               |   -   |      1      |      0      |  length  | [0,255]        | [-]      |
 * | length    | X  |     | uint8_t                       |   -   |      1      |      0      |     -    | [1..N]         | bytes    |
 * | crc_out   | X  |  X  | uint8_t*                     |   -   |      1      |      0      |     1    | [0,255]        | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :Validate pointers;
 * if (data == NULL OR crc_out == NULL) then (YES)
 *   :return EDD_NULL_PTR;
 *   stop
 * endif
 * :Validate length;
 * if (length == 0) then (YES)
 *   :return EDD_INVALID_LENGTH;
 *   stop
 * endif
 * :crc = 0x00;
 * while (i < length)
 *   :crc = Table[crc XOR data[i]];
 * endwhile
 * :*crc_out = crc;
 * :return EDD_OK;
 * stop
 * @enduml
 *
 * @return EDD_ReturnType
 * - EDD_OK if CRC was computed successfully.
 * - EDD_NULL_PTR if any input pointer is NULL.
 * - EDD_INVALID_LENGTH if length is 0.
 */
EDD_ReturnType EDD_CalcCrc8(const uint8_t *data, uint8_t length, uint8_t *crc_out);

/**
 * @brief Verify that a buffer CRC matches an expected CRC.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to validate integrity of a received buffer
 * by computing the CRC-8 over the buffer and comparing it against the provided
 * `expected_crc`.
 *
 * The processing logic:
 * - Validates pointer arguments (`data`, `result_out`).
 * - Validates that `length` is greater than zero.
 * - Computes CRC-8 over the buffer (table based).
 * - Compares computed CRC with `expected_crc`.
 * - Writes the verification result:
 *   - `*result_out = 1` if CRC matches.
 *   - `*result_out = 0` otherwise.
 *
 * @par Interface summary
 *
 * | Interface      | In | Out | Data type / Signature  | Param | Data factor | Data offset | Data size | Data range        | Data unit |
 * |---------------|:--:|:---:|------------------------|:-----:|------------:|------------:|----------:|-------------------|----------|
 * | data          | X  |     | const uint8_t*         |   -   |      1      |      0      |  length  | [0,255]           | [-]      |
 * | length        | X  |     | uint8_t                 |   -   |      1      |      0      |     -    | [1..N]            | bytes    |
 * | expected_crc  | X  |     | uint8_t                |   -   |      1      |      0      |     1    | [0,255]           | [-]      |
 * | result_out    | X  |  X  | uint8_t*               |   -   |      1      |      0      |     1    | {0,1}             | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :Validate pointers;
 * if (data == NULL OR result_out == NULL) then (YES)
 *   :return EDD_NULL_PTR;
 *   stop
 * endif
 * :Validate length;
 * if (length == 0) then (YES)
 *   :return EDD_INVALID_LENGTH;
 *   stop
 * endif
 * :computed = CalcCrc8(data, length);
 * if (computed == expected_crc) then (YES)
 *   :*result_out = 1;
 * else (NO)
 *   :*result_out = 0;
 * endif
 * :return EDD_OK;
 * stop
 * @enduml
 *
 * @return EDD_ReturnType
 * - EDD_OK if verification executed successfully.
 * - EDD_NULL_PTR if any required pointer is NULL.
 * - EDD_INVALID_LENGTH if length is 0.
 */
EDD_ReturnType EDD_VerifyCrc8(const uint8_t *data, uint8_t length, uint8_t expected_crc, uint8_t *result_out);

/**
 * @brief Update CRC-8 accumulator with a new byte (streaming support).
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to allow incremental CRC computation for
 * streaming scenarios (e.g., CRC over a frame received byte-by-byte).
 *
 * The processing logic:
 * - Validates input pointer (`crc_in_out`).
 * - Computes table index as `(*crc_in_out) XOR byte`.
 * - Updates CRC accumulator with `table[index]`.
 *
 * @par Interface summary
 *
 * | Interface     | In | Out | Data type / Signature | Param | Data factor | Data offset | Data size | Data range | Data unit |
 * |--------------|:--:|:---:|-----------------------|:-----:|------------:|------------:|----------:|-----------|----------|
 * | crc_in_out   | X  |  X  | uint8_t*              |   -   |      1      |      0      |     1    | [0,255]   | [-]      |
 * | byte         | X  |     | uint8_t               |   -   |      1      |      0      |     1    | [0,255]   | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :Validate crc_in_out pointer;
 * if (crc_in_out == NULL) then (YES)
 *   :return EDD_NULL_PTR;
 *   stop
 * endif
 * :idx = (*crc_in_out) XOR byte;
 * :*crc_in_out = Table[idx];
 * :return EDD_OK;
 * stop
 * @enduml
 *
 * @return EDD_ReturnType
 * - EDD_OK if update executed successfully.
 * - EDD_NULL_PTR if crc_in_out is NULL.
 */
EDD_ReturnType EDD_Crc8Update(uint8_t *crc_in_out, uint8_t byte);

#ifdef __cplusplus
}
#endif

#endif /* ERRORDATADETECTION_H */
