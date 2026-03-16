#pragma once

#include <cstdint>

#include "common.hpp"

namespace app {

/**
 * @brief Fixed-point telemetry wire packet sent upstream over UART.
 *
 * Motion and pose values are quantized into millimeter and milliradian units to
 * reduce payload size while preserving sufficient precision for the current robot.
 */
struct TelemetryPacket {
  std::uint8_t sync0;
  std::uint8_t sync1;
  std::uint8_t packet_id;
  std::uint8_t payload_size;
  std::uint8_t sequence;
  std::uint32_t timestamp_ms;
  std::int16_t left_mmps;
  std::int16_t right_mmps;
  std::int32_t x_mm;
  std::int32_t y_mm;
  std::int32_t theta_mrad;
  std::uint16_t battery_mv;
  std::uint8_t battery_percent;
  std::uint8_t fault_flags;
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
