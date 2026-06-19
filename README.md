# 🌱 Plant Vital Monitoring with Industrial Sensors

> **TU Chemnitz · Department of Electrical Engineering and Information Technology**  
> Chair of Measurement and Sensor Technology · Prof. Dr.-Ing. Olfa Kanoun  
> Project Lab Embedded Systems · Group 11 · July 2025

---

## 📋 Table of Contents
- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Sensor Details](#sensor-details)
- [Circuit & Wiring](#circuit--wiring)
- [Software & Code](#software--code)
- [Results](#results)
- [Challenges & Solutions](#challenges--solutions)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Team](#team)

---

## 🔍 Overview

This project implements a **smart, non-invasive plant health monitoring system** using industrial-grade sensors. It tracks:

- 🌿 **Plant stem motion & growth** — via inductance sensing (LDC1101)
- 💧 **Soil & leaf moisture** — via capacitance sensing (FDC2214)
- 🕐 **Real-time timestamped logging** — via RTC module (HW-689)
- 📡 **Wireless data transmission** — via Bluetooth (BT05)
- 💾 **Local data storage** — via MicroSD card (CSV format)

The system is designed for deployment in greenhouses, vertical farms, research labs, and open-field agriculture.

```
┌─────────────────────────────────────────────────────────┐
│              Plant Vital Monitoring System               │
│                                                         │
│  [LDC1101]──SPI──┐                                     │
│  Inductance       ├──[Teensy 4.0]──I²C──[HW-689 RTC]  │
│  Sensor           │       │                             │
│  [FDC2214]──I²C──┘       ├──SPI──[MicroSD Card]       │
│  Capacitance              │                             │
│  Sensor                   └──UART──[BT05 Bluetooth]    │
└─────────────────────────────────────────────────────────┘
```

---

## 🏗️ System Architecture

```
                        ┌──────────────────┐
                        │   Teensy 4.0     │
                        │  Microcontroller │
                        │  (Central MCU)   │
                        └────────┬─────────┘
               ┌─────────────────┼──────────────────┐
               │                 │                  │
    ┌──────────▼───┐   ┌────────▼──────┐  ┌───────▼──────┐
    │  LDC1101     │   │   FDC2214     │  │   HW-689 RTC │
    │  (SPI)       │   │   (I²C)       │  │   (I²C)      │
    │  Inductance  │   │  Capacitance  │  │  Timestamps  │
    └──────────────┘   └───────────────┘  └──────────────┘
               │
    ┌──────────▼───────────────────────┐
    │         Data Output              │
    ├──────────────────────────────────┤
    │  📱 BT05 Bluetooth → Mobile App │
    │  💾 MicroSD Card → CSV files    │
    
  

```

---

  <img width="600" height="600" alt="WhatsApp Image 2026-06-19 at 13 19 56" src="https://github.com/user-attachments/assets/bb368a04-c118-4321-a6f5-7d1a752ec91d" />

## 🔧 Hardware Components

| Component | Model | Interface | Purpose |
|---|---|---|---|
| Microcontroller | Teensy 4.0 | — | Central processing unit |
| Inductance Sensor | LDC1101 | SPI | Plant stem motion & growth |
| Capacitance Sensor | FDC2214 | I²C | Soil/leaf moisture detection |
| Real-Time Clock | HW-689 (PCF8563) | I²C | Timestamp generation |
| Storage | MicroSD Card Adapter | SPI | CSV data logging |
| Wireless | BT05 Bluetooth Module | UART | Wireless data streaming |

### PCB Design
Both sensor boards (LDC1101 and FDC2214) were designed from scratch in **Autodesk EAGLE**, providing:
- Cleaner signal routing vs. breadboard
- Reduced EMI/noise interference
- Compact, field-deployable form factor

---

## 📡 Sensor Details

### LDC1101 — Inductance-to-Digital Converter

The LDC1101 drives an LC resonator circuit and monitors the energy required to keep it oscillating, measuring:
- **Resonant Frequency (L):** stem growth & displacement
- **Parallel Resistance (Rp):** material properties

**Key Specs:**
| Parameter | Value |
|---|---|
| Supply Voltage | 1.8 V – 3.3 V |
| Frequency Range | 500 kHz – 10 MHz |
| L Resolution | 16-bit or 24-bit |
| Rp Resolution | 16-bit |
| Package | VSON-10 (3×3 mm) |
| Operating Temp | –40°C to +125°C |
| Interface | 4-pin SPI |

**Resonant Frequency Formula:**

$$F_{sensor} = \frac{1}{2\pi\sqrt{L \cdot C}}$$

Where:
- `L` = sensor inductance (Henrys)
- `C` = sensor parallel capacitance (Farads)

---

### FDC2214 — Capacitance-to-Digital Converter

The FDC2214 uses an LC resonator to detect capacitance shifts caused by:
- Soil moisture changes
- Leaf proximity or surface moisture
- Ambient humidity variation

Unlike charge-transfer capacitive sensors, its **narrow-band resonant architecture** makes it virtually immune to EMI.

**Key Specs:**
| Parameter | Value |
|---|---|
| Supply Voltage | 2.7 V – 3.6 V |
| Resolution | Up to 28-bit |
| Active Current | 2.1 mA |
| Max Input Capacitance | 250 nF |
| Package | WQFN-16 (4×4 mm) |
| Operating Temp | –40°C to +125°C |
| Interface | I²C (up to 400 kbps) |

---

## 🔌 Circuit & Wiring

### Wiring Diagram

| Module | Pin | Teensy 4.0 |
|---|---|---|
| **RTC HW-689** | SCL | SCL0 (Pin 19) |
| | SDA | SDA0 (Pin 18) |
| | VCC | 3.3V |
| | GND | GND |
| **MicroSD Adapter** | MISO | MISO (Pin 12) |
| | MOSI | MOSI (Pin 11) |
| | SCK | SCK (Pin 13) |
| | CS | Pin 9 |
| | VCC | 5V |
| | GND | GND |
| **LDC1101 PCB** | SDO/INTB | MISO (Pin 12) |
| | SCLK | SCK (Pin 13) |
| | SDI | MOSI (Pin 11) |
| | CSB | CS (Pin 10) |
| | VDD | 3.3V |
| | GND | GND |
| **BT05 Bluetooth** | TXD | RX1 (Pin 0) |
| | RXD | TX1 (Pin 1) |
| | VCC | 5V |
| | GND | GND |
| **FDC2214 PCB** | SCL | SCL1 (Pin 16) |
| | SDA | SDA1 (Pin 17) |
| | VCC | 3.3V |
| | GND | GND |

> 📁 See [`EAGLE SCH_LDC_FDC.pdf`](EAGLE%20SCH_LDC_FDC.pdf) for EAGLE schematic files.

---

## 💻 Software & Code

### Libraries Required

```cpp
#include <SPI.h>       // SPI communication (LDC1101, SD Card)
#include <Wire.h>      // I²C communication (FDC2214, RTC)
#include <SdFat.h>     // SD card file system
#include "RTClib.h"    // RTC module interface
```

Install via Arduino/Teensyduino Library Manager:
- `SdFat` by Bill Greiman
- `RTClib` by Adafruit

### SPI Communication (LDC1101)

The LDC1101 must be configured while in **Sleep Mode**. Registers are accessed with a 16-bit SPI transaction:

```
CSB goes LOW → [R/W bit | 7-bit address] → [8-bit data] → CSB goes HIGH
```

- **Write:** `0x00 | (reg & 0x3F)` as command byte
- **Read:** `0x80 | (reg & 0x3F)` as command byte
- SPI settings: **1 MHz, MSBFIRST, SPI_MODE0**

### I²C Communication (FDC2214)

- Default I²C address: `0x2B` (ADDR pin HIGH) or `0x2A` (ADDR pin LOW)
- Max speed: **400 kbps**
- 16-bit register read/write with standard I²C framing
- ADDR pin must remain stable after exiting Shutdown Mode

### Source Files

| File | Description |
|---|---|
| [LDC1101 Firmware](src/LDC1101_main.ino) | Full LDC1101 data acquisition + SD + Bluetooth |
| [`src/FDC2214_main.ino`](src/FDC2214_main.ino) | FDC2214 capacitance sensing via I²C |

---

## 📊 Results

### LDC1101 — Measured Output

With `CLKIN = 16 MHz` and `C = 820 nF`:

```
LHR Output (raw) = 1,801,089

F_sensor = (LHROutput / 2²⁴) × CLKin
         = (1,801,089 / 16,777,216) × 16 × 10⁶
         = 1.717 MHz

Using F_sensor = 1/(2π√(L·C)):
→ L = 10.5 nH
```

### Sample SD Card Log

```
*****SD Card Data*****
LHRData: 1897600   Time: 15:0:0   Date: 30/6/2025
LHRData: 1630428   Time: 15:0:1   Date: 30/6/2025
LHRData: 1892504   Time: 15:0:2   Date: 30/6/2025
LHRData: 1893464   Time: 15:0:3   Date: 30/6/2025
LHRData: 1892584   Time: 15:0:5   Date: 30/6/2025
*****SD Card END*****
```

### Bluetooth Mobile App (BlueLight for Android)

Live readings streamed over BT05 to smartphone:
```
1:14:49.875 PM  LHRData: 1811264
1:14:48.825 PM  LHRData: 1810716
1:14:47.745 PM  LHRData: 1811497
1:14:46.665 PM  LHRData: 1811140
```

---

## ⚠️ Challenges & Solutions

### FDC2214 I²C Communication Failure

**Problem:** The ACK bit was missing in I²C responses — the sensor wasn't responding to address calls from the Teensy.

**Diagnosis:** Used a logic analyzer to capture the I²C bus signals and confirmed that `NACK` was being returned at the address phase.

**Root Cause:** A fault in the PCB layout caused incorrect connections at the I²C lines (SDA/SCL routed to wrong pads on the WQFN chip footprint).

**Solution:** Inspected PCB under magnification, identified the misrouted traces, redesigned the PCB layout in EAGLE, and fabricated a corrected board — resolving the communication issue.

---

## 🚀 Getting Started

### Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) + [Teensyduino](https://www.pjrc.com/teensy/td_download.html)
- Teensy 4.0 board selected in IDE
- Required libraries installed (see above)

### Steps

```bash
# 1. Clone this repository
git clone https://github.com/YOUR_USERNAME/plant-vital-monitoring.git
cd plant-vital-monitoring

# 2. Open source file in Arduino IDE
#    For LDC1101: open src/LDC1101_main.ino
#    For FDC2214: open src/FDC2214_main.ino

# 3. Wire your hardware (see wiring table above)

# 4. Select board: Tools → Board → Teensy 4.0
#    Select port: Tools → Port → (your Teensy port)

# 5. Upload and open Serial Monitor at 9600 baud
```

### Bluetooth Monitoring
1. Install **BlueLight** app (Android) or any Bluetooth terminal
2. Pair your phone with the BT05 module
3. Connect and receive live sensor data wirelessly

---

## 📁 Project Structure

```
plant-vital-monitoring/
│
├── README.md
│
├── src/
│   ├── LDC1101_main.ino        → ./src/LDC1101_main.ino
│   └── FDC2214_main.ino        → ./src/FDC2214_main.ino
│
├── hardware/
│   ├── schematics/
│   │   ├── LDC1101_schematic.png  → ./hardware/schematics/LDC1101_schematic.png
│   │   └── FDC2214_schematic.png  → ./hardware/schematics/FDC2214_schematic.png
│   │
│   └── pcb/
│       ├── LDC1101_pcb.png        → ./hardware/pcb/LDC1101_pcb.png
│       └── FDC2214_pcb.png        → ./hardware/pcb/FDC2214_pcb.png
│
├── docs/
│   └── Project_Report.pdf       → ./docs/Project_Report.pdf
│
└── images/
    ├── system_overview.png      → ./images/system_overview.png
    ├── ldc1101_assembled.png    → ./images/ldc1101_assembled.png
    ├── fdc2214_assembled.png    → ./images/fdc2214_assembled.png
    ├── wiring_diagram.png       → ./images/wiring_diagram.png
    ├── serial_output.png        → ./images/serial_output.png
    └── bluetooth_output.png     → ./images/bluetooth_output.png
```

---

## 👥 Team

| Name | Responsibility |
|---|---|
| Randhir Bhagat | Hardware Design (FDC2214) & Presentation |
| Rushikesh Lakadkar | Hardware Design (LDC1101) & Documentation |
| Komma Siva Vyshnavi Reddy | Configuration of Bluetooth, RTC, and SD Card |
| Sheikh Zeeshan Ahmed | Configuration of FDC & LDC with Teensy 4.0, Setup Integration |

**Institution:** Technische Universität Chemnitz  
**Department:** Electrical Engineering and Information Technology  
**Chair:** Measurement and Sensor Technology  
**Supervisor:** Prof. Dr.-Ing. Olfa Kanoun  
**Date:** July 2025

---

## 📚 References

1. Texas Instruments. *LDC1101 1.8-V High-Resolution, High-Speed Inductance-to-Digital Converter* [Datasheet]. https://www.ti.com/lit/ds/symlink/ldc1101.pdf
2. Texas Instruments. *FDC2214 Product Page*. https://www.ti.com/product/FDC2214

---

## 📄 License

This project was developed as part of an academic course at TU Chemnitz. Code is provided for educational purposes.
