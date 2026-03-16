#include "app.hpp"

#include "bsp_battery.h"
#include "bsp_encoder.h"
#include "bsp_motor.h"
#include "bsp_time.h"
#include "bsp_uart.h"

namespace app {

namespace {

void ProcessPendingUart(CommandReceiver &command_receiver, DriveController &drive_controller)
{
  BspUart_PollRx();

  std::uint8_t byte = 0u;
  MotionCommand command{};
  while (BspUart_ReadByte(&byte))
  {
    if (command_receiver.OnByte(byte, command))
    {
      drive_controller.SetCommand(command, BspTime_NowMs());
    }
  }
}

}  // namespace

void App::Init()
{
  BspMotor_Init();
  BspEncoder_Init();
  BspTime_Init();
  BspUart_Init();
  BspBattery_Init();
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

  ProcessPendingUart(command_receiver_, drive_controller_);

  TelemetryData telemetry = drive_controller_.telemetry();
  BspBatteryReading battery{};
  if (BspBattery_Read(&battery))
  {
    telemetry.battery_voltage_v = battery.bus_voltage_v;
    telemetry.battery_percent = battery.percentage;
  }

  telemetry_tx_.MaybeSend(BspTime_NowMs(), telemetry);
}

}  // namespace app
