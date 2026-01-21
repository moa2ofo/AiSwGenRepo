#include "VoltMonitoring_cfg.h"

/* ---- VALORI DI CONFIGURAZIONE (progetto-dipendenti) ---- */

const uint16_t VoltMon_ThresholdUnder_mV = 8000u;
const uint16_t VoltMon_ThresholdOver_mV = 13000u;
const uint16_t VoltMon_Hysteresis_mV = 500u;

const uint16_t VoltMon_ActivationTime_ms = 500u;
const uint16_t VoltMon_DeactivationTime_ms = 500u;

/* Periodo task di monitoraggio (esempio: 10 ms) */
const uint16_t VoltMon_TaskPeriod_ms = 10u;

static uint16_t supplyDcFiler_u16 = 0u;
static uint16_t supplyDcNotFiler_u16 = 0u;
const uint16_t lowerVoltMonCfg_cu16 = 0;
const uint16_t higherVoltMonCfg_cu16 = 20000;
const uint16_t middleLowerVoltMonCfg_cu16 = 5000;
const uint16_t middleHigherVoltMonCfg_cu16 = 10000;

/* Implementazione di esempio: qui metterai la vera lettura ADC / HAL */
uint16_t VoltMon_ReadVoltageProject_mV(void) {
  uint16_t l_voltage_mV = 0u;
  uint16_t l_supplyDcFiler_u16 = supplyDcFiler_u16;
  uint16_t l_supplyDcNotFiler_u16 = supplyDcNotFiler_u16;

  if(l_supplyDcNotFiler_u16 > lowerVoltMonCfg_cu16 && l_supplyDcNotFiler_u16 < middleLowerVoltMonCfg_cu16) {
    l_voltage_mV = l_supplyDcNotFiler_u16;
  } else if(l_supplyDcNotFiler_u16 >= middleLowerVoltMonCfg_cu16 && l_supplyDcNotFiler_u16 <= middleHigherVoltMonCfg_cu16) {
    l_voltage_mV = ((l_supplyDcFiler_u16 + l_supplyDcNotFiler_u16) >> 1);
  } else if(l_supplyDcNotFiler_u16 > middleHigherVoltMonCfg_cu16 && l_supplyDcNotFiler_u16 < higherVoltMonCfg_cu16) {
    l_voltage_mV = ((l_supplyDcFiler_u16 + l_supplyDcNotFiler_u16) >> 1);
  } else {
    /* valore fuori range */
    l_voltage_mV = 0u;
  }

  return l_voltage_mV;
}
