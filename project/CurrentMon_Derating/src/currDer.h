#ifndef CURRENTMON_CURRDER_H
#define CURRENTMON_CURRDER_H

#include <stdint.h>

/**
 * @file CurrDer.h
 * @brief Current monitoring derating API and support types.
 *
 * This module defines the types, macros, calibration tables and APIs used
 * to compute current derating limits for DC-link and line currents based
 * on supply voltage and ECU temperature. The main entry point is
 * @ref CurrentMon_Derating, which reads the measurement interfaces,
 * evaluates the derating curves and exposes the resulting limits via
 * global output variables.
 */

/** AUTOSAR-like types for clarity */
typedef int16_t  sint16;  /**< Signed 16-bit integer.  */
typedef int8_t   sint8;   /**< Signed 8-bit integer.   */
typedef uint8_t  uint8;   /**< Unsigned 8-bit integer. */
typedef uint16_t uint16;  /**< Unsigned 16-bit integer */

/*==============================================================*/
/*   TYPES                                                      */
/*==============================================================*/

/**
 * @brief 2D point for derating curve (x, y).
 *
 * This structure describes a single point of a 1-D derating curve
 * used by @ref CurrentMon_SetDeratingLimits_s16. The field @c x_s16 represents
 * the input variable (e.g. voltage or temperature), while @c y_s16
 * represents the corresponding limit (e.g. current limit).
 *
 * Typical curves:
 * - Vbat vs. DC current limit (@ref CurrentMon_IdcVdcTab)
 * - ECU temperature vs. DC current limit (@ref CurrentMon_IdcTecuTab)
 * - ECU temperature vs. line current limit (@ref CurrentMon_IlineTecuTab)
 */
typedef struct CurrentMon_mathApplPoint_s 
{
  sint16 x_s16;  /**< Abscissa of the point (input variable).  */
  sint16 y_s16;  /**< Ordinate of the point (derated limit).   */
} CurrentMon_mathApplPoint_t;

/*==============================================================*/
/*   CONSTANTS / MACROS                                         */
/*==============================================================*/

/**
 * @brief Number of calibration points for the DC current vs. Vdc derating curve.
 *
 * This constant defines the size of @ref CurrentMon_IdcVdcTab, which
 * is used by @ref CurrentMon_SetDeratingLimits_s16 to compute @c limit_IdcVdc inside
 * @ref CurrentMon_Derating.
 */
#define CURRENTMON_IDC_VDC_N_POINTS       (7u)

/**
 * @brief Number of calibration points for the DC current vs. ECU temperature derating curve.
 *
 * This constant defines the size of @ref CurrentMon_IdcTecuTab, which
 * is used by @ref CurrentMon_SetDeratingLimits_s16 to compute @c limit_Idctemp inside
 * @ref CurrentMon_Derating.
 */
#define CURRENTMON_IDC_TECU_N_POINTS      (5u)

/**
 * @brief Number of calibration points for the line current vs. ECU temperature derating curve.
 *
 * This constant defines the size of @ref CurrentMon_IlineTecuTab, which
 * is used by @ref CurrentMon_SetDeratingLimits_s16 to compute @ref CurrentMon_IlineLim_s16
 * inside @ref CurrentMon_Derating.
 */
#define CURRENTMON_ILINE_TECU_N_POINTS    (6u)

/**
 * @brief Utility macro returning the minimum of two values.
 *
 * This macro is used to combine different derating contributions by
 * selecting the most conservative (lowest) limit. In this module it is
 * primarily used by @ref CurrentMon_Derating to compute the final DC
 * current limit @c CurrentMon_IdcLim_s16 as:
 *
 * @code
 * CurrentMon_IdcLim_s16 = CURRENTMON_MIN(limit_IdcVdc, limit_Idctemp);
 * @endcode
 *
 * @param val1 First value (expression without side effects).
 * @param val2 Second value (expression without side effects).
 *
 * @return The smaller of @p val1 and @p val2.
 *
 * @note The arguments may be evaluated more than once; they must not
 *       have side effects.
 */
#define CURRENTMON_MIN(val1, val2)    ( ((val1) > (val2)) ? (val2) : (val1) )

#define STUB_VALUE 5u
#define CURRENTMON_AMPS2SW_RES STUB_VALUE
/*==============================================================*/
/*   INPUT INTERFACE (HW / MEASUREMENT)                         */
/*==============================================================*/

/**
 * @brief Read the measured battery supply voltage.
 *
 * This function returns the current battery/supply voltage in millivolts,
 * as provided by the underlying hardware or ADC measurement layer.
 *
 * In the formal model (Frama-C) it is treated as a pure function:
 * it does not modify any global state and simply returns the current
 * value of the ghost variable @c CurrMon_Vbat_mV.
 *
 * @return Battery voltage in millivolts [mV].
 *
 * @note The returned value is used by @ref CurrentMon_Derating as input
 *       for the voltage-based DC current derating curve
 *       (@ref CurrentMon_IdcVdc_tab), which contributes to @c limit_IdcVdc.
 */
uint16 VoltMeas_get_vbat_u16(void)
{
  return STUB_VALUE;
}

/**
 * @brief Read the filtered ECU temperature.
 *
 * This function returns the ECU temperature in degrees Celsius, already
 * filtered and scaled as required by the derating logic.
 *
 * In the formal model (Frama-C) it is treated as a pure function:
 * it does not modify any global state and simply returns the current
 * value of the ghost variable @c CurrMon_Tecu_degC.
 *
 * @return ECU temperature in degrees Celsius [°C].
 *
 * @note The returned value is used by @ref CurrentMon_Derating as input
 *       for both temperature-based derating curves:
 *       - @ref CurrentMon_IdcTecu_tab (DC current limit vs. TECU)
 *       - @ref CurrentMon_IlineTecu_tab (line current limit vs. TECU)
 */
sint16 TempMeas_get_filtEcuTemp_s16(void)
{
  return STUB_VALUE;
}

/*==============================================================*/
/*   DERATING CORE FUNCTION                                     */
/*==============================================================*/

/**
 * @brief Evaluate a 1-D derating curve and return the corresponding limit.
 *
 * This helper function interpolates a derating characteristic described
 * by an array of @ref CurrentMon_mathApplPoint_t and returns the associated limit
 * for the given input value @p x_var.
 *
 * Typical usage (inside @ref CurrentMon_Derating):
 * - Voltage-based DC current derating using @ref CurrentMon_IdcVdcTab
 *   to compute @c limit_IdcVdc.
 * - Temperature-based DC current derating using @ref CurrentMon_IdcTecuTab
 *   to compute @c limit_Idctemp.
 * - Temperature-based line current derating using @ref CurrentMon_IlineTecuTab
 *   to compute @c CurrentMon_IlineLim_s16.
 *
 * From the formal specification (ACSL) this function is modeled as pure:
 * it does not modify any global state and always returns a non-negative
 * result within an implementation-defined upper bound.
 *
 * @param[in] x_var     Input variable for which the derating limit is required
 *                      (e.g. supply voltage in mV or ECU temperature in °C).
 * @param[in] tab       Pointer to the derating curve (array of points).
 * @param[in] n_points  Number of valid points in @p tab.
 *
 * @return Derated limit associated to @p x_var (non-negative).
 *
 * @pre @p tab must not be @c NULL.
 * @pre @p n_points must be greater than zero.
 */
sint16 CurrentMon_SetDeratingLimits_s16(const sint16 x_var,
                                         const CurrentMon_mathApplPoint_t* tab, const uint8 n_points)
{
  (void)x_var;
  (void)tab;
  (void)n_points;
  return STUB_VALUE;
}

/*==============================================================*/
/*   API PRINCIPALE                                             */
/*==============================================================*/
/**
 * @brief  Evaluate current derating limits based on supply voltage and ECU temperature.
 *
 * This function computes the derated current limits for the DC link and the line
 * current as a function of the measured battery voltage and ECU temperature.
 *
 *
 * @par Interface summary
 *
 * | Interface                          | In | Out | Data type                  | Param                                                          | Data factor | Data offset | Data size                      | Data range             | Data unit |
 * |------------------------------------|:--:|:---:|----------------------------|----------------------------------------------------------------|------------:|------------:|-------------------------------:|------------------------|-----------|
 * | CurrentMon_SetDeratingLimits_s16() | ✓  |     | sint16                     | (const sint16, const CurrentMon_mathApplPoint_t*, const uint8) |           1 |           0 |                            1   | [INT16_MIN, INT16_MAX] | [-]       |
 * | VoltMeas_get_vbat_u16()            | ✓  |     | uint16                     | (void)                                                         |           1 |           0 |                            1   | [0, UINT16_MAX]        | [mV]      |
 * | TempMeas_get_filtEcuTemp_s16()     | ✓  |     | sint16                     | (void)                                                         |           1 |           0 |                            1   | [INT16_MIN, INT16_MAX] | [°C]      |
 * | CURRENT_MON_IDC_VDC_N_POINTS       | ✓  |     | macro                      | –                                                              |           1 |           0 |                            1   |            [7]         | [-]       |
 * | CURRENT_MON_IDC_TECU_N_POINTS      | ✓  |     | macro                      | –                                                              |           1 |           0 |                            1   |            [5]         | [-]       |
 * | CURRENT_MON_ILINE_TECU_N_POINTS    | ✓  |     | macro                      | –                                                              |           1 |           0 |                            1   |            [6]         | [-]       |
 * | CurrentMon_IdcVdcTab               | ✓  |     | CurrentMon_mathApplPoint_t | –                                                              |           1 |           0 | CURRENTMON_IDC_VDC_N_POINTS    | [-]                    | [-]       |
 * | CurrentMon_IdcTecuTab              | ✓  |     | CurrentMon_mathApplPoint_t | –                                                              |           1 |           0 | CURRENTMON_IDC_TECU_N_POINTS   | [-]                    | [-]       |
 * | CurrentMon_IlineTecuTab            | ✓  |     | CurrentMon_mathApplPoint_t | –                                                              |           1 |           0 | CURRENTMON_ILINE_TECU_N_POINTS | [-]                    | [-]       |
 * | CurrentMon_IlineLim_s16            |    | ✓   | sint16                     | –                                                              |           1 |           0 |                            1   | [INT16_MIN, INT16_MAX] | [-]       |
 * | CurrentMon_IdcLim_s16              |    | ✓   | sint16                     | –                                                              |           1 |           0 |                            1   | [INT16_MIN, INT16_MAX] | [-]       |
 *
 * @par Activity diagram
 * The following activity diagram summarizes the execution flow of CurrentMon_Derating():
 *
 * @par Activity diagram (PlantUML)
 *
 * @startuml
 * start
 *
 * :Init l_vbat_u16 equal to the value returned by VoltMeas_get_vbat_u16()
 * Init l_tecu_s16 equal to the value returned by TempMeas_get_filtEcuTemp_s16();
 *
 * :Assign IdcVdc the value returned by CurrentMon_SetDeratingLimits_s16(l_vbat_u16,  CurrentMon_IdcVdcTab,CURRENTMON_IDC_VDC_N_POINTS)
 * Assign IdcTemp the value returned by CurrentMon_SetDeratingLimits_s16(l_tecu_s16,  CurrentMon_IdcTecuTab,CURRENTMON_IDC_TECU_N_POINTS)
 * Assign IlineTemp the value returned by CurrentMon_SetDeratingLimits_s16(l_tecu_s16,  CurrentMon_IlineTecuTab,CURRENTMON_ILINE_TECU_N_POINTS)
 * Assign to IdcLim_s16 the min between l_limitIdcVdc_s16 and l_limitIdctemp_s16;
 *
 * stop
 * @enduml
 *
 * @return None.
 */
void CurrentMon_Derating(void);

#endif