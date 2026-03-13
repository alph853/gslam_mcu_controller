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

}  // namespace app::config
