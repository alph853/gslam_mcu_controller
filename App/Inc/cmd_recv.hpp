#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "common.hpp"

namespace app {

class CommandReceiver {
 public:
  bool OnByte(std::uint8_t byte, CmdVel &command);

 private:
  std::array<std::uint8_t, kMaxFrameSize> frame_{};
  std::size_t index_ = 0;
  std::size_t expected_size_ = 0;
};

}  // namespace app
