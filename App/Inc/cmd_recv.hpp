#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "common.hpp"

namespace app {

struct MotionCommandPacket {
  std::uint8_t sync0;
  std::uint8_t sync1;
  std::uint8_t packet_id;
  std::uint8_t payload_size;
  std::uint8_t sequence;
  float v_mps;
  float w_radps;
  std::uint16_t crc16;
} __attribute__((packed));

static_assert(sizeof(MotionCommandPacket) == 15, "Unexpected MotionCommandPacket size");

class CommandReceiver {
public:
  /**
   * @brief Consume one byte and output a motion command when a full valid frame is decoded.
   * @param byte Incoming protocol byte.
   * @param command Decoded motion command output when a full valid frame is available.
   */
  bool OnByte(std::uint8_t byte, MotionCommand &command);

private:
  bool TryDecodeFrame(const std::uint8_t *frame, std::size_t frame_size, MotionCommand &command);

  std::array<std::uint8_t, config::kMaxFrameSize> frame_{};
  std::size_t index_ = 0;
  std::size_t expected_size_ = 0;
};

}  // namespace app
