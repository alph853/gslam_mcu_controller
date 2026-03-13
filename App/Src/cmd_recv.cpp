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

bool TryDecodeFrame(const std::uint8_t *frame, std::size_t frame_size, DecodedPacket &packet)
{
  if ((frame_size < 7u) || (frame[0] != kSync0) || (frame[1] != kSync1))
  {
    return false;
  }

  const std::uint8_t payload_size = frame[3];
  if ((payload_size > kMaxPayloadSize) || (frame_size != (std::size_t)(7u + payload_size)))
  {
    return false;
  }

  const std::uint16_t expected_crc = Crc16(frame, 5u + payload_size);
  const std::uint16_t actual_crc = (std::uint16_t)frame[5u + payload_size] |
                                   ((std::uint16_t)frame[6u + payload_size] << 8);
  if (expected_crc != actual_crc)
  {
    return false;
  }

  packet.type = static_cast<PacketType>(frame[2]);
  packet.length = payload_size;
  packet.sequence = frame[4];
  if (payload_size > 0u)
  {
    std::memcpy(packet.payload.data(), frame + 5, payload_size);
  }
  return true;
}

}  // namespace

bool CommandReceiver::OnByte(std::uint8_t byte, CmdVel &command)
{
  if (index_ == 0u)
  {
    if (byte != kSync0)
    {
      return false;
    }
    frame_[index_++] = byte;
    return false;
  }

  if (index_ == 1u)
  {
    if (byte != kSync1)
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
    if (expected_size_ > kMaxFrameSize)
    {
      index_ = 0u;
      expected_size_ = 0u;
    }
    return false;
  }

  if ((expected_size_ != 0u) && (index_ == expected_size_))
  {
    DecodedPacket packet{};
    const bool decoded = TryDecodeFrame(frame_.data(), expected_size_, packet);
    index_ = 0u;
    expected_size_ = 0u;

    if ((!decoded) || (packet.type != PacketType::kCmdVel) || (packet.length != sizeof(CmdVelPacket)))
    {
      return false;
    }

    CmdVelPacket wire{};
    std::memcpy(&wire, packet.payload.data(), sizeof(wire));
    command.v_mps = wire.v_mps;
    command.w_radps = wire.w_radps;
    return true;
  }

  return false;
}

}  // namespace app
