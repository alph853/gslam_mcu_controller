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

## Design Rules

- Only `BSP` and generated files should touch HAL handles directly.
- ISRs should stay thin and forward events into the app layer.
- `App/` should stay small until the protocol and control logic stabilize.
- Split files further only when the interfaces stop changing quickly.
