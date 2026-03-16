#include "telem_transmit.hpp"

#include <limits>

#include "bsp_uart.h"
#include "config.hpp"

namespace app {

namespace {

constexpr float kMillimetersPerMeter = 1000.0f;
constexpr float kMilliradiansPerRadian = 1000.0f;

template <typename Integer>
Integer SaturateToInt(float value, float scale)
{
  const float scaled = value * scale;
  const float min_value = (float)std::numeric_limits<Integer>::min();
  const float max_value = (float)std::numeric_limits<Integer>::max();
  const float clamped = scaled < min_value ? min_value : (scaled > max_value ? max_value : scaled);
  return (Integer)(clamped >= 0.0f ? clamped + 0.5f : clamped - 0.5f);
}

std::uint16_t Crc16(const std::uint8_t *data, std::size_t size)
{
  std::uint16_t crc = 0xFFFFu;
  for (std::size_t i = 0; i < size; ++i)
  {
    crc ^= (std::uint16_t)data[i] << 8;
    for (int bit = 0; bit < 8; ++bit)
    {
      crc = (crc & 0x8000u) != 0u ? (std::uint16_t)((crc << 1) ^ 0x1021u) : (std::uint16_t)(crc << 1);
    }
  }
  return crc;
}

void EncodeFrame(const TelemetryData &telemetry, std::uint8_t sequence, TelemetryPacket &packet)
{
  const std::uint16_t battery_mv =
      telemetry.battery_voltage_v <= 0.0f ? 0u : SaturateToInt<std::uint16_t>(telemetry.battery_voltage_v, kMillimetersPerMeter);

  packet.sync0 = config::kSync0;
  packet.sync1 = config::kSync1;
  packet.packet_id = config::kTelemetryPacketId;
  packet.payload_size = (std::uint8_t)(sizeof(TelemetryPacket) - 7u);
  packet.sequence = sequence;
  packet.timestamp_ms = telemetry.timestamp_ms;
  packet.left_mmps = SaturateToInt<std::int16_t>(telemetry.wheel_speeds.left_mps, kMillimetersPerMeter);
  packet.right_mmps = SaturateToInt<std::int16_t>(telemetry.wheel_speeds.right_mps, kMillimetersPerMeter);
  packet.x_mm = SaturateToInt<std::int32_t>(telemetry.pose.x_m, kMillimetersPerMeter);
  packet.y_mm = SaturateToInt<std::int32_t>(telemetry.pose.y_m, kMillimetersPerMeter);
  packet.theta_mrad = SaturateToInt<std::int32_t>(telemetry.pose.theta_rad, kMilliradiansPerRadian);
  packet.battery_mv = battery_mv;
  packet.battery_percent = telemetry.battery_percent;
  packet.fault_flags = (std::uint8_t)(telemetry.fault_flags & 0xFFu);
  packet.crc16 = Crc16(reinterpret_cast<const std::uint8_t *>(&packet), sizeof(TelemetryPacket) - sizeof(packet.crc16));
}

}  // namespace

void TelemetryTransmitter::MaybeSend(std::uint32_t now_ms, const TelemetryData &telemetry)
{
  if ((now_ms - last_tx_ms_) < config::kTelemetryPeriodMs)
  {
    return;
  }

  TelemetryPacket packet{};
  EncodeFrame(telemetry, sequence_++, packet);
  if (BspUart_Write(reinterpret_cast<const std::uint8_t *>(&packet), sizeof(packet)))
  {
    last_tx_ms_ = now_ms;
  }
}

}  // namespace app
