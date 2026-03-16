#ifndef BSP_BATTERY_H
#define BSP_BATTERY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Last sampled battery state from the INA219 bus-voltage register.
 */
typedef struct
{
  /** Measured bus voltage in volts. */
  float bus_voltage_v;
  /** Voltage-based state-of-charge estimate in percent. */
  uint8_t percentage;
  /** `true` when the reading came from a successful I2C transaction. */
  bool valid;
} BspBatteryReading;

/** @brief Configure the INA219 with the default continuous conversion profile. */
void BspBattery_Init(void);

/**
 * @brief Read the latest battery measurement from the INA219.
 * @param reading Output storage for the sampled reading.
 * @retval `true` if the sample was read successfully.
 * @retval `false` if the sensor transaction failed or `reading` is null.
 */
bool BspBattery_Read(BspBatteryReading *reading);

#ifdef __cplusplus
}
#endif

#endif
