#pragma once

#include <cstdint>

#include "cmd_recv.hpp"
#include "drive_controller.hpp"
#include "telem_transmit.hpp"

namespace app {

class App {
public:
  /** @brief Initialize BSP-backed application services and state. */
  void Init();
  /** @brief Execute one fast control-cycle iteration. */
  void RunFastTick();
  /** @brief Execute one slow background iteration. */
  void RunSlowTick();
  /**
   * @brief Feed one received UART byte into the command path.
   * @param byte Received UART byte.
   */
  void OnUartRxByte(std::uint8_t byte);

private:
  bool initialized_ = false;
  DriveController drive_controller_{};
  CommandReceiver command_receiver_{};
  TelemetryTransmitter telemetry_tx_{};
};

}  // namespace app
