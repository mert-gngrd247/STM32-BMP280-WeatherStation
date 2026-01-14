# STM32 Digital Weather Station (BMP280)

## Overview
This project is a bare-metal embedded driver for the **BMP280 Temperature & Pressure Sensor**, built from scratch on the **STM32G031K8** (Nucleo-32) using the HAL library. 

Unlike generic Arduino libraries, this driver manually handles the I2C register communication, bitwise data reconstruction, and implements the specific Bosch compensation formulas to convert raw ADC values into accurate degrees Celsius and Pascals.

## Hardware
* **Microcontroller:** STM32 Nucleo-G031K8
* **Sensor:** BMP280 (I2C Interface)
* **Communication:** UART Serial to PC (115200 baud)

## Pin Configuration
| BMP280 Pin | Nucleo Pin | Function |
| :--- | :--- | :--- |
| **VCC** | 3V3 | Power |
| **GND** | GND | Ground |
| **SCL** | PA9 | I2C Clock |
| **SDA** | PA10 | I2C Data |

## Key Features
* **I2C Protocol:** Implements Master Transmit/Receive with register addressing.
* **Bitwise Operations:** Reconstructs 20-bit sensor data from split 8-bit registers.
* **Calibration Math:** Reads factory trimming parameters (`dig_T1`...`dig_P9`) and applies fixed-point compensation logic.
* **Formatted Output:** Prints human-readable data to Serial Console.

## Sample Output

Sensor OK! ID: 0x58 Sensor Configured. Temp: 24.18 C | Press: 1012 hPa Temp: 24.20 C | Press: 1012 hPa