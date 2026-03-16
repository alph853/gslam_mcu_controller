#pragma once

#include <cstdint>

#include "config.hpp"

namespace app {

/** @brief Body-frame velocity command used internally by the drive controller. */
struct MotionCommand {
  float v_mps = 0.0f;
  float w_radps = 0.0f;
};

/** @brief Left/right wheel speed targets in meters per second. */
struct WheelTargets {
  float left_mps = 0.0f;
  float right_mps = 0.0f;
};

/** @brief Estimated left/right wheel speeds in meters per second. */
struct WheelSpeeds {
  float left_mps = 0.0f;
  float right_mps = 0.0f;
};

/** @brief Planar robot pose estimate in meters and radians. */
struct Pose2D {
  float x_m = 0.0f;
  float y_m = 0.0f;
  float theta_rad = 0.0f;
};

/** @brief Internal telemetry snapshot before wire-format quantization. */
struct TelemetryData {
  std::uint32_t timestamp_ms = 0;
  WheelSpeeds wheel_speeds{};
  Pose2D pose{};
  float battery_voltage_v = 0.0f;
  std::uint8_t battery_percent = 0;
  MotionCommand commanded{};
  std::uint32_t fault_flags = 0;
};

enum FaultFlags : std::uint32_t {
  kFaultNone = 0,
  kFaultCommandTimeout = 1u << 0,
  kFaultProtocol = 1u << 1,
};

/** @brief Reserved heartbeat wire packet schema. */
struct HeartbeatPacket {
  std::uint8_t sync0;
  std::uint8_t sync1;
  std::uint8_t packet_id;
  std::uint8_t payload_size;
  std::uint8_t sequence;
  std::uint32_t timestamp_ms;
  std::uint16_t crc16;
} __attribute__((packed));

static_assert(sizeof(HeartbeatPacket) == 11, "Unexpected HeartbeatPacket size");

/** @brief Parsed generic frame header fields extracted during command decode. */
struct DecodedFrame {
  std::uint8_t packet_id = 0;
  std::uint8_t sequence = 0;
  std::uint8_t payload_size = 0;
};

enum class RobotState {
  kBoot,
  kIdle,
  kRunning,
  kSafeStop,
};
}  // namespace app
