#ifndef VOLT_MONITORING_CFG_H
#define VOLT_MONITORING_CFG_H

#include <stdint.h>

/**
 * @file VoltMonitoring_cfg.h
 * @brief Configuration interface for the Voltage Monitoring module.
 *
 * @details
 * This header defines all project-specific configuration parameters required by
 * the Voltage Monitoring module.
 *
 * The configuration layer is responsible for:
 * - Providing the project-specific supply voltage reading interface.
 * - Defining undervoltage and overvoltage thresholds.
 * - Defining hysteresis and timing parameters used by the monitoring state machine.
 *
 * All symbols declared in this file must be defined in the corresponding
 * `VoltMonitoring_cfg.c` file.
 *
 * @note
 * No application logic shall be implemented in this file.
 * This file must only contain configuration data and interfaces.
 */

/*==============================================================================
 * Voltage reading interface
 *============================================================================*/

/**
 * @brief Project-specific macro to read the supply voltage in millivolts.
 *
 * @details
 * This macro abstracts the project-specific voltage acquisition function.
 * It allows the Voltage Monitoring platform code to remain independent from
 * the actual ADC / HAL / hardware implementation.
 *
 * The macro shall expand to a function call returning the measured voltage
 * expressed in millivolts.
 *
 * @return Supply voltage in millivolts [mV].
 */
#define READ_VOLT_PROJECT_MV VoltMon_ReadVoltageProject_mV()

/*==============================================================================
 * Voltage thresholds configuration
 *============================================================================*/

/**
 * @brief Undervoltage threshold in millivolts.
 *
 * @details
 * If the measured supply voltage is below this threshold for at least
 * `VoltMon_ActivationTime_ms`, an undervoltage condition is detected.
 *
 * Typical value: 8000 mV.
 */
extern const uint16_t VoltMon_ThresholdUnder_mV;

/**
 * @brief Overvoltage threshold in millivolts.
 *
 * @details
 * If the measured supply voltage is above this threshold for at least
 * `VoltMon_ActivationTime_ms`, an overvoltage condition is detected.
 *
 * Typical value: 13000 mV.
 */
extern const uint16_t VoltMon_ThresholdOver_mV;

/**
 * @brief Voltage hysteresis in millivolts.
 *
 * @details
 * This parameter defines the hysteresis band applied when recovering from
 * undervoltage or overvoltage conditions.
 *
 * Typical value: 500 mV.
 */
extern const uint16_t VoltMon_Hysteresis_mV;

/*==============================================================================
 * Timing configuration
 *============================================================================*/

/**
 * @brief Activation time in milliseconds.
 *
 * @details
 * Minimum time the voltage must continuously violate a threshold before
 * entering an undervoltage or overvoltage state.
 *
 * Typical value: 500 ms.
 */
extern const uint16_t VoltMon_ActivationTime_ms;

/**
 * @brief Deactivation time in milliseconds.
 *
 * @details
 * Minimum time the voltage must continuously remain inside the safe range
 * before returning to the NORMAL state.
 *
 * Typical value: 500 ms.
 */
extern const uint16_t VoltMon_DeactivationTime_ms;

/**
 * @brief Voltage monitoring task period in milliseconds.
 *
 * @details
 * Period at which the voltage monitoring function (`voltMonRun`) is called.
 * This value may be used to convert time-based parameters (ms) into
 * execution cycles if required.
 *
 * Typical value: application dependent.
 */
extern const uint16_t VoltMon_TaskPeriod_ms;

/*==============================================================================
 * Project voltage reading function
 *============================================================================*/

/**
 * @brief Read and condition the project supply voltage in millivolts.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to provide a robust project-specific voltage
 * measurement (in mV) by selecting/combining two internal voltage samples:
 * an unfiltered value (`supplyDcNotFiler_u16`) and a filtered value
 * (`supplyDcFiler_u16`).
 *
 *
 * @par Interface summary
 *
 * | Interface                                   | In | Out | Data type   | Param | Data factor | Data offset | Data size | Data range     | Data unit |
 * |---------------------------------------------|:--:|:---:|-------------|-------|------------:|------------:|----------:|----------------|-----------|
 * | supplyDcNotFiler_u16                        | X  |     | uint16      |   -   |      1      |           0 |         1 | [0, 65535]     | [mV]      |
 * | supplyDcFiler_u16                           | X  |     | uint16      |   -   |      1      |           0 |         1 | [0, 65535]     | [mV]      |
 * | lowerVoltMonCfg_cu16                        | X  |     | const uint16|   -   |      1      |           0 |         1 | [0, 20000]     | [mV]      |
 * | middleLowerVoltMonCfg_cu16                  | X  |     | const uint16|   -   |      1      |           0 |         1 | [0, 20000]     | [mV]      |
 * | middleHigherVoltMonCfg_cu16                 | X  |     | const uint16|   -   |      1      |           0 |         1 | [0, 20000]     | [mV]      |
 * | higherVoltMonCfg_cu16                       | X  |     | const uint16|   -   |      1      |           0 |         1 | [0, 20000]     | [mV]      |
 * | VoltMon_ReadVoltageProject_mV()             |    | X   | uint16(void)|   -   |      1      |           0 |         1 | [0, 19999]     | [mV]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :Read l_supplyDcNotFiler_u16;
 * :Read l_supplyDcFiler_u16;
 * :Read limits (lower, midLow, midHigh, higher);
 *
 * if (l_supplyDcNotFiler_u16 > lower && l_supplyDcNotFiler_u16 < midLow) then (LOW BAND)
 *   :l_voltage_mV = l_supplyDcNotFiler_u16;
 * else if (l_supplyDcNotFiler_u16 >= midLow && l_supplyDcNotFiler_u16 <= midHigh) then (MIDDLE BAND)
 *   :l_voltage_mV = (l_supplyDcFiler_u16 + l_supplyDcNotFiler_u16) >> 1;
 * else if (l_supplyDcNotFiler_u16 > midHigh && l_supplyDcNotFiler_u16 < higher) then (HIGH BAND)
 *   :l_voltage_mV = (l_supplyDcFiler_u16 + l_supplyDcNotFiler_u16) >> 1;
 * else (OUT OF RANGE)
 *   :l_voltage_mV = 0;
 * endif
 *
 * :return l_voltage_mV;
 * stop
 * @enduml
 *
 * @return Conditioned project voltage in millivolts.
 * Returns 0u if the unfiltered voltage is outside the configured valid range.
 */
uint16_t VoltMon_ReadVoltageProject_mV(void);

#endif /* VOLT_MONITORING_CFG_H */
