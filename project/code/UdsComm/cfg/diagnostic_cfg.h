
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

#define E_OK                               ((Std_ReturnType)0x00u)
#define E_NOT_OK                           ((Std_ReturnType)0x01u)
#define kLinDiagNrcRequestOutOfRange       ((uint8)0x31u)

/**
 * @brief Validate that the LIN diagnostic request is addressed to the expected NAD.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to verify that the current request NAD matches
 * the node address supported by this ECU. If the NAD is correct the function
 * reports success, otherwise it reports failure.
 *
 * The processing logic:
 * - Reads the `currentNad` value passed by the caller.
 * - Compares `currentNad` against the expected NAD value (0x7F).
 * - If the NAD matches:
 *   - sets `*result = E_OK`.
 * - Otherwise:
 *   - sets `*result = E_NOT_OK`.
 *
 * @par Interface summary
 *
 * | Interface        | In | Out | Data type / Signature                        | Param | Data factor | Data offset | Data size | Data range | Data unit |
 * |------------------|:--:|:---:|---------------------------------------------|:-----:|------------:|------------:|----------:|-----------|----------|
 * | currentNad       | X  |     | uint8                                       |   -   |      1      |      0      |     1     | [0,255]   | [-]      |
 * | result           | X  |  X  | Std_ReturnType*                              |   -   |      -      |      -      |     -     | E_OK/E_NOT_OK | [-]   |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :Compare currentNad with 0x7F;
 * if (currentNad == 0x7F) then (YES)
 *   :*result = E_OK;
 * else (NO)
 *   :*result = E_NOT_OK;
 * endif
 * stop
 * @enduml
 *
 * @return None.
 * The function writes the outcome into `*result`.
 */
void checkCurrentNad(uint8 currentNad, Std_ReturnType *result);

/**
 * @brief Validate the received LIN diagnostic message length.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to verify that the received diagnostic message
 * length is within the supported range. The function ensures that the message
 * contains at least one byte and does not exceed the maximum supported payload.
 *
 * The processing logic:
 * - Reads the `dataLength` input value.
 * - Checks whether `dataLength` is greater than 0.
 * - Checks whether `dataLength` is less than or equal to 32 bytes.
 * - If both checks are true:
 *   - sets `*result = E_OK`.
 * - Otherwise:
 *   - sets `*result = E_NOT_OK`.
 *
 * @par Interface summary
 *
 * | Interface   | In | Out | Data type / Signature         | Param | Data factor | Data offset | Data size | Data range     | Data unit |
 * |-------------|:--:|:---:|------------------------------|:-----:|------------:|------------:|----------:|---------------|----------|
 * | dataLength  | X  |     | uint16_t                      |   -   |      1      |      0      |     1     | [0,65535]      | [byte]   |
 * | result      | X  |  X  | Std_ReturnType*               |   -   |      -      |      -      |     -     | E_OK/E_NOT_OK  | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :Check (dataLength > 0) AND (dataLength <= 32);
 * if (valid length) then (YES)
 *   :*result = E_OK;
 * else (NO)
 *   :*result = E_NOT_OK;
 * endif
 * stop
 * @enduml
 *
 * @return None.
 * The function writes the result into `*result`.
 */
void checkMsgDataLength(uint16_t dataLength, Std_ReturnType *result);

/**
 * @brief Dispatch the handler associated with a ReadDataByIdentifier DID request.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to select and execute the correct DID handler
 * for a "ReadDataByIdentifier" diagnostic request. The dispatcher evaluates the
 * requested DID and, if supported, configures the expected payload size and calls
 * the corresponding handler to fill the response buffer. If the DID is not
 * supported, it reports a negative response condition and provides the NRC code.
 *
 * The processing logic:
 * - Initializes the handler to `SubfunctionRequestOutOfRange_`.
 * - Evaluates the requested DID (`l_did_cu16`) using a switch statement.
 * - If DID is supported (0xF308):
 *   - sets `*l_diagBufSize_u8 = DID_F308_SIZE`.
 *   - sets handler to `RdbiVhitOverVoltageFaultDiag_`.
 * - Otherwise:
 *   - sets `*l_didSupported_ = E_NOT_OK`.
 *   - sets `*l_errCode_u8 = kLinDiagNrcRequestOutOfRange`.
 * - Finally calls the selected handler:
 *   - handler writes payload into `l_diagBuf_pu8` if supported,
 *   - handler may update the error code.
 *
 * @par Interface summary
 *
 * | Interface           | In | Out | Data type / Signature                                      | Param | Data factor | Data offset | Data size | Data range      | Data unit |
 * |---------------------|:--:|:---:|-----------------------------------------------------------|:-----:|------------:|------------:|----------:|-----------------|----------|
 * | l_errCode_u8        | X  |  X  | uint8*                                                    |   -   |      1      |      0      |     1     | [0,255]         | [-]      |
 * | l_did_cu16          | X  |     | uint16                                                   |   -   |      1      |      0      |     1     | [0,65535]       | [-]      |
 * | l_diagBufSize_u8    | X  |  X  | uint8*                                                    |   -   |      1      |      0      |     1     | [0,255]         | [byte]   |
 * | l_didSupported_     | X  |  X  | Std_ReturnType*                                          |   -   |      -      |      -      |     -     | E_OK/E_NOT_OK   | [-]      |
 * | l_diagBuf_pu8       | X  |  X  | uint8*                                                    |   -   |      1      |      0      |     N     | project-defined | [-]      |
 * | RdbiVhitOverVoltageFaultDiag_ | X | X | Std_ReturnType(uint8*,uint8*,uint8*)                 |   -   |      -      |      -      |     -     | E_OK/E_NOT_OK   | [-]      |
 * | SubfunctionRequestOutOfRange_ | X | X | Std_ReturnType(uint8*,uint8*,uint8*)                |   -   |      -      |      -      |     -     | E_OK/E_NOT_OK   | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
  * @startuml
 * start
 * :l_handler = SubfunctionRequestOutOfRange_;
 *
 * if (l_did_cu16 == 0xF308) then (YES)
 *   :*l_diagBufSize_u8 = DID_F308_SIZE;
 *   :l_handler = RdbiVhitOverVoltageFaultDiag_;
 * else (NO)
 *   :*l_didSupported_ = E_NOT_OK;
 *   :*l_errCode_u8 = kLinDiagNrcRequestOutOfRange;
 * endif
 *
 * :return l_handler(l_diagBuf_pu8, l_diagBufSize_u8, &l_errCode_u8);
 * stop
 * @enduml
 *
 * @return Std_ReturnType.
 * - E_OK: handler executed successfully.
 * - E_NOT_OK: unsupported DID or handler failure.
 */
Std_ReturnType getHandlersForReadDataById(uint8 * l_errCode_u8, uint16 l_did_cu16,  uint8 *l_diagBufSize_u8, Std_ReturnType *l_didSupported_,  uint8 *l_diagBuf_pu8);

/** @} */

#endif