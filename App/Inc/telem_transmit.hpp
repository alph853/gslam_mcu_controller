#pragma once

#include <cstdint>

#include "common.hpp"

namespace app {

class TelemetryTransmitter {
 public:
  void MaybeSend(std::uint32_t now_ms, const TelemetryData &telemetry);

 private:
  std::uint32_t last_tx_ms_ = 0;
  std::uint8_t sequence_ = 0;
};

}  // namespace app
