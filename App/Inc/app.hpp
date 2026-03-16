#pragma once

#include <cstdint>

#include "cmd_recv.hpp"
#include "drive_controller.hpp"
#include "telem_transmit.hpp"

namespace app {

class App {
public:
  /**
   * @brief Initialize BSP-backed application services and runtime state.
   */
  void Init();

  /**
   * @brief Execute one fast control-cycle iteration.
   */
  void RunFastTick();

  /**
   * @brief Execute one slow background iteration.
   *
   * This drains pending UART RX bytes, updates slow-rate sensor readings, and
   * sends telemetry when its period elapses.
   */
  void RunSlowTick();

private:
  bool initialized_ = false;
  DriveController drive_controller_{};
  CommandReceiver command_receiver_{};
  TelemetryTransmitter telemetry_tx_{};
};

}  // namespace app
