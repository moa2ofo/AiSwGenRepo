/**
 * @file    SensorProcessing.h
 * @brief   Sensor utilities for filtering and computing average values.
 *
 *
 * @par Interface summary
 * The following table summarizes the main interfaces and data items used:
 *
 * | Interface                          | In | Out | Data type                     | Param                                             | Data factor | Data offset | Data size                          | Data range             | Data unit |
 * |------------------------------------|:--:|:---:|--------------------------------|---------------------------------------------------|------------:|------------:|-----------------------------------:|------------------------|-----------|
 * | SensorProcessing_ProcessSensors_u16() | ✓  | ✓   | uint16_t                      | (SensorProcessing_sensor_s *sensors_ps, uint16_t count_u16) |           1 |           0 |                                1 | [0, UINT16_MAX]        | [-]       |
 * | SensorProcessing_ComputeAverage_u16() | ✓  | ✓   | uint16_t                      | (const uint16_t *data_pu16, uint16_t length_u16)  |           1 |           0 |                                1 | [0, UINT16_MAX]        | [-]       |
 * | SensorProcessing_sensor_s.id_u16      | ✓  |     | uint16_t                      | -                                                 |           1 |           0 |                                1 | [0, UINT16_MAX]        | [-]       |
 * | SensorProcessing_sensor_s.name_pc     | ✓  |     | char[]                        | -                                                 |           1 |           0 | SENSORPROCESSING_SENSOR_NAME_MAX_LEN | [ASCII chars]          | [-]       |
 * | SensorProcessing_sensor_s.value_u16   | ✓  |     | uint16_t                      | -                                                 |           1 |           0 |                                1 | [0, UINT16_MAX]        | [unitless]|
 * | g_processSensorsCallCount_u16         |    | ✓   | uint16_t                      | -                                                 |           1 |           0 |                                1 | [0, UINT16_MAX]        | [-]       |
 * | SENSORPROCESSING_MAX_VALUES_BUFFER    | ✓  |     | macro                         | -                                                 |           1 |           0 |                                1 | [10]                   | [-]       |
 * | SENSORPROCESSING_SENSOR_NAME_MAX_LEN  | ✓  |     | macro                         | -                                                 |           1 |           0 |                                1 | [32]                   | [-]       |
 * | SENSORPROCESSING_SENSOR_THRESHOLD_NORMAL | ✓ |     | macro                         | -                                                 |           1 |           0 |                                1 | [50]                   | [-]       |
 * | SENSORPROCESSING_SENSOR_INVALID_VALUE | ✓  |     | macro                         | -                                                 |           1 |           0 |                                1 | [0]                    | [-]       |
 *
 * @par Activity diagram (PlantUML)
 * The following PlantUML diagram describes the high-level flow:
 *
 * @startuml
 * title SensorProcessing_ProcessSensors_u16() Activity Diagram
 * 
 * start
 * 
 * :Init static variable CallCounter_u16
 * Assign value to CallCounter_u16 value CallCounter_u16+1
 * Save CallCounter_u16 into g_processSensorsCallCount_u16
 * Init local buffer l_values_pu16[SENSORPROCESSING_MAX_VALUES_BUFFER] 
 * Init local variable l_validCount_u16
 * Init i_u16 equal to 0
 * Init l_sensorId_u16 equal to 0
 * Init l_sensorName_pc equal to NULL
 * Init l_sensorVal_u16 equal to 0;
 * 
 * while (i_u16 < count_u16) is (true)
 * 
 *  :Assign the address of sensors_ps[i_u16] to pointer l_pSensor_ps
 *  Assign l_pSensor_ps->id_u16 to l_sensorId_u16
 *  Assign l_pSensor_ps->name_pc to l_sensorName_pc
 *  Assign l_pSensor_ps->value_u16 to l_sensorVal_u16;
 * 
 *  if (l_sensorVal_u16 == 0) then (invalid)
 *    :DoNothing;
 *  else if (l_sensorVal_u16 < SENSORPROCESSING_SENSOR_THRESHOLD_NORMAL) then (normal)
 *    :Assign l_sensorVal_u16 + 1 to l_values_pu16[i_u16] \n Assign incremented l_validCount_u16;
 *  else 
 *    :Assign l_sensorVal_u16 to l_values_pu16[i_u16] \n Assign incremented l_validCount_u16;
 *  endif
 * 
 *  :i_u16++;
 * 
 * endwhile (false)
 * :Assign pointer l_pValues_pu16 to l_values_pu16[0] \n Call SensorProcessing_ComputeAverage_u16(l_pValues_pu16, l_validCount_u16) \n Assign the returnd value by SensorProcessing_ComputeAverage_u16 to l_avg_u16;
 * 
 * if (CallCounter_u16 > 5) then (true)
 *  if (l_avg_u16 > 0) then (invalid)
 *    :Assign to l_avg_u16 the value l_avg_u16 - 1;
 *  else
 *    :DoNothing;
 *  endif
 * endif
 * 
 * :Return final l_avg_u16;
 * 
 * stop
 * @enduml
 */
uint16_t SensorProcessing_ProcessSensors_u16(SensorProcessing_sensor_s *sensors_ps, uint16_t count_u16);