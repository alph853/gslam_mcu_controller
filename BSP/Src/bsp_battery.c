#include "bsp_battery.h"

#include "i2c.h"

enum
{
  INA219_I2C_ADDRESS = (0x40u << 1),
  INA219_REG_CONFIG = 0x00u,
  INA219_REG_BUS_VOLTAGE = 0x02u,
  INA219_CONFIG_32V_320MV_CONTINUOUS = 0x399Fu,
  INA219_TRANSFER_TIMEOUT_MS = 10u,
};

/* Default percentage mapping assumes a 3S lithium pack on the INA219 bus rail. */
static const float kBatteryEmptyVoltageV = 9.0f;
static const float kBatteryFullVoltageV = 12.6f;

static bool ina219_write_u16(uint8_t reg, uint16_t value)
{
  uint8_t buffer[2];
  buffer[0] = (uint8_t)(value >> 8);
  buffer[1] = (uint8_t)(value & 0xFFu);
  return HAL_I2C_Mem_Write(&hi2c1,
                           INA219_I2C_ADDRESS,
                           reg,
                           I2C_MEMADD_SIZE_8BIT,
                           buffer,
                           sizeof(buffer),
                           INA219_TRANSFER_TIMEOUT_MS) == HAL_OK;
}

static bool ina219_read_u16(uint8_t reg, uint16_t *value)
{
  uint8_t buffer[2];
  if (HAL_I2C_Mem_Read(&hi2c1,
                       INA219_I2C_ADDRESS,
                       reg,
                       I2C_MEMADD_SIZE_8BIT,
                       buffer,
                       sizeof(buffer),
                       INA219_TRANSFER_TIMEOUT_MS) != HAL_OK)
  {
    return false;
  }

  *value = ((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1];
  return true;
}

static uint8_t voltage_to_percentage(float voltage_v)
{
  if (voltage_v <= kBatteryEmptyVoltageV)
  {
    return 0u;
  }

  if (voltage_v >= kBatteryFullVoltageV)
  {
    return 100u;
  }

  const float normalized = (voltage_v - kBatteryEmptyVoltageV) / (kBatteryFullVoltageV - kBatteryEmptyVoltageV);
  return (uint8_t)(normalized * 100.0f + 0.5f);
}

void BspBattery_Init(void)
{
  (void)ina219_write_u16(INA219_REG_CONFIG, INA219_CONFIG_32V_320MV_CONTINUOUS);
}

bool BspBattery_Read(BspBatteryReading *reading)
{
  uint16_t raw_bus_voltage = 0u;

  if (reading == 0)
  {
    return false;
  }

  if (!ina219_read_u16(INA219_REG_BUS_VOLTAGE, &raw_bus_voltage))
  {
    reading->bus_voltage_v = 0.0f;
    reading->percentage = 0u;
    reading->valid = false;
    return false;
  }

  reading->bus_voltage_v = (float)(raw_bus_voltage >> 3) * 0.004f;
  reading->percentage = voltage_to_percentage(reading->bus_voltage_v);
  reading->valid = true;
  return true;
}
