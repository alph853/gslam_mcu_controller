#pragma once

#include <cstdint>

namespace app::config {

constexpr float kWheelRadiusM = 0.05f;
constexpr float kTrackWidthM = 0.30f;
constexpr float kEncoderCountsPerRev = 2048.0f;
constexpr float kMaxLinearSpeedMps = 1.0f;
constexpr float kMaxAngularSpeedRadps = 4.0f;
constexpr float kMaxLinearAccelMps2 = 1.5f;
constexpr float kMaxAngularAccelRadps2 = 6.0f;
constexpr std::uint32_t kCommandTimeoutMs = 250u;
constexpr float kLeftKp = 0.4f;
constexpr float kLeftKi = 0.2f;
constexpr float kRightKp = 0.4f;
constexpr float kRightKi = 0.2f;
constexpr std::uint32_t kTelemetryPeriodMs = 50u;

constexpr std::uint8_t kSync0 = 0xAA;
constexpr std::uint8_t kSync1 = 0x55;
constexpr std::uint8_t kMotionCommandPacketId = 1;
constexpr std::uint8_t kTelemetryPacketId = 2;
constexpr std::uint8_t kHeartbeatPacketId = 3;
constexpr std::size_t kMaxPayloadSize = 48;
constexpr std::size_t kMaxFrameSize = 2 + 1 + 1 + 1 + kMaxPayloadSize + 2;

}  // namespace app::config
