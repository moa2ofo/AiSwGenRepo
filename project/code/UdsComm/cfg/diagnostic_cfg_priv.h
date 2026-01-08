

#ifndef DIAGNOSTIC_CFG_PRIV_H
#define DIAGNOSTIC_CFG_PRIV_H
/**
 * @file diagnostic_cfg_priv.h
 * @brief Configuration and helper services for the Diagnostic module.
 *
 * @defgroup DiagnosticCfgModule Diagnostic Configuration
 * @{
 *
 * @details
 * This header collects the privare API of the diagnostic configuration layer.
 *
 */
#include "diagnostic_cfg.h"

#define DID_F308_SIZE 1U

typedef Std_ReturnType (*diagHandler_t)(uint8*const  output_pu8, uint8*const  size_pu8,
                                        uint8* const errCode_pu8);

/**
 * @brief DID handler that provides the Over Voltage Fault diagnostic information.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to populate the diagnostic output buffer with
 * the payload associated to the DID 0xF308 (Over Voltage Flag). This handler
 * provides a project-defined byte that represents the diagnostic state.
 *
 * The processing logic:
 * - Ignores `size_pu8` and `errCode_pu8` if not needed by this handler.
 * - Writes a constant example payload into `output_pu8[0]`.
 * - Returns `E_OK` to indicate that the DID payload was successfully produced.
 *
 * @par Interface summary
 *
 * | Interface     | In | Out | Data type / Signature               | Param | Data factor | Data offset | Data size | Data range      | Data unit |
 * |---------------|:--:|:---:|------------------------------------|:-----:|------------:|------------:|----------:|-----------------|----------|
 * | output_pu8    | X  |  X  | uint8*                              |   -   |      1      |      0      |     N     | project-defined | [-]      |
 * | size_pu8      | X  |     | uint8*                              |   -   |      1      |      0      |     1     | [0,255]         | [byte]   |
 * | errCode_pu8   | X  |  X  | uint8*                              |   -   |      1      |      0      |     1     | [0,255]         | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :output_pu8[0] = 0x01;
 * :return E_OK;
 * stop
 * @enduml
 *
 * @return Std_ReturnType.
 * - E_OK: payload produced successfully.
 * - E_NOT_OK: not used by this handler in current implementation.
 */
/**
 * @brief DID handler that provides the Over Voltage Fault diagnostic information.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to populate the diagnostic output buffer with
 * the payload associated to the DID 0xF308 (Over Voltage Flag). This handler
 * provides a project-defined byte that represents the diagnostic state.
 *
 * The processing logic:
 * - Writes the Over Voltage status into `output_pu8[0]`.
 * - Sets `*size_pu8 = 1` to indicate that one payload byte was written.
 * - Leaves `*errCode_pu8` unchanged because this is a positive path.
 * - Returns `E_OK`.
 *
 * @par Interface summary
 *
 * | Interface     | In | Out | Data type / Signature               | Param | Data factor | Data offset | Data size | Data range      | Data unit |
 * |---------------|:--:|:---:|------------------------------------|:-----:|------------:|------------:|----------:|-----------------|----------|
 * | output_pu8    | X  |  X  | uint8*                              |   -   |      1      |      0      |     N     | project-defined | [-]      |
 * | size_pu8      | X  |  X  | uint8*                              |   -   |      1      |      0      |     1     | [0,255]         | [byte]   |
 * | errCode_pu8   | X  |  X  | uint8*                              |   -   |      1      |      0      |     1     | [0,255]         | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :output_pu8[0] = <overvoltage_status>;
 * :*size_pu8 = 1;
 * :return E_OK;
 * stop
 * @enduml
 *
 * @return Std_ReturnType.
 * - E_OK: payload produced successfully.
 * - E_NOT_OK: handler failure (not expected in current implementation).
 */


Std_ReturnType RdbiVhitOverVoltageFaultDiag_(uint8*const  output_pu8,
    uint8*const  size_pu8, uint8* const errCode_pu8);

/**
 * @brief Default DID handler used for unsupported requests ("Request Out Of Range").
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to act as a fallback diagnostic handler when
 * the requested DID or subfunction is not supported. It provides the correct
 * NRC (Negative Response Code) by filling `*errCode_pu8` with the configured
 * error and returning `E_NOT_OK`.
 *
 * The processing logic:
 * - Ignores `output_pu8` and `size_pu8` because no payload is generated.
 * - If `errCode_pu8` is not NULL:
 *   - sets `*errCode_pu8 = 0x12` (Request Out Of Range).
 * - Returns `E_NOT_OK`.
 *
 * @par Interface summary
 *
 * | Interface     | In | Out | Data type / Signature               | Param | Data factor | Data offset | Data size | Data range      | Data unit |
 * |---------------|:--:|:---:|------------------------------------|:-----:|------------:|------------:|----------:|-----------------|----------|
 * | output_pu8    | X  |     | uint8*                              |   -   |      1      |      0      |     N     | project-defined | [-]      |
 * | size_pu8      | X  |     | uint8*                              |   -   |      1      |      0      |     1     | [0,255]         | [byte]   |
 * | errCode_pu8   | X  |  X  | uint8*                              |   -   |      1      |      0      |     1     | [0,255]         | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * if (errCode_pu8 != NULL) then (YES)
 *   :*errCode_pu8 = 0x12;
 * endif
 * :return E_NOT_OK;
 * stop
 * @enduml
 *
 * @return Std_ReturnType.
 * - E_NOT_OK: request is not supported.
 */
/**
 * @brief Default DID handler used for unsupported requests ("Request Out Of Range").
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to act as a fallback diagnostic handler when
 * the requested DID or subfunction is not supported. It provides the correct
 * NRC (Negative Response Code) by filling `*errCode_pu8` with the configured
 * error and returning `E_NOT_OK`.
 *
 * The processing logic:
 * - Does not write any payload to `output_pu8`.
 * - If `size_pu8` is not NULL, sets `*size_pu8 = 0`.
 * - If `errCode_pu8` is not NULL, sets `*errCode_pu8 = 0x12` (Request Out Of Range).
 * - Returns `E_NOT_OK`.
 *
 * @par Interface summary
 *
 * | Interface     | In | Out | Data type / Signature               | Param | Data factor | Data offset | Data size | Data range      | Data unit |
 * |---------------|:--:|:---:|------------------------------------|:-----:|------------:|------------:|----------:|-----------------|----------|
 * | output_pu8    | X  |     | uint8*                              |   -   |      1      |      0      |     N     | project-defined | [-]      |
 * | size_pu8      | X  |  X  | uint8*                              |   -   |      1      |      0      |     1     | [0,255]         | [byte]   |
 * | errCode_pu8   | X  |  X  | uint8*                              |   -   |      1      |      0      |     1     | [0,255]         | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * if (size_pu8 != NULL) then (YES)
 *   :*size_pu8 = 0;
 * endif
 * if (errCode_pu8 != NULL) then (YES)
 *   :*errCode_pu8 = 0x12;
 * endif
 * :return E_NOT_OK;
 * stop
 * @enduml
 *
 * @return Std_ReturnType.
 * - E_NOT_OK: request is not supported.
 */


Std_ReturnType SubfunctionRequestOutOfRange_(uint8*const  output_pu8,
    uint8*const  size_pu8, uint8* const errCode_pu8);

#endif