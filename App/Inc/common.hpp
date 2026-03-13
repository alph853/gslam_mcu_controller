#pragma once

#include <cstdint>

#include "config.hpp"

namespace app {

struct MotionCommand {
  float v_mps = 0.0f;
  float w_radps = 0.0f;
};

struct WheelTargets {
  float left_mps = 0.0f;
  float right_mps = 0.0f;
};

struct WheelSpeeds {
  float left_mps = 0.0f;
  float right_mps = 0.0f;
};

struct Pose2D {
  float x_m = 0.0f;
  float y_m = 0.0f;
  float theta_rad = 0.0f;
};

struct TelemetryData {
  std::uint32_t timestamp_ms = 0;
  WheelSpeeds wheel_speeds{};
  Pose2D pose{};
  MotionCommand commanded{};
  std::uint32_t fault_flags = 0;
};

enum FaultFlags : std::uint32_t {
  kFaultNone = 0,
  kFaultCommandTimeout = 1u << 0,
  kFaultProtocol = 1u << 1,
};

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
