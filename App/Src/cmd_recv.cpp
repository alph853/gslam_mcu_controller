#include "cmd_recv.hpp"

#include <cstring>

namespace app {

namespace {

std::uint16_t Crc16(const std::uint8_t *data, std::size_t size)
{
  std::uint16_t crc = 0xFFFFu;
  for (std::size_t i = 0; i < size; ++i)
  {
    crc ^= (std::uint16_t)data[i] << 8;
    for (int bit = 0; bit < 8; ++bit)
    {
      crc = (crc & 0x8000u) != 0u ? (std::uint16_t)((crc << 1) ^ 0x1021u) : (std::uint16_t)(crc << 1);
    }
  }
  return crc;
}

}  // namespace

bool CommandReceiver::TryDecodeFrame(const std::uint8_t *frame,
                                     std::size_t frame_size,
                                     MotionCommand &command)
{
  if ((frame_size < 7u) || (frame[0] != config::kSync0) || (frame[1] != config::kSync1))
  {
    return false;
  }

  DecodedFrame decoded{};
  decoded.packet_id = frame[2];
  decoded.payload_size = frame[3];
  decoded.sequence = frame[4];

  if ((decoded.payload_size > config::kMaxPayloadSize) || (frame_size != (std::size_t)(7u + decoded.payload_size)))
  {
    return false;
  }

  const std::uint16_t expected_crc = Crc16(frame, 5u + decoded.payload_size);
  const std::uint16_t actual_crc = (std::uint16_t)frame[5u + decoded.payload_size] |
                                   ((std::uint16_t)frame[6u + decoded.payload_size] << 8);
  if (expected_crc != actual_crc)
  {
    return false;
  }

  if ((decoded.packet_id != config::kMotionCommandPacketId) ||
      (decoded.payload_size != sizeof(MotionCommand)))
  {
    return false;
  }

  std::memcpy(&command, frame + 5, sizeof(command));
  return true;
}

bool CommandReceiver::OnByte(std::uint8_t byte, MotionCommand &command)
{
  if (index_ == 0u)
  {
    if (byte != config::kSync0)
    {
      return false;
    }
    frame_[index_++] = byte;
    return false;
  }

  if (index_ == 1u)
  {
    if (byte != config::kSync1)
    {
      index_ = 0u;
      return false;
    }
    frame_[index_++] = byte;
    return false;
  }

  frame_[index_++] = byte;
  if (index_ == 4u)
  {
    expected_size_ = 7u + frame_[3];
    if (expected_size_ > config::kMaxFrameSize)
    {
      index_ = 0u;
      expected_size_ = 0u;
    }
    return false;
  }

  if ((expected_size_ != 0u) && (index_ == expected_size_))
  {
    const bool decoded = TryDecodeFrame(frame_.data(), expected_size_, command);
    index_ = 0u;
    expected_size_ = 0u;
    return decoded;
  }

  return false;
}

}  // namespace app
