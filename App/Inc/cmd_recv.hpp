#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "common.hpp"

namespace app {

/**
 * @brief Fixed-point motion command wire packet sent by the Jetson.
 *
 * The payload stores `v_mmps` and `w_mradps` to keep the command frame compact
 * while the controller converts back to internal floating-point units.
 */
struct MotionCommandPacket {
  std::uint8_t sync0;
  std::uint8_t sync1;
  std::uint8_t packet_id;
  std::uint8_t payload_size;
  std::uint8_t sequence;
  std::int16_t v_mmps;
  std::int16_t w_mradps;
  std::uint16_t crc16;
} __attribute__((packed));

static_assert(sizeof(MotionCommandPacket) == 11, "Unexpected MotionCommandPacket size");

class CommandReceiver {
public:
  /**
   * @brief Consume one byte and output a motion command when a full valid frame is decoded.
   * @param byte Incoming protocol byte.
   * @param command Decoded motion command output when a full valid frame is available.
   */
  bool OnByte(std::uint8_t byte, MotionCommand &command);

private:
  /**
   * @brief Validate and decode one complete framed command packet.
   * @param frame Pointer to the candidate frame bytes.
   * @param frame_size Number of bytes in `frame`.
   * @param command Decoded command output on success.
   * @retval `true` when the frame is valid and `command` was updated.
   * @retval `false` when framing, size, packet ID, or CRC validation fails.
   */
  bool TryDecodeFrame(const std::uint8_t *frame, std::size_t frame_size, MotionCommand &command);

  std::array<std::uint8_t, config::kMaxFrameSize> frame_{};
  std::size_t index_ = 0;
  std::size_t expected_size_ = 0;
};

}  // namespace app
