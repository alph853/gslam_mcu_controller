#pragma once

#include <cstdint>

#include "cmd_recv.hpp"
#include "drive_controller.hpp"
#include "telem_transmit.hpp"

namespace app {

class App {
 public:
  void Init();
  void RunFastTick();
  void RunSlowTick();
  void OnUartRxByte(std::uint8_t byte);

 private:
  bool initialized_ = false;
  DriveController drive_controller_{};
  CommandReceiver command_receiver_{};
  TelemetryTransmitter telemetry_tx_{};
};

}  // namespace app
