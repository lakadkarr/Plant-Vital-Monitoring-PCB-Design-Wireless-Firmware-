# 🌿 Plant Vital Monitoring with Industrial Sensors

<div align="center">

![TU Chemnitz](https://img.shields.io/badge/TU%20Chemnitz-Department%20of%20EE%20%26%20IT-009640?style=for-the-badge)
![Embedded Systems](https://img.shields.io/badge/Project%20Lab-Embedded%20Systems-blue?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen?style=for-the-badge)
![License](https://img.shields.io/badge/License-Academic-orange?style=for-the-badge)

> **Non-invasive, industrial-grade plant health monitoring using capacitive and inductive sensing — with real-time Bluetooth streaming and SD card logging.**

</div>

---

## 📑 Table of Contents

- [Overview](#-overview)
- [System Architecture](#-system-architecture)
- [Hardware Components](#-hardware-components)
  - [LDC1101 — Inductance Sensor](#1-ldc1101--inductance-sensor)
  - [FDC2214 — Capacitance Sensor](#2-fdc2214--capacitance-sensor)
  - [Teensy 4.0 — Microcontroller](#3-teensy-40--microcontroller)
  - [HW689 — RTC Module](#4-hw689--rtc-module)
  - [MicroSD Card Adapter](#5-microsd-card-adapter)
  - [BT05 Bluetooth Module](#6-bt05-bluetooth-module)
- [PCB Design](#-pcb-design)
- [Wiring Diagram](#-wiring-diagram)
- [Software](#-software)
  - [LDC1101 via SPI](#ldc1101-via-spi)
  - [FDC2214 via I²C](#fdc2214-via-i2c)
- [Results](#-results)
- [Challenges & Debugging](#-challenges--debugging)
- [Project Roadmap](#-project-roadmap)
- [Team](#-team)
- [References](#-references)

---

## 🔍 Overview

This project implements a **smart, non-invasive plant monitoring system** using industrial-grade sensors to support precision agriculture and smart farming. The system tracks:

| Parameter | Sensor Used | Method |
|---|---|---|
| Plant stem movement / growth | LDC1101 (inductive) | Non-contact inductance change |
| Soil/leaf surface moisture | FDC2214 (capacitive) | Capacitance via LC resonance |
| Timestamp of each reading | HW689 RTC | I²C real-time clock |

All data is:
- ⏱️ **Timestamped** with a battery-backed RTC
- 💾 **Stored locally** on a microSD card in CSV format
- 📱 **Streamed wirelessly** via Bluetooth to smartphones or PCs

Designed for greenhouses, vertical farms, research labs, and open-field agriculture.

---

## 🏗️ System Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                        Teensy 4.0                            │
│                    (Central Processing)                      │
│                                                              │
│   SPI ◄──── LDC1101 (Inductance)   [Stem movement / growth] │
│   I²C ◄──── FDC2214 (Capacitance)  [Moisture / humidity]    │
│   I²C ◄──── HW689 RTC              [Timestamps]             │
│   SPI ────► MicroSD Adapter        [CSV data logging]       │
│  UART ────► BT05 Bluetooth         [Wireless streaming]     │
└──────────────────────────────────────────────────────────────┘
```

---

## 🔧 Hardware Components

### 1. LDC1101 — Inductance Sensor

> 📁 [`/hardware/LDC1101/Schematic`](./LDC_1101_sch.png/)

> 📁 [`/hardware/LDC1101/PCB Board`](./LDC1101_brd.png/)

The **LDC1101** is a high-resolution inductance-to-digital converter that detects tiny changes in an LC circuit caused by plant stem movement or proximity to metallic objects.

| Feature | Specification |
|---|---|
| Supply Voltage | 1.8 V – 3.3 V |
| Frequency Range | 500 kHz – 10 MHz |
| RP Resolution | 16-bit |
| L Resolution | 16-bit or 24-bit |
| Operating Temp | –40°C to +125°C |
| Package | VSON-10 (3 mm × 3 mm) |
| Communication | SPI (4-wire) |

**Resonant Frequency Formula:**

$$F_{sensor} = \frac{1}{2\pi\sqrt{L \cdot C}}$$

**Applications in this project:** Stem expansion detection, plant growth direction sensing, non-contact movement tracking.

---

### 2. FDC2214 — Capacitance Sensor

> 📁 [`/hardware/FDC2214/`](./FDC2214_sch.png/)

> 📁 [`/hardware/FDC2214/`](./FDC2214_brd.png/)

The **FDC2214** converts capacitance into digital data using LC resonance. It detects moisture changes in soil, leaves, or air — without direct contact.

| Feature | Specification |
|---|---|
| Supply Voltage | 2.7 V – 3.6 V |
| Resolution | Up to 28-bit |
| Active Current | 2.1 mA |
| Operating Temp | –40°C to +125°C |
| Package | WQFN-16 (4 mm × 4 mm) |
| Communication | I²C (up to 400 kbps) |
| Max Input Cap | 250 nF |

**Key advantage:** Resonant sensing architecture provides strong EMI immunity, even under fluorescent lighting.

**Applications in this project:** Soil moisture measurement, leaf surface moisture, ambient humidity tracking.

---

### 3. Teensy 4.0 — Microcontroller

> 📁 [`/LDC1101_SPI/`](./Firmware_LDC1101_Teensy4.0_SPI.ino/)

> 📁 [`/FDC2214_I2C/`](./Firmware_FDC2214_Teensy4.0_I2C.ino/)

Central processing unit of the system. Manages all sensor communication, data logging, timestamping, and wireless transmission.

**Responsibilities:**
- Reads LDC1101 via **SPI** (pins 10–13)
- Reads FDC2214 via **I²C** (pins 16–17)
- Reads HW689 RTC via **I²C** (pins 18–19)
- Logs to SD card via **SPI** (pin 9 CS)
- Streams data via **UART → BT05** (pins 0–1)

---

### 4. HW689 — RTC Module

> 📁 [`/hardware/components/`](./RTC_HW_689.jpeg/)

Battery-backed real-time clock that timestamps every sensor reading. Communicates via I²C and continues tracking time even when the main system is powered down — ideal for field deployments.

---

### 5. MicroSD Card Adapter

> 📁 [`/hardware/components/`](./SD_Card_Adapter.jpeg/)

Positioned at the PCB edge for easy access. Stores timestamped sensor data in **CSV format** via SPI at 10 MHz.

**Data format example:**
```
LHRData: 1892504   Time: 15:0:2   Date: 30/6/2025
LHRData: 1893464   Time: 15:0:3   Date: 30/6/2025
```

---

### 6. BT05 Bluetooth Module

> 📁 [`/hardware/components/`](./BT05_Bluetooth_Module.jpeg/)

Enables wireless data streaming to Android smartphones or PCs via UART. Used with the **BlueLight** Bluetooth terminal app for real-time monitoring without a wired connection.

---

## 🖥️ PCB Design

All PCBs were designed in **Autodesk EAGLE** with goals of minimizing noise, maximizing component density, and ensuring signal integrity.

| Board | Files | Description |
|---|---|---|
| LDC1101 PCB | [`/hardware/Sch/LDC1101/`](./LDC_1101_sch.png/) + [`/hardware/brd/LDC1101/`](./LDC1101_brd.png/) | Schematic + Eagle layout |
| FDC2214 PCB | [`/hardware/Sch/FDC2214/`](./FDC2214_sch.png/) + [`/hardware/brd/FDC2214/`](./FDC2214_brd.png/) | Schematic + Eagle layout |

> 📸 See [`/LDC Chip/`](./LDC_PCB_Designed.jpeg/)  [`/FDC Chip/`](./FDC_PCB_Designed.jpeg/)

for schematic screenshots, PCB renders, and assembled board photos.

**Design decisions:**
- LDC1101 placed **centrally** for stable signal routing
- FDC2214 placed **centrally** with flexible electrode connections to soil/leaf probes
- MicroSD adapter placed at **PCB edge** for easy card access
- BT05 placed near **communication ports**

---

## 🔌 Wiring Diagram

> 📁 [`/hardware/wiring/`](./Hardware_Wiring.jpeg/)

> 📁 [`/hardware/wiring/`](./Harware_Wiring_Sketch.jpeg/)

| Module | Pin | Teensy 4.0 |
|---|---|---|
| **RTC HW689** | SCL | SCL0 (Pin 19) |
| | SDA | SDA0 (Pin 18) |
| | VCC | 3.3V |
| | GND | GND |
| **MicroSD Adapter** | MISO | Pin 12 |
| | MOSI | Pin 11 |
| | SCK | Pin 13 |
| | CS | Pin 9 |
| | VCC | 5V |
| **LDC1101 PCB** | SDO/INTB | MISO (Pin 12) |
| | SCLK | SCK (Pin 13) |
| | SDI | MOSI (Pin 11) |
| | CSB | CS (Pin 10) |
| | VDD | 3.3V |
| **BT05 Bluetooth** | TXD | RX1 (Pin 0) |
| | RXD | TX1 (Pin 1) |
| | VCC | 5V |
| **FDC2214 PCB** | SCL | SCL1 (Pin 16) |
| | SDA | SDA1 (Pin 17) |
| | VCC | 3.3V |
| | SD | GND |

---

## 💻 Software

> 📁 [`/firmware/`](./firmware/)

Developed using the **Teensy 4.0 Arduino IDE** in simplified C/C++. Libraries used:

```cpp
#include <SPI.h>       // SPI communication (LDC1101, SD card)
#include <Wire.h>      // I²C communication (FDC2214, RTC)
#include <SdFat.h>     // SD card file I/O
#include "RTClib.h"    // RTC timestamp management
```

---

### LDC1101 via SPI

> 📄 [`/firmware/LDC1101/ldc1101_main.ino`](./firmware/LDC1101/ldc1101_main.ino)

**SPI Configuration:** 1 MHz, Mode 0 (CPOL=0, CPHA=0), MSB first

**Key setup steps:**
1. Wait for POR_READ = 0 (power-on reset complete)
2. Enter **Sleep mode** to configure registers
3. Set RPMIN, LHR_RCOUNT (0xFFFF for max resolution), LHR_OFFSET
4. Enable LHR mode (`ALT_CONFIG.LOPTIMAL = 1`)
5. Exit Sleep mode → begin continuous conversion

**LHR Data Reading:**
```cpp
// Poll status register until data ready
while (readRegister_LDC(0x3B) & 0x01);

// Read 24-bit LHR value (3 bytes)
uint8_t lsb = readRegister_LDC(0x38);
uint8_t mid = readRegister_LDC(0x39);
uint8_t msb = readRegister_LDC(0x3A);
uint32_t lhrValue = ((uint32_t)msb << 16) | ((uint32_t)mid << 8) | lsb;
```

---

### FDC2214 via I²C

> 📄 [`/firmware/FDC2214/fdc2214_main.ino`](./firmware/FDC2214/fdc2214_main.ino)

**I²C Address:** `0x2B` (ADDR pin HIGH) or `0x2A` (ADDR pin LOW)
**Speed:** Up to 400 kbps

**Channel 3 configuration:**
```cpp
writeRegister_FDC(0x0B, 0xFFFF); // RCOUNT_CH3 — max resolution
writeRegister_FDC(0x17, 0x1000); // CLOCK_DIVIDERS — divide by 1
writeRegister_FDC(0x21, 0xE800); // Drive current 1.167 mA
writeRegister_FDC(0x1A, 0x0000); // Exit Sleep Mode
```

---

## 📊 Results

### LDC1101 — Measured Data

Using CLKIN = 16 MHz and C = 820 nF:

| Parameter | Value |
|---|---|
| LHR Output (measured) | 1,801,089 |
| Calculated F_sensor | **1.717 MHz** |
| Derived Inductance L | **10.5 nH** |

**Formula applied:**

$$F_{sensor} = \frac{LHR_{output}}{2^{24}} \times CLKIN = \frac{1801089}{2^{24}} \times 16 \times 10^6 = 1.717\ \text{MHz}$$

**Live data successfully streamed** to both laptop (Serial Monitor) and Android phone (BlueLight app) via Bluetooth.

> 📸 Screenshots: [`/images/results/`](./images/results/)

---

## 🐛 Challenges & Debugging

### FDC2214 — Missing I²C ACK Bit

**Problem:** The FDC2214 was not responding on the I²C bus — no ACK bit observed.

**Diagnosis:** Used a **logic analyzer** to inspect the I²C lines and confirmed a missing ACK from the sensor.

**Root cause:** A **PCB routing fault** — the I²C lines were incorrectly connected on the FDC2214 board (chip placement orientation mismatch between expected and designed footprint).

**Fix:** Redesigned the PCB layout with corrected I²C routing. Communication was restored after the updated board was fabricated.

> 📸 Debug images: [`/images/debugging/`](./images/debugging/)

---

## 🗺️ Project Roadmap

```
✅ Design PCB in EAGLE
✅ Fabricate PCB
✅ Solder Components
✅ PCB Continuity Check (Multimeter)
✅ Connect to Teensy 4.0 (SPI / I²C)
✅ Test SPI/I²C — Read CHIP_ID
✅ Configure LDC1101 / FDC2214 Registers
✅ Read Sensor Data (RP / L / C)
✅ Store & Transmit Data (SD card + Bluetooth)
⬜ FDC2214 Full Integration (PCB redesign in progress)
⬜ Capacitance-to-moisture calibration
```

---

## 👥 Team

**Group 11 — TU Chemnitz, Chair of Measurement and Sensor Technology**
**Supervisor:** Prof. Dr.-Ing. Olfa Kanoun

| Member | Matrikel | Responsibility |
|---|---|---|
| **Rushikesh Lakadkar** | 836920 | Hardware Design (LDC1101) & Documentation |
| **Randhir Bhagat** | 834322 | Hardware Design (FDC2214) & Presentation |
| **Komma Siva Vyshnavi Reddy** | 853746 | Bluetooth, RTC & SD Card Configuration |
| **Sheikh Zeeshan Ahmed** | 852762 | FDC & LDC Teensy Integration & Setup |

📅 **Submission Date:** 2025-07-09

---

## 📚 References

1. Texas Instruments — [LDC1101 Datasheet](https://www.ti.com/lit/ds/symlink/ldc1101.pdf)
2. Texas Instruments — [FDC2214 Product Page](https://www.ti.com/product/FDC2214)
3. LDC1101 Design Files — [Google Drive](https://drive.google.com/drive/folders/1_IrN6HWdk9oRrfM9LKO4tfMBH7DWAbs8?usp=drive_link)
4. FDC2214 Design Files — [Google Drive](https://drive.google.com/drive/folders/1Vq0TcMECMADvFhva9oh7v_DhHj89oiy?usp=drive_link)
5. Setup Photos — [Google Drive](https://drive.google.com/drive/folders/1DPBWYxrA6F4h2PfMvFqMYst3x2fYfpn?usp=drive_link)
6. Component Photos — [Google Drive](https://drive.google.com/drive/folders/1XJp2vHJehz3JjfGdg7PYrKDumUkCGjrt?usp=sharing)

---

## 📁 Suggested Repository Structure

```
plant-vital-monitoring/
│
├── README.md
│
├── firmware/
│   ├── LDC1101/
│   │   └── ldc1101_main.ino
│   └── FDC2214/
│       └── fdc2214_main.ino
│
├── hardware/
│   ├── LDC1101/
│   │   ├── schematic.sch
│   │   ├── board.brd
│   │   └── gerbers/
│   ├── FDC2214/
│   │   ├── schematic.sch
│   │   ├── board.brd
│   │   └── gerbers/
│   └── wiring/
│       └── wiring_diagram.png
│
├── images/
│   ├── pcb/
│   │   ├── ldc1101_schematic.png
│   │   ├── ldc1101_pcb.png
│   │   ├── ldc1101_assembled.jpg
│   │   ├── fdc2214_schematic.png
│   │   ├── fdc2214_pcb.png
│   │   └── fdc2214_assembled.jpg
│   ├── setup/
│   │   ├── full_wiring.jpg
│   │   └── project_setup.jpg
│   ├── results/
│   │   ├── laptop_data.png
│   │   └── mobile_app_data.png
│   └── debugging/
│       ├── logic_analyzer.jpg
│       ├── ack_missing.png
│       └── pcb_fault.jpg
│
└── docs/
    └── Project_Report_Plant_Vital_Monitoring.pdf
```

---

<div align="center">

**Made with 🌱 at Technische Universität Chemnitz**
*Chair of Measurement and Sensor Technology*

</div>
