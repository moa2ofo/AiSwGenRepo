#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <stdint.h>

/**
 * @file Diagnostic.h
 * @brief Public interface for LIN diagnostic services and shared diagnostic buffers.
 *
 * @details
 * This header exposes the global LIN diagnostic buffer and the associated message
 * length used by the diagnostic stack, together with the application-level
 * service handler(s).
 *
 * **Data flow overview**
 * - Incoming diagnostic requests are stored in `pbLinDiagBuffer`.
 * - `g_linDiagDataLength` represents the current request/response length (in bytes),
 *   interpreted by the diagnostic services.
 * - Service handlers (e.g. ReadDataByIdentifier 0x22) parse the request fields from
 *   `pbLinDiagBuffer` and build the response payload in-place.
 *
 * @note
 * The diagnostic buffer is shared across multiple services. Callers must ensure
 * that concurrent access is prevented (e.g. by design, scheduling, or protection).
 */

/**
 * @brief Shared LIN diagnostic buffer.
 *
 * @details
 * Fixed-size buffer used by the LIN diagnostic layer for both requests and responses.
 * The layout is service-dependent; for service 0x22 the DID is expected at:
 * - `pbLinDiagBuffer[1]` = DID MSB
 * - `pbLinDiagBuffer[2]` = DID LSB
 * - `pbLinDiagBuffer[3..]` = response payload area
 *
 * @par Interface summary
 *
 * | Interface             | In | Out | Data type | Param | Data factor | Data offset | Data size | Data range | Data unit |
 * |-----------------------|:--:|:---:|-----------|:-----:|------------:|------------:|----------:|------------|----------|
 * | pbLinDiagBuffer[32]   | X  |  X  | uint8[]   |   -   |      1      |      0      |     32    | [0,255]    | [-]      |
 *
 * @warning
 * Buffer overrun must be prevented by all services writing into this array.
 */
extern uint8_t pbLinDiagBuffer[32];

/**
 * @brief Current LIN diagnostic message length.
 *
 * @details
 * Length in bytes of the active diagnostic message associated with `pbLinDiagBuffer`.
 * Depending on the current phase, it may represent:
 * - received request length before service execution, or
 * - response length after service execution.
 *
 * @par Interface summary
 *
 * | Interface              | In | Out | Data type | Param | Data factor | Data offset | Data size | Data range   | Data unit |
 * |------------------------|:--:|:---:|-----------|:-----:|------------:|------------:|----------:|--------------|----------|
 * | g_linDiagDataLength    | X  |  X  | uint16    |   -   |      1      |      0      |     1     | [0,65535]    | [byte]   |
 */
extern uint16_t g_linDiagDataLength;

/**
 * @brief Handle LIN diagnostic service "ReadDataByIdentifier" (0x22).
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to decode the requested DID (Data Identifier)
 * from the LIN diagnostic request buffer, validate the request (target NAD and
 * expected message length), retrieve the DID payload through the configured
 * handler, and finally send either a positive response (with DID + data) or a
 * negative response (with the detected error code).
 *
 * The processing logic:
 * - Extracts the DID from `pbLinDiagBuffer[1]` (MSB) and `pbLinDiagBuffer[2]` (LSB).
 * - Validates that the current request is addressed to the correct NAD.
 * - Validates the received request length (`g_linDiagDataLength`).
 * - Calls the DID handler dispatcher to:
 *   - determine whether the DID is supported,
 *   - fill the response data into the diagnostic buffer starting at `pbLinDiagBuffer[3]`,
 *   - return the number of payload bytes written.
 * - If processing is successful, updates `g_linDiagDataLength` to `payloadLen + 2`
 *   (DID bytes) and sends a positive response.
 * - Otherwise, sends a negative response using the error code.
 *
 * @par Interface summary
 *
 * | Interface                               | In | Out | Data type / Signature                                                | Param | Data factor | Data offset | Data size | Data range      | Data unit |
 * |-----------------------------------------|:--:|:---:|----------------------------------------------------------------------|:-----:|------------:|------------:|----------:|-----------------|----------|
 * | pbLinDiagBuffer[1]                      | X  |     | uint8                                                                |   -   |      1      |      0      |     1     | [0,255]         | [-]      |
 * | pbLinDiagBuffer[2]                      | X  |     | uint8                                                                |   -   |      1      |      0      |     1     | [0,255]         | [-]      |
 * | pbLinDiagBuffer[3..]                    | X  |  X  | uint8[]                                                              |   -   |      1      |      0      |     N     | project-defined | [-]      |
 * | g_linDiagDataLength                     | X  |  X  | uint16                                                              |   -   |      1      |      0      |     1     | [0,65535]       | [byte]   |
 * | checkCurrentNad()                       | X  |  X  | void(uint8 nad, Std_ReturnType *result)                               |   -   |      -      |      -      |     -     |                | [-]      |
 * | checkMsgDataLength()                    | X  |  X  | void(uint16 len, Std_ReturnType *result)                              |   -   |      -      |      -      |     -     |               | [-]      |
 * | getHandlersForReadDataById()             | X  |  X  | Std_ReturnType(uint8, uint16, uint8*, Std_ReturnType*, uint8*)        |   -   |      -      |      -      |     -     | E_OK/E_NOT_OK   | [-]      |
 * | LinDiagSendPosResponse()                |    |  X  | void(void)                                                          |   -   |      -      |      -      |     -     | -               | [-]      |
 * | LinDiagSendNegResponse()                | X  |  X  | void(uint8 errorCode)                                                |   -   |      -      |      -      |     -     | -               | [-]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :Read DID from pbLinDiagBuffer[1..2];
 * :l_result = E_OK;
 * :l_errCode = 0;
 * :l_diagBuf = &pbLinDiagBuffer[3];
 * :l_diagBufSize = 0;
 *
 * :checkCurrentNad(0, &l_result);
 * if (l_result == E_OK) then (OK)
 *   :checkMsgDataLength(g_linDiagDataLength, &l_result);
 * endif
 *
 * if (l_result == E_OK) then (OK)
 *   :getHandlersForReadDataById(l_errCode, l_did,
 *                               &l_diagBufSize,
 *                               &l_didSupported,
 *                               l_diagBuf);
 * endif
 *
 * if (l_result == E_OK) then (POS)
 *   :g_linDiagDataLength = l_diagBufSize + 2;
 *   :LinDiagSendPosResponse();
 * else (NEG)
 *   :LinDiagSendNegResponse(l_errCode);
 * endif
 * stop
 * @enduml
 *
 * @return None.
 * The function sends a LIN diagnostic response and may update
 * `g_linDiagDataLength` and the payload area `pbLinDiagBuffer[3..]`
 * depending on the outcome.
 */
void ApplLinDiagReadDataById(void);

#endif /* DIAGNOSTIC_H */
