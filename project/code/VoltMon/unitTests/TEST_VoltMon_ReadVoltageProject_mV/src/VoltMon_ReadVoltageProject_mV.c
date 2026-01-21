#include "VoltMon_ReadVoltageProject_mV.h"

extern uint16_t supplyDcFiler_u16 = 0u;
extern uint16_t supplyDcNotFiler_u16 = 0u;

const uint16_t lowerVoltMonCfg_cu16 = 0;
const uint16_t higherVoltMonCfg_cu16 = 20000;
const uint16_t middleLowerVoltMonCfg_cu16 = 5000;
const uint16_t middleHigherVoltMonCfg_cu16 = 10000;

/* FUNCTION TO TEST */

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
