#ifndef BSP_BATTERY_H
#define BSP_BATTERY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  float bus_voltage_v;
  uint8_t percentage;
  bool valid;
} BspBatteryReading;

void BspBattery_Init(void);
bool BspBattery_Read(BspBatteryReading *reading);

#ifdef __cplusplus
}
#endif

#endif
