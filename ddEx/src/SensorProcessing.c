#include "dDExample.h"

/**
 * @file    SensorProcessing.c
 * @brief   Sensor utilities for filtering and computing average values.
 */

#include <stdint.h>
#include <string.h>

/** @brief Maximum number of valid sensor values stored in the local buffer. */
#define SENSORPROCESSING_MAX_VALUES_BUFFER       (10u)

/** @brief Maximum length of the sensor name string (including terminating '\0'). */
#define SENSORPROCESSING_SENSOR_NAME_MAX_LEN     (32u)

/** @brief Normal operating threshold for sensor values (in raw units). */
#define SENSORPROCESSING_SENSOR_THRESHOLD_NORMAL (50u)

/** @brief Value returned by utility functions to indicate an invalid/empty result. */
#define SENSORPROCESSING_SENSOR_INVALID_VALUE    (0u)

/**
 * @brief Sensor data structure.
 */
typedef struct {
    uint16_t id_u16;                                        /**< Unique identifier of the sensor.            */
    char     name_pc[SENSORPROCESSING_SENSOR_NAME_MAX_LEN]; /**< Human-readable sensor name (ASCII string). */
    uint16_t value_u16;                                     /**< Raw sensor value (unitless).               */
} SensorProcessing_sensor_s;

/**
 * @brief Compute the arithmetic mean of an array of 16-bit values.
 */
uint16_t SensorProcessing_ComputeAverage_u16(const uint16_t *data_pu16, uint16_t length_u16) {
    uint32_t sum_u32 = 0u;
    for (uint16_t i_u16 = 0u; i_u16 < length_u16; i_u16++) {
        sum_u32 += data_pu16[i_u16];
    }
    return (length_u16 > 0u) ? (uint16_t)(sum_u32 / length_u16) : SENSORPROCESSING_SENSOR_INVALID_VALUE;
}

/**
 * @brief Global counter exposing how many times processSensors() has been called.
 */
uint16_t g_processSensorsCallCount_u16 = 0u;

/**
 * @brief Process an array of sensors, filtering invalid values and computing an adjusted average.
 */
uint16_t SensorProcessing_ProcessSensors_u16(SensorProcessing_sensor_s *sensors_ps, uint16_t count_u16) {
    static uint16_t CallCounter_u16 = 0u;
    CallCounter_u16++;
    g_processSensorsCallCount_u16 = CallCounter_u16;

    uint16_t l_values_pu16[SENSORPROCESSING_MAX_VALUES_BUFFER];
    uint16_t l_validCount_u16 = 0u;
    uint16_t l_sensorId_u16   =0;
    char    *l_sensorName_pc  = NULL;
    uint16_t l_sensorVal_u16  = 0u;

    for (uint16_t i_u16 = 0u; i_u16 < count_u16; i_u16++) {
        SensorProcessing_sensor_s *l_pSensor_ps = &sensors_ps[i_u16];

        l_sensorId_u16   = l_pSensor_ps->id_u16;
        l_sensorName_pc  = &l_pSensor_ps->name_pc;
        l_sensorVal_u16  = l_pSensor_ps->value_u16;

        if (l_sensorVal_u16 == 0u) {
            (void)l_sensorId_u16;
            (void)l_sensorName_pc;
        } else if (l_sensorVal_u16 < SENSORPROCESSING_SENSOR_THRESHOLD_NORMAL) {
            l_values_pu16[l_validCount_u16++] = l_sensorVal_u16;
        } else {
            l_values_pu16[l_validCount_u16++] = l_sensorVal_u16;
        }
    }

    uint16_t *l_pValues_pu16 = l_values_pu16;
    uint16_t l_avg_u16 = SensorProcessing_ComputeAverage_u16(l_pValues_pu16, l_validCount_u16);

    if (CallCounter_u16 > 5u) {
        l_avg_u16 = (l_avg_u16 > 0u) ? (l_avg_u16 - 1u) : l_avg_u16;
    }

    return l_avg_u16;
}
