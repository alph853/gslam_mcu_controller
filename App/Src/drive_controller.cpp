#include "drive_controller.hpp"

#include <cmath>

#include "bsp_encoder.h"
#include "bsp_motor.h"
#include "config.hpp"

namespace app {

namespace {

float Clamp(float value, float min_value, float max_value)
{
  return (value < min_value) ? min_value : (value > max_value) ? max_value : value;
}

float Slew(float target, float previous, float max_delta)
{
  return Clamp(target, previous - max_delta, previous + max_delta);
}

}  // namespace

PIController::PIController(float kp, float ki)
    : kp_(kp), ki_(ki)
{
}

float PIController::Update(float target, float measured, float dt_seconds)
{
  const float error = target - measured;
  integral_ += error * dt_seconds;
  return Clamp(kp_ * error + ki_ * integral_, -1.0f, 1.0f);
}

void PIController::Reset()
{
  integral_ = 0.0f;
}

DriveController::DriveController()
    : left_pi_(config::kLeftKp, config::kLeftKi),
      right_pi_(config::kRightKp, config::kRightKi)
{
}

void DriveController::Init(std::uint32_t now_ms)
{
  previous_left_count_ = BspEncoder_GetLeftCount();
  previous_right_count_ = BspEncoder_GetRightCount();
  encoders_initialized_ = true;
  telemetry_.timestamp_ms = now_ms;
  state_ = RobotState::kIdle;
}

void DriveController::SetCommand(const MotionCommand &command, std::uint32_t now_ms)
{
  latest_command_ = command;
  last_command_ms_ = now_ms;
  has_seen_command_ = true;
}

void DriveController::FastTick(std::uint32_t now_ms, float dt_seconds)
{
  const bool command_timed_out = IsCommandTimedOut(now_ms);
  const MotionCommand shaped = LimitCommand(command_timed_out ? MotionCommand{} : latest_command_,
                                            dt_seconds);
  const WheelTargets targets = BodyToWheelTargets(shaped);
  const WheelSpeeds wheel_speeds =
      EstimateWheelSpeeds(BspEncoder_GetLeftCount(), BspEncoder_GetRightCount(), dt_seconds);

  float left_duty = 0.0f;
  float right_duty = 0.0f;
  telemetry_.fault_flags = kFaultNone;

  if (command_timed_out)
  {
    left_pi_.Reset();
    right_pi_.Reset();
    BspMotor_Enable(false);
    telemetry_.fault_flags |= kFaultCommandTimeout;
  }
  else
  {
    BspMotor_Enable(true);
    left_duty = left_pi_.Update(targets.left_mps, wheel_speeds.left_mps, dt_seconds);
    right_duty = right_pi_.Update(targets.right_mps, wheel_speeds.right_mps, dt_seconds);
  }

  BspMotor_SetLeftDuty(left_duty);
  BspMotor_SetRightDuty(right_duty);
  UpdateOdometry(wheel_speeds, dt_seconds);
  UpdateSupervisor(command_timed_out);

  telemetry_.timestamp_ms = now_ms;
  telemetry_.wheel_speeds = wheel_speeds;
  telemetry_.pose = pose_;
  telemetry_.commanded = shaped;
}

bool DriveController::HasSeenCommand() const
{
  return has_seen_command_;
}

bool DriveController::IsCommandTimedOut(std::uint32_t now_ms) const
{
  if (!has_seen_command_)
  {
    return true;
  }
  return (now_ms - last_command_ms_) > config::kCommandTimeoutMs;
}

RobotState DriveController::state() const
{
  return state_;
}

const TelemetryData &DriveController::telemetry() const
{
  return telemetry_;
}

WheelTargets DriveController::BodyToWheelTargets(const MotionCommand &cmd) const
{
  return WheelTargets{
      cmd.v_mps - 0.5f * config::kTrackWidthM * cmd.w_radps,
      cmd.v_mps + 0.5f * config::kTrackWidthM * cmd.w_radps,
  };
}

WheelSpeeds DriveController::EstimateWheelSpeeds(std::int32_t left_count,
                                                 std::int32_t right_count,
                                                 float dt_seconds)
{
  if (!encoders_initialized_)
  {
    previous_left_count_ = left_count;
    previous_right_count_ = right_count;
    encoders_initialized_ = true;
  }

  const std::int32_t delta_left = left_count - previous_left_count_;
  const std::int32_t delta_right = right_count - previous_right_count_;
  previous_left_count_ = left_count;
  previous_right_count_ = right_count;

  const float meters_per_count =
      2.0f * 3.14159265359f * config::kWheelRadiusM / config::kEncoderCountsPerRev;
  return WheelSpeeds{
      (delta_left * meters_per_count) / dt_seconds,
      (delta_right * meters_per_count) / dt_seconds,
  };
}

MotionCommand DriveController::LimitCommand(const MotionCommand &requested, float dt_seconds)
{
  limited_command_.v_mps = Slew(
      Clamp(requested.v_mps, -config::kMaxLinearSpeedMps, config::kMaxLinearSpeedMps),
      limited_command_.v_mps,
      config::kMaxLinearAccelMps2 * dt_seconds);
  limited_command_.w_radps = Slew(
      Clamp(requested.w_radps, -config::kMaxAngularSpeedRadps, config::kMaxAngularSpeedRadps),
      limited_command_.w_radps,
      config::kMaxAngularAccelRadps2 * dt_seconds);
  return limited_command_;
}

void DriveController::UpdateOdometry(const WheelSpeeds &wheel_speeds, float dt_seconds)
{
  const float linear = 0.5f * (wheel_speeds.left_mps + wheel_speeds.right_mps);
  const float angular = (wheel_speeds.right_mps - wheel_speeds.left_mps) / config::kTrackWidthM;
  const float delta_theta = angular * dt_seconds;
  const float heading = pose_.theta_rad + 0.5f * delta_theta;

  pose_.x_m += linear * std::cos(heading) * dt_seconds;
  pose_.y_m += linear * std::sin(heading) * dt_seconds;
  pose_.theta_rad += delta_theta;
}

void DriveController::UpdateSupervisor(bool command_timed_out)
{
  if (!has_seen_command_)
  {
    state_ = RobotState::kIdle;
    return;
  }

  state_ = command_timed_out ? RobotState::kSafeStop : RobotState::kRunning;
}

}  // namespace app
