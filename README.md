# gslam

STM32F407VETx firmware for a differential-drive base controlled by a Jetson over `USART1`.

## Hardware Mapping

| Function | Peripheral | Pins | Notes |
| --- | --- | --- | --- |
| Left encoder | `TIM5` encoder | `PA0`, `PA1` | 32-bit counter |
| Right encoder | `TIM2` encoder | `PA15`, `PB3` | 32-bit counter |
| Left PWM | `TIM1_CH1`, `TIM1_CH2` | `PE9`, `PE11` | Half-bridge outputs |
| Right PWM | `TIM1_CH3`, `TIM1_CH4` | `PE13`, `PE14` | Half-bridge outputs |
| Right enable | GPIO output | `PD0` | `EnableRight` |
| Left enable | GPIO output | `PD1` | `EnableLeft` |
| UART | `USART1` | `PA9` TX, `PA10` RX | Jetson link |

## Runtime Model

- The Jetson sends only body commands: `(v, w)` as a binary `CmdVelPacket`.
- `TIM6` drives the fast control tick.
- The main loop handles slow work such as telemetry.
- `Core/` and `Drivers/` stay generated/vendor-owned.
- `BSP/` is the C boundary for HAL-facing board access.
- `App/` stays compact while the firmware is still early-stage.

## Project Structure

```text
Core/
Drivers/

BSP/
  Inc/
    bsp_encoder.h
    bsp_motor.h
    bsp_time.h
    bsp_uart.h
  Src/
    bsp_encoder.c
    bsp_motor.c
    bsp_time.c
    bsp_uart.c

App/
  Inc/
    app_entry.h
    common.hpp
    config.hpp
    controller.hpp
    cmd_recv.hpp
    telem_transmit.hpp
    app.hpp
  Src/
    app_entry.cpp
    controller.cpp
    cmd_recv.cpp
    telem_transmit.cpp
    app.cpp
```

## File Responsibilities

### `Core/` and `Drivers/`

Generated and vendor-owned code:

- startup
- interrupts
- peripheral init
- HAL sources
- linker integration

Application logic should not accumulate here beyond minimal user-code bridge points.

### `BSP/`

Thin C wrappers around generated peripherals and HAL:

- motor PWM and enable control
- encoder access
- UART RX/TX
- timebase and control tick start

### `App/Inc/common.hpp`

Shared app-side schemas and structs:

- command and telemetry structs
- packet types and framing constants
- runtime state enums
- shared data containers

### `App/Inc/config.hpp`

Robot parameters and tunables:

- geometry
- encoder conversion constants
- command timeout
- telemetry period
- PI gains
- motion limits

These are still placeholders and should be replaced with real hardware values.

### `App/Inc/controller.hpp` and `App/Src/controller.cpp`

Merged control-side implementation.

This intentionally groups together what would otherwise be separate early-stage pieces:

- command limiting
- wheel estimation
- kinematics
- odometry
- supervisor state updates
- motor command generation
- `PIController`

Main class: `DriveController`

### `App/Inc/cmd_recv.hpp` and `App/Src/cmd_recv.cpp`

UART command decode path for incoming `CmdVelPacket` frames.

Responsibilities:

- frame accumulation
- CRC validation
- packet decode
- conversion from wire packet to `CmdVel`

### `App/Inc/telem_transmit.hpp` and `App/Src/telem_transmit.cpp`

Telemetry encode and send path.

Responsibilities:

- build telemetry packet
- frame encode
- periodic transmit scheduling

### `App/Inc/app.hpp` and `App/Src/app.cpp`

Small top-level composition root that wires together:

- controller
- command receiver
- telemetry transmitter
- BSP init

### `App/Inc/app_entry.h` and `App/Src/app_entry.cpp`

C ABI bridge used by `main.c`.

Exports:

- `App_Init()`
- `App_RunFastTick()`
- `App_RunSlowTick()`
- `App_OnUartRxByte()`

## Design Rules

- Only `BSP` and generated files should touch HAL handles directly.
- ISRs should stay thin and forward events into the app layer.
- `App/` should stay small until the protocol and control logic stabilize.
- Split files further only when the interfaces stop changing quickly.
