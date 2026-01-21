
#ifndef DIAGNOSTIC_CFG_H
#define DIAGNOSTIC_CFG_H
/**
 * @file diagnostic_cfg.h
 * @brief Configuration and helper services for the Diagnostic module.
 *
 * @defgroup DiagnosticCfgModule Diagnostic Configuration
 * @{
 *
 * @details
 * This header collects the public API of the diagnostic configuration layer.
 * It is intended to be the single documentation entry point for the
 * diagnostic configuration module (diagnostic_cfg.c + diagnostic_cfg.h).
 *
 */

#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint8_t Std_ReturnType;

#define E_OK ((Std_ReturnType)0x00u)
#define E_NOT_OK ((Std_ReturnType)0x01u)
#define kLinDiagNrcRequestOutOfRange ((uint8)0x31u)

/**
 * @brief Check whether the current NAD is valid.
 *
 * @details
 * Evaluates the current node address (NAD) and writes the result into
 * @p result.
 *
 * @param currentNad Current NAD to be validated.
 * @param[out] result Pointer to the output status.
 */

void checkCurrentNad(uint8 currentNad, Std_ReturnType *result);

/**
 * @brief Check whether a diagnostic message data length is valid.
 *
 * @details
 * Verifies that @p dataLength matches the expected length constraints for
 * the diagnostic service and writes the check outcome into @p result.
 *
 * @param dataLength Message data length to be validated.
 * @param[out] result Pointer to the output status.
 */

void checkMsgDataLength(uint16_t dataLength, Std_ReturnType *result);

/**
 * @brief Retrieve handler callbacks for a ReadDataByIdentifier DID.
 *
 * @details
 * For the requested diagnostic identifier (DID), this function selects the
 * corresponding handler functions and fills the provided output pointers.
 * If the DID is not supported, an appropriate error code is stored in
 * @p l_errCode_u8.
 *
 * @param[out] l_errCode_u8 Pointer to the error code output.
 * @param l_did_cu16 Requested diagnostic identifier (DID).
 * @param[out] l_diagBufSize_u8 Pointer to the expected diagnostic buffer size.
 * @param[out] l_didSupported_ Pointer to DID supported flag.
 * @param[out] l_diagBuf_pu8 Pointer to the diagnostic buffer.
 *
 * @return Standard return status.
 */

Std_ReturnType getHandlersForReadDataById(uint8 *l_errCode_u8, uint16 l_did_cu16, uint8 *l_diagBufSize_u8, Std_ReturnType *l_didSupported_, uint8 *l_diagBuf_pu8);

/** @} */

#endif