# Tilt-Controlled CuteBOT Pro

Wireless controller for the CuteBOT Pro robot using two micro:bit v2 boards. Tilt the controller micro:bit to move the robot. Implemented entirely in **bare-metal C** on the nRF52833 

---

## Project Structure

```
microbit-cutebot-controller/
├── transmitter/
│   ├── main.c          # reads accelerometer, sends radio command
│   ├── acc.c / acc.h   # LSM303AGR driver (bare metal I²C)
│   └── radio.c / radio.h  # BLE LR transmitter
└── receiver/
    ├── main.c          # receives command, drives motors
    ├── motor.c / motor.h  # CuteBOT Pro I²C motor driver
    └── radio_rx.c / radio_rx.h  # BLE LR receiver (IRQ-driven)
```

---

## Hardware

| Component | Role |
|---|---|
| micro:bit v2 (nRF52833) | Transmitter — tilt controller |
| micro:bit v2 (nRF52833) | Receiver — mounted on CuteBOT |
| CuteBOT Pro | Robot  |

---

## Modules

### `acc.c` — Accelerometer Driver

Custom bare-metal driver for the **LSM303AGR** (the accelerometer inside the micro:bit v2).

- Communicates via **I²C** (TWI0 peripheral of the nRF52833)
- SCL → P0.08 · SDA → P0.16 (internal pins, found via schematic)
- Configured for 100Hz ODR, ±2g full scale, 12-bit high-resolution mode
- Returns one of five directions: `DIR_UP`, `DIR_DOWN`, `DIR_LEFT`, `DIR_RIGHT`, `DIR_FLAT`
- Tilt threshold: 300mg (~17°)

Key registers (from [LSM303AGR datasheet](https://www.st.com/resource/en/datasheet/lsm303agr.pdf)):

| Register | Address | Value | Description |
|---|---|---|---|
| CTRL_REG1 | 0x20 | 0x57 | ODR=100Hz, X/Y/Z enabled |
| CTRL_REG4 | 0x23 | 0x88 | BDU=1, ±2g, High-Res |
| OUT_X_L | 0x28 | — | First output byte (auto-increment reads 4) |

### `radio.c` — Transmitter

Sends a single-byte command over **BLE Long Range (125kbps)**.

- Mode: `BLE_LR125Kbit` · Channel: 10 · TX Power: +8dBm
- Packet (PDU): `[0x00, 0x01, cmd]` where cmd = `F/B/L/R/S`
- 3-byte CRC (BLE standard polynomial)

### `radio_rx.c` — Receiver

Receives packets via **interrupt** (`RADIO_IRQHandler`).

- SHORTS: `END→DISABLE→RXEN` keeps radio always listening automatically
- Validates CRC before storing command
- `radio_get_command()` returns the last valid command received

### `motor.c` — Motor Driver

Controls CuteBOT Pro motors via **I²C** (address `0x10`).

- SCL → P0.26 · SDA → P1.00 (external I²C pins)
- 7-byte command packet per motor: `{0x99, 0x01, motor, dir, speed, 0x00, 0x88}`
- `motors(int8_t left, int8_t right)`: positive = forward, negative = backward

Protocol reference: [elecfreaks/pxt-Cutebot-Pro](https://github.com/elecfreaks/pxt-Cutebot-Pro)

---

## Building

This project was developed with **SEGGER Embedded Studio** targeting the nRF52833.

1. Open SEGGER Embedded Studio
2. Open the solution file microbit.emProject
3. Build and flash to the respective micro:bit

---

## Datasheets

- [LSM303AGR — STMicroelectronics](https://www.st.com/resource/en/datasheet/lsm303agr.pdf)
- [nRF52833 Product Specification — Nordic Semiconductor](https://docs.nordicsemi.com/bundle/ps_nrf52833/page/keyfeatures_html5.html)
- [CuteBOT Pro Protocol — Elecfreaks](https://github.com/elecfreaks/pxt-Cutebot-Pro)
