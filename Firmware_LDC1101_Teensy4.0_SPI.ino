#include <SPI.h>
#include <Wire.h>
#include <SdFat.h>
#include "RTClib.h"

// ===================== SD CARD =====================
SdFat SD;
const int SD_CS = 9;

// ===================== LDC1101 (SPI) =====================
const int LDC_CS = 10;

#define READ_REG  0x80
#define WRITE_REG 0x00

// ===================== FDC2214 (I2C) =====================
#define FDC_ADDR 0x2B

// ===================== RTC =====================
RTC_PCF8563 rtc;

// ===================== BLUETOOTH =====================
#define BTSerial Serial1

// ======================================================
//                    LDC1101 FUNCTIONS
// ======================================================
void writeRegister_LDC(uint8_t reg, uint8_t value) {
  digitalWrite(LDC_CS, LOW);
  SPI.transfer(WRITE_REG | (reg & 0x3F));
  SPI.transfer(value);
  digitalWrite(LDC_CS, HIGH);
}

uint8_t readRegister_LDC(uint8_t reg) {
  digitalWrite(LDC_CS, LOW);
  SPI.transfer(READ_REG | (reg & 0x3F));
  uint8_t val = SPI.transfer(0x00);
  digitalWrite(LDC_CS, HIGH);
  return val;
}

void LDC_setup() {
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  pinMode(LDC_CS, OUTPUT);
  digitalWrite(LDC_CS, HIGH);

  while (readRegister_LDC(0x20) & 0x01);

  writeRegister_LDC(0x01, 0x05);
  writeRegister_LDC(0x0B, 0x00);
  writeRegister_LDC(0x30, 0xFF);
  writeRegister_LDC(0x31, 0xFF);
  writeRegister_LDC(0x32, 0x00);
  writeRegister_LDC(0x33, 0x00);

  Serial.println("LDC1101 Ready");
}

uint32_t read_LDC() {
  while (readRegister_LDC(0x3B) & 0x01);

  uint8_t lsb = readRegister_LDC(0x38);
  uint8_t mid = readRegister_LDC(0x39);
  uint8_t msb = readRegister_LDC(0x3A);

  return ((uint32_t)msb << 16) | ((uint32_t)mid << 8) | lsb;
}

// ======================================================
//                    FDC2214 FUNCTIONS
// ======================================================
void write_FDC(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(FDC_ADDR);
  Wire.write(reg);
  Wire.write(value >> 8);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}

uint16_t read_FDC(uint8_t reg) {
  Wire.beginTransmission(FDC_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(FDC_ADDR, (uint8_t)2);

  if (Wire.available() == 2) {
    return (Wire.read() << 8) | Wire.read();
  }
  return 0;
}

void FDC_setup() {
  write_FDC(0x1A, 0xF200); // channel config
  write_FDC(0x1B, 0x020C); // filter
  write_FDC(0x1A, 0x0000); // exit sleep

  Serial.println("FDC2214 Ready");
}

uint32_t FDC_read() {
  uint16_t msb = read_FDC(0x06);
  uint16_t lsb = read_FDC(0x07);

  return ((uint32_t)(msb & 0x0FFF) << 16) | lsb;
}

// ======================================================
//                    SD CARD
// ======================================================
void SD_setup() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  if (!SD.begin(SD_CS, SD_SCK_MHZ(10))) {
    Serial.println("SD FAIL");
    while (1);
  }

  Serial.println("SD Ready");
}

// ======================================================
//                    SETUP
// ======================================================
void setup() {
  Serial.begin(9600);
  while (!Serial);

  BTSerial.begin(9600);

  Wire.begin();

  // RTC
  if (!rtc.begin()) {
    Serial.println("RTC FAIL");
    while (1);
  }

  rtc.adjust(DateTime(2025, 6, 29, 13, 25, 0));

  SD_setup();
  LDC_setup();
  FDC_setup();
}

// ======================================================
//                    LOOP
// ======================================================
void loop() {

  // ---------- READ ALL SENSORS ----------
  uint32_t ldcValue = read_LDC();
  uint32_t fdcValue = FDC_read();
  DateTime now = rtc.now();

  // ---------- BLUETOOTH OUTPUT ----------
  BTSerial.print("LDC:");
  BTSerial.print(ldcValue);
  BTSerial.print(" | FDC:");
  BTSerial.println(fdcValue);

  // ---------- SD LOGGING ----------
  FsFile file = SD.open("DATA.txt", FILE_WRITE);

  if (file) {
    file.print("LDC:");
    file.print(ldcValue);

    file.print(" | FDC:");
    file.print(fdcValue);

    file.print(" | TIME:");
    file.print(now.hour()); file.print(":");
    file.print(now.minute()); file.print(":");
    file.print(now.second());

    file.print(" | DATE:");
    file.print(now.day()); file.print("/");
    file.print(now.month()); file.print("/");
    file.println(now.year());

    file.close();
  }

  delay(1000);
}
