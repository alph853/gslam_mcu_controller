#include "telem_transmit.hpp"

#include <array>
#include <cstring>

#include "bsp_uart.h"
#include "config.hpp"

namespace app {

namespace {

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
  packet.sync0 = config::kSync0;
  packet.sync1 = config::kSync1;
  packet.packet_id = config::kTelemetryPacketId;
  packet.payload_size = (std::uint8_t)(sizeof(TelemetryPacket) - 7u);
  packet.sequence = sequence;
  packet.timestamp_ms = telemetry.timestamp_ms;
  packet.left_mps = telemetry.wheel_speeds.left_mps;
  packet.right_mps = telemetry.wheel_speeds.right_mps;
  packet.x_m = telemetry.pose.x_m;
  packet.y_m = telemetry.pose.y_m;
  packet.theta_rad = telemetry.pose.theta_rad;
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
