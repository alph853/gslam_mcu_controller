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

- The Jetson sends body commands over `USART1` as a fixed-size binary frame.
- `USART1` RX uses circular DMA into a software ring buffer.
- `USART1` TX uses normal DMA per frame.
- `TIM6` drives the fast control tick.
- The main loop handles slow work such as telemetry.
- `Core/` and `Drivers/` stay generated/vendor-owned.
- `BSP/` is the C boundary for HAL-facing board access.
- `App/` stays compact while the firmware is still early-stage.

## UART Protocol

`USART1` runs at `115200 8N1`.

All frames share the same wire envelope:

| Field | Size | Notes |
| --- | --- | --- |
| `sync0` | 1 byte | `0xAA` |
| `sync1` | 1 byte | `0x55` |
| `packet_id` | 1 byte | Message type |
| `payload_size` | 1 byte | Payload bytes only |
| `sequence` | 1 byte | Sender-owned rolling counter |
| `payload` | `payload_size` bytes | Packet-specific body |
| `crc16` | 2 bytes | CRC-16/CCITT over header plus payload, little-endian on the wire |

The receiver resynchronizes by scanning for `0xAA 0x55`, then validates `payload_size` and `crc16`.

### Upstream To Controller

`packet_id = 1` is the motion command frame sent by the Jetson:

| Field | Type | Size | Notes |
| --- | --- | --- | --- |
| `sync0` | `uint8_t` | 1 | `0xAA` |
| `sync1` | `uint8_t` | 1 | `0x55` |
| `packet_id` | `uint8_t` | 1 | `1` |
| `payload_size` | `uint8_t` | 1 | `4` |
| `sequence` | `uint8_t` | 1 | Host sequence |
| `v_mmps` | `int16_t` | 2 | Linear velocity in mm/s |
| `w_mradps` | `int16_t` | 2 | Angular velocity in mrad/s |
| `crc16` | `uint16_t` | 2 | CRC over bytes `0..8` |

Total frame size: `11` bytes.

The controller drains bytes from a DMA-backed ring buffer in the main loop, reconstructs framed packets, and converts the fixed-point command back to floating-point control units internally. If no fresh command arrives within `250 ms`, the drive controller enters command-timeout safe stop.

### Controller To Upstream

`packet_id = 2` is the telemetry frame emitted by the controller:

| Field | Type | Size | Notes |
| --- | --- | --- | --- |
| `sync0` | `uint8_t` | 1 | `0xAA` |
| `sync1` | `uint8_t` | 1 | `0x55` |
| `packet_id` | `uint8_t` | 1 | `2` |
| `payload_size` | `uint8_t` | 1 | `24` |
| `sequence` | `uint8_t` | 1 | MCU sequence |
| `timestamp_ms` | `uint32_t` | 4 | Local controller time |
| `left_mmps` | `int16_t` | 2 | Estimated left wheel speed in mm/s |
| `right_mmps` | `int16_t` | 2 | Estimated right wheel speed in mm/s |
| `x_mm` | `int32_t` | 4 | Odometry X in mm |
| `y_mm` | `int32_t` | 4 | Odometry Y in mm |
| `theta_mrad` | `int32_t` | 4 | Odometry heading in mrad |
| `battery_mv` | `uint16_t` | 2 | INA219 bus voltage in mV |
| `battery_percent` | `uint8_t` | 1 | Voltage-based battery estimate |
| `fault_flags` | `uint8_t` | 1 | Low 8 bits of the controller fault mask |
| `crc16` | `uint16_t` | 2 | CRC over bytes `0..28` |

Total frame size: `31` bytes.

Telemetry is sent from the slow loop every `50 ms`, so the nominal telemetry rate is `20 Hz`.

## Design Rules

- Only `BSP` and generated files should touch HAL handles directly.
- ISRs should stay thin and forward events into the app layer.
- `App/` should stay small until the protocol and control logic stabilize.
- Split files further only when the interfaces stop changing quickly.
