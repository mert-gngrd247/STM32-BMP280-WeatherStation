# STM32 Digital Weather Station (BMP280 + Python Dashboard)

## Overview
This project is a bare-metal embedded driver for the **BMP280 Temperature & Pressure Sensor**, built from scratch on the **STM32G031K8** (Nucleo-32) using the HAL library.

Unlike generic Arduino libraries, this driver manually handles the I2C register communication, bitwise data reconstruction, and implements the specific Bosch compensation formulas to convert raw ADC values into accurate degrees Celsius and Pascals.

It now features a **Python Dashboard** for real-time Hardware-in-the-Loop (HIL) visualization.

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
* **Calibration Math:** Reads factory trimming parameters (`dig_T1`...`dig_P9`) and applies fixed-point compensation logic.
* **CSV Telemetry:** Formats sensor data into a machine-readable stream (`Temp,Pressure`) for external processing.
* **Live Dashboard:** Includes a Python script to plot data in real-time.

## ⚙️ Technical Implementation

### 1. Data Processing
To enable easy parsing by the PC, data is transmitted over UART (115200 baud) in a strict CSV format.
* **Format:** `Temperature,Pressure`
* **Example Raw Output:**
  ```text
  24,1012
  24,1013
  25,1012
  
### 2. Python Visualization
The project includes a `scripts/` directory containing a visualization tool.
* **Script:** `weather_dashboard.py`
* **Dependencies:** `pyserial`, `matplotlib`
* **Function:** Connects to the UART stream, parses the CSV, and updates dual live graphs (Temperature & Pressure) at approximately 2Hz.

## 🚀 How to Run

### Step 1: Firmware
1. Open the project in **STM32CubeIDE**.
2. Build and Flash the **Nucleo-G031K8**.

### Step 2: Python Dashboard
1. Navigate to the `scripts/` folder.
2. Install requirements: `pip install pyserial matplotlib`
3. Connect the board via USB.
4. Run the dashboard:
   ```bash
   python weather_dashboard.py
   
## 📂 File Structure
* `Core/Src/main.c`: Main loop, I2C driver, and CSV transmission logic.
* `scripts/weather_dashboard.py`: Python real-time plotting script.
* `WeatherStation.ioc`: STM32CubeMX Configuration.
