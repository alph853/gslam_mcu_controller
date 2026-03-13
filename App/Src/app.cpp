#include "app.hpp"

#include "bsp_encoder.h"
#include "bsp_motor.h"
#include "bsp_time.h"
#include "bsp_uart.h"

namespace app {

void App::Init()
{
  BspMotor_Init();
  BspEncoder_Init();
  BspTime_Init();
  BspUart_Init();
  drive_controller_.Init(BspTime_NowMs());
  BspTime_StartControlTick();
  initialized_ = true;
}

void App::RunFastTick()
{
  if (!initialized_)
    return;

  drive_controller_.FastTick(BspTime_NowMs(), BspTime_ControlDtSeconds());
}

void App::RunSlowTick()
{
  if (!initialized_)
    return;

  telemetry_tx_.MaybeSend(BspTime_NowMs(), drive_controller_.telemetry());
}

void App::OnUartRxByte(std::uint8_t byte)
{
  MotionCommand command{};
  if (command_receiver_.OnByte(byte, command))
  {
    drive_controller_.SetCommand(command, BspTime_NowMs());
  }
}

}  // namespace app
