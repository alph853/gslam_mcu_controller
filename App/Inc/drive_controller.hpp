#pragma once

#include <cstdint>

#include "common.hpp"

namespace app {

class PIController {
public:
  /** @brief Construct a PI controller with fixed proportional and integral gains. */
  PIController(float kp, float ki);
  /**
   * @brief Update the controller output from a target, measurement, and timestep.
   * @param target Target value for the controlled quantity.
   * @param measured Measured value for the controlled quantity.
   * @param dt_seconds Control timestep in seconds.
   */
  float Update(float target, float measured, float dt_seconds);
  /** @brief Clear the controller integrator state. */
  void Reset();

private:
  float kp_;
  float ki_;
  float integral_ = 0.0f;
};

class DriveController {
public:
  /** @brief Construct the drive controller with configured wheel PI controllers. */
  DriveController();

  /**
   * @brief Initialize controller state from the current hardware readings.
   * @param now_ms Current system time in milliseconds.
   */
  void Init(std::uint32_t now_ms);
  /**
   * @brief Update the latest motion command and freshness timestamp.
   * @param command Latest motion command.
   * @param now_ms Current system time in milliseconds.
   */
  void SetCommand(const MotionCommand &command, std::uint32_t now_ms);
  /**
   * @brief Run one full drive-control update for the current control tick.
   * @param now_ms Current system time in milliseconds.
   * @param dt_seconds Control timestep in seconds.
   */
  void FastTick(std::uint32_t now_ms, float dt_seconds);

  /** @brief Return whether any valid command has been received yet. */
  bool HasSeenCommand() const;
  /**
   * @brief Return whether the latest command is stale at the given time.
   * @param now_ms Current system time in milliseconds.
   */
  bool IsCommandTimedOut(std::uint32_t now_ms) const;
  /** @brief Return the current high-level drive state. */
  RobotState state() const;
  /** @brief Return the most recent telemetry snapshot. */
  const TelemetryData &telemetry() const;

private:
  WheelTargets BodyToWheelTargets(const MotionCommand &cmd) const;
  WheelSpeeds EstimateWheelSpeeds(std::int32_t left_count,
                                  std::int32_t right_count,
                                  float dt_seconds);
  MotionCommand LimitCommand(const MotionCommand &requested, float dt_seconds);
  void UpdateOdometry(const WheelSpeeds &wheel_speeds, float dt_seconds);
  void UpdateSupervisor(bool command_timed_out);

  PIController left_pi_;
  PIController right_pi_;
  MotionCommand limited_command_{};
  MotionCommand latest_command_{};
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
