#pragma once

#include <cstdint>

#include "common.hpp"

namespace app {

class PIController {
 public:
  PIController(float kp, float ki);
  float Update(float target, float measured, float dt_seconds);
  void Reset();

 private:
  float kp_;
  float ki_;
  float integral_ = 0.0f;
};

class DriveController {
 public:
  DriveController();

  void Init(std::uint32_t now_ms);
  void SetCommand(const CmdVel &command, std::uint32_t now_ms);
  void FastTick(std::uint32_t now_ms, float dt_seconds);

  bool HasSeenCommand() const;
  bool IsCommandTimedOut(std::uint32_t now_ms) const;
  RobotState state() const;
  const TelemetryData &telemetry() const;

 private:
  WheelTargets BodyToWheelTargets(const CmdVel &cmd) const;
  WheelSpeeds EstimateWheelSpeeds(std::int32_t left_count,
                                  std::int32_t right_count,
                                  float dt_seconds);
  CmdVel LimitCommand(const CmdVel &requested, float dt_seconds);
  void UpdateOdometry(const WheelSpeeds &wheel_speeds, float dt_seconds);
  void UpdateSupervisor(bool command_timed_out);

  PIController left_pi_;
  PIController right_pi_;
  CmdVel limited_command_{};
  CmdVel latest_command_{};
  TelemetryData telemetry_{};
  Pose2D pose_{};
  std::int32_t previous_left_count_ = 0;
  std::int32_t previous_right_count_ = 0;
  std::uint32_t last_command_ms_ = 0;
  bool encoders_initialized_ = false;
  bool has_seen_command_ = false;
  RobotState state_ = RobotState::kBoot;
};

}  // namespace app
