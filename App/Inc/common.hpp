#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace app {

struct CmdVel {
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
  CmdVel commanded{};
  std::uint32_t fault_flags = 0;
};

enum FaultFlags : std::uint32_t {
  kFaultNone = 0,
  kFaultCommandTimeout = 1u << 0,
  kFaultProtocol = 1u << 1,
};

enum class PacketType : std::uint8_t {
  kCmdVel = 1,
  kTelemetry = 2,
  kHeartbeat = 3,
};

struct CmdVelPacket {
  float v_mps;
  float w_radps;
};

struct TelemetryPacket {
  std::uint32_t timestamp_ms;
  float left_mps;
  float right_mps;
  float x_m;
  float y_m;
  float theta_rad;
};

constexpr std::uint8_t kSync0 = 0xAA;
constexpr std::uint8_t kSync1 = 0x55;
constexpr std::size_t kMaxPayloadSize = 48;
constexpr std::size_t kMaxFrameSize = 2 + 1 + 1 + 1 + kMaxPayloadSize + 2;

struct DecodedPacket {
  PacketType type = PacketType::kTelemetry;
  std::uint8_t sequence = 0;
  std::uint8_t length = 0;
  std::array<std::uint8_t, kMaxPayloadSize> payload{};
};

enum class RobotState {
  kBoot,
  kIdle,
  kRunning,
  kSafeStop,
};

}  // namespace app
