#pragma once

#include <cstdint>

#include "common.hpp"

namespace app {

struct TelemetryPacket {
  std::uint8_t sync0;
  std::uint8_t sync1;
  std::uint8_t packet_id;
  std::uint8_t payload_size;
  std::uint8_t sequence;
  std::uint32_t timestamp_ms;
  float left_mps;
  float right_mps;
  float x_m;
  float y_m;
  float theta_rad;
  std::uint16_t crc16;
} __attribute__((packed));

static_assert(sizeof(TelemetryPacket) == 31, "Unexpected TelemetryPacket size");

class TelemetryTransmitter {
public:
  /**
   * @brief Send telemetry when the configured transmit period has elapsed.
   * @param now_ms Current system time in milliseconds.
   * @param telemetry Latest telemetry snapshot to encode and transmit.
   */
  void MaybeSend(std::uint32_t now_ms, const TelemetryData &telemetry);

private:
  std::uint32_t last_tx_ms_ = 0;
  std::uint8_t sequence_ = 0;
};

}  // namespace app
