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

bool EncodeFrame(PacketType type,
                 std::uint8_t sequence,
                 const void *payload,
                 std::uint8_t payload_size,
                 std::array<std::uint8_t, kMaxFrameSize> &frame,
                 std::size_t &frame_size)
{
  if (payload_size > kMaxPayloadSize)
  {
    return false;
  }

  frame[0] = kSync0;
  frame[1] = kSync1;
  frame[2] = static_cast<std::uint8_t>(type);
  frame[3] = payload_size;
  frame[4] = sequence;
  if ((payload != nullptr) && (payload_size > 0u))
  {
    std::memcpy(frame.data() + 5, payload, payload_size);
  }

  const std::uint16_t crc = Crc16(frame.data(), 5u + payload_size);
  frame[5u + payload_size] = (std::uint8_t)(crc & 0xFFu);
  frame[6u + payload_size] = (std::uint8_t)(crc >> 8);
  frame_size = 7u + payload_size;
  return true;
}

}  // namespace

void TelemetryTransmitter::MaybeSend(std::uint32_t now_ms, const TelemetryData &telemetry)
{
  if ((now_ms - last_tx_ms_) < config::kTelemetryPeriodMs)
  {
    return;
  }

  TelemetryPacket packet{
      telemetry.timestamp_ms,
      telemetry.wheel_speeds.left_mps,
      telemetry.wheel_speeds.right_mps,
      telemetry.pose.x_m,
      telemetry.pose.y_m,
      telemetry.pose.theta_rad,
  };

  std::array<std::uint8_t, kMaxFrameSize> frame{};
  std::size_t frame_size = 0;
  if (EncodeFrame(PacketType::kTelemetry, sequence_++, &packet, sizeof(packet), frame, frame_size))
  {
    BspUart_Write(frame.data(), (std::uint16_t)frame_size);
    last_tx_ms_ = now_ms;
  }
}

}  // namespace app
