#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include "RTClib.h"

SdFat SD;
RTC_PCF8563 rtc;

// ---------------- PIN DEFINITIONS ----------------
const int chipSelect_SDCard = 9;
const int CS_PIN_LDC = 10;

#define BTSerial Serial1  // UART Bluetooth (0 = RX, 1 = TX)

#define READ_REG  0x80
#define WRITE_REG 0x00

// ---------------- LDC1101 SPI FUNCTIONS ----------------
void writeRegister_LDC(uint8_t reg, uint8_t value) {
  digitalWrite(CS_PIN_LDC, LOW);
  SPI.transfer(WRITE_REG | (reg & 0x3F));
  SPI.transfer(value);
  digitalWrite(CS_PIN_LDC, HIGH);
}

uint8_t readRegister_LDC(uint8_t reg) {
  digitalWrite(CS_PIN_LDC, LOW);
  SPI.transfer(READ_REG | (reg & 0x3F));
  uint8_t val = SPI.transfer(0x00);
  digitalWrite(CS_PIN_LDC, HIGH);
  return val;
}

// ---------------- LDC1101 SETUP ----------------
void LHR_modeSetup() {
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  pinMode(CS_PIN_LDC, OUTPUT);
  digitalWrite(CS_PIN_LDC, HIGH);

  // Wait for Power-On Reset complete
  while (readRegister_LDC(0x20) & 0x01);

  writeRegister_LDC(0x01, 0x05);
  writeRegister_LDC(0x0B, 0x01);
  writeRegister_LDC(0x34, 0x00);

  writeRegister_LDC(0x30, 0xFF);
  writeRegister_LDC(0x31, 0xFF);

  writeRegister_LDC(0x32, 0x00);
  writeRegister_LDC(0x33, 0x00);

  writeRegister_LDC(0x05, 0x01);
  writeRegister_LDC(0x0C, 0x01);
  writeRegister_LDC(0x0B, 0x00);

  Serial.println("LDC1101 Setup Completed");
}

// ---------------- READ LDC DATA ----------------
uint32_t read_LHR_Data() {
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  while (readRegister_LDC(0x3B) & 0x01);

  uint8_t lsb = readRegister_LDC(0x38);
  uint8_t mid = readRegister_LDC(0x39);
  uint8_t msb = readRegister_LDC(0x3A);

  return ((uint32_t)msb << 16) | ((uint32_t)mid << 8) | lsb;
}

// ---------------- SD CARD SETUP ----------------
void SDcard_setup() {
  pinMode(chipSelect_SDCard, OUTPUT);
  digitalWrite(chipSelect_SDCard, HIGH);

  if (!SD.begin(chipSelect_SDCard, SD_SCK_MHZ(10))) {
    Serial.println("SD Card failed!");
    while (1);
  }

  Serial.println("SD Card Initialized");
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);
  while (!Serial);

  BTSerial.begin(9600);

  Wire.begin();

  // RTC INIT
  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  rtc.adjust(DateTime(2025, 6, 29, 13, 25, 0));

  Serial.println("RTC Running");

  SDcard_setup();
  LHR_modeSetup();
}

// ---------------- LOOP ----------------
void loop() {

  uint32_t lhrValue = read_LHR_Data();
  DateTime now = rtc.now();

  // -------- Bluetooth Output --------
  BTSerial.print("LHRData: ");
  BTSerial.println(lhrValue);

  // -------- SD CARD WRITE --------
  FsFile file = SD.open("Data7.txt", FILE_WRITE);

  if (file) {
    file.print("LHRData: ");
    file.print(lhrValue);
    file.print(" | Time: ");
    file.print(now.hour()); file.print(":");
    file.print(now.minute()); file.print(":");
    file.print(now.second());
    file.print(" | Date: ");
    file.print(now.day()); file.print("/");
    file.print(now.month()); file.print("/");
    file.println(now.year());

    file.close();
  } else {
    Serial.println("SD write failed");
  }

  delay(1000);

  // -------- SD CARD READ (DEBUG) --------
  file = SD.open("Data7.txt", FILE_READ);

  if (file) {
    Serial.println("\n--- SD CARD DATA ---");

    while (file.available()) {
      Serial.write(file.read());
    }

    file.close();

    Serial.println("\n--- END ---");
  } else {
    Serial.println("SD read failed");
  }
}
