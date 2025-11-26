#ifndef CALC_ECU_TEMP_H
#define CALC_ECU_TEMP_H

#include <stdint.h>

/**
 * @file calcEcuTemp.h
 * @brief ECU temperature calculation interface and configuration parameters.
 *
 * This header provides the API and configuration thresholds used to compute
 * the ECU temperature estimate in @ref Calc_ecuTemperature_s16.
 * The function combines an NTC-based temperature measurement with a system
 * temperature measurement and applies a piecewise strategy:
 * - Use the system temperature at low temperatures or if the NTC is faulty.
 * - Use the NTC temperature at high temperatures.
 * - Blend the two measurements linearly in a middle range.
 */

/**
 * @brief Lower threshold for ECU temperature calculation [°C].
 *
 * Below or equal to this value, the ECU temperature estimate falls back
 * to the system temperature measurement (or if the NTC is reported faulty).
 */
#define PAR_ECU_TEMP_LOW_THR 30

/**
 * @brief Upper threshold for ECU temperature calculation [°C].
 *
 * Above or equal to this value, the ECU temperature estimate is taken
 * directly from the NTC temperature measurement. Between
 * @ref PAR_ECU_TEMP_LOW_THR and @ref PAR_ECU_TEMP_UP_THR, a linear
 * interpolation between NTC and system temperature is applied.
 */
#define PAR_ECU_TEMP_UP_THR 40

/** AUTOSAR-like type alias for signed 16-bit integer. */
typedef int16_t sint16;

/**
 * @brief Compute ECU temperature estimate from NTC and system temperatures.
 *
 * @details
 * **Goal of the function**
 *
 * The purpose of this function is to provide a robust estimate of the ECU
 * temperature by combining:
 * - An NTC-based temperature measurement (fast, but can fail or be noisy).
 * - A system temperature measurement (more stable, backup in case of NTC fault).
 *
 * @par Interface summary
 *
 * | Interface              | In | Out | Data type | Param | Data factor | Data offset | Data size | Data range   | Data unit |
 * |------------------------|:--:|:---:|-----------|-------|------------:|------------:|----------:|--------------|-----------|
 * | NtcTemp_s16            | ✓  |     | sint16    |   –   |           1 |           0 |         1 | [-40, 140]   | [°C]      |
 * | SysTemp_s16            | ✓  |     | sint16    |   –   |           1 |           0 |         1 | [-40, 140]   | [°C]      |
 * | NtcErr_b               | ✓  |     | bool      |   –   |           1 |           0 |         1 | [0, 1]       | [-]       |
 * | l_ecuTemp_s16          |    |  ✓  | sint16    |   –   |           1 |           0 |         1 | [-40, 140]   | [°C]      |
 * | PAR_ECU_TEMP_LOW_THR   | ✓  |     | macro     |   –   |           1 |           0 |         1 | [30]         | [°C]      |
 * | PAR_ECU_TEMP_UP_THR    | ✓  |     | macro     |   –   |           1 |           0 |         1 | [40]         | [°C]      |
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 * :Init l_ecuTemp_s16 equal to 0;
 * if (NtcTemp_s16 <= PAR_ECU_TEMP_LOW_THR \n   OR NtcErr_b == true) then (yes)
 *   :Assign SysTemp_s16 to l_ecuTemp_s16;
 * else (no)
 *   if (NtcTemp_s16 >= PAR_ECU_TEMP_UP_THR) then (yes)
 *     :Assign NtcTemp_s16 to l_ecuTemp_s16;
 *   else (no)
 *     :Evaluate l_ecuTemp_s16 as \n(NtcTemp_s16 * (NtcTemp_s16 - PAR_ECU_TEMP_LOW_THR)\n + SysTemp_s16 * (PAR_ECU_TEMP_UP_THR - NtcTemp_s16))\n / (PAR_ECU_TEMP_UP_THR - PAR_ECU_TEMP_LOW_THR);
 *     note 
 *       This interpolation blends the two temperature
 *       measurements smoothly in the middle range,
 *       ensuring a gradual transition between
 *       SysTemp_s16 (low range) and NtcTemp_s16 (high range).
 *     end note
 *   endif
 * endif
 *
 * :return l_ecuTemp_s16;
 * stop
 * @enduml
 *
 * @return ECU temperature estimate in degrees Celsius [°C].
 */
sint16 Calc_ecuTemperature_s16(void);

#endif /* CALC_ECU_TEMP_H */
