#include <Wire.h>

// ======================================================
//                FDC2214 CONFIGURATION
// ======================================================

#define FDC2214_ADDR 0x2B   // Default I2C address

// ======================================================
//                LOW LEVEL I2C FUNCTIONS
// ======================================================

// Write 16-bit register
void FDC_writeRegister(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(FDC2214_ADDR);
  Wire.write(reg);
  Wire.write(value >> 8);     // MSB
  Wire.write(value & 0xFF);   // LSB
  Wire.endTransmission();
}

// Read 16-bit register
uint16_t FDC_readRegister(uint8_t reg) {
  Wire.beginTransmission(FDC2214_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);   // repeated start

  Wire.requestFrom(FDC2214_ADDR, (uint8_t)2);

  if (Wire.available() == 2) {
    uint16_t value = Wire.read() << 8;
    value |= Wire.read();
    return value;
  }

  return 0;
}

// ======================================================
//                SENSOR INITIALIZATION
// ======================================================

void FDC2214_init() {

  delay(50); // allow power-up

  // Enter sleep/config mode
  FDC_writeRegister(0x1A, 0xF200);

  // Set deglitch filter / bandwidth
  FDC_writeRegister(0x1B, 0x020C);

  // Configure channel (example: Channel 3 active)
  FDC_writeRegister(0x19, 0x0003); // ACTIVE_CHAN = CH3

  // Set RCOUNT (resolution)
  FDC_writeRegister(0x14, 0xFFFF); // CH3 RCOUNT

  // Set SETTLECOUNT
  FDC_writeRegister(0x16, 0x0400);

  // Set clock divider
  FDC_writeRegister(0x1A, 0x0000); // Exit sleep mode

  Serial.println("FDC2214 Initialized");
}

// ======================================================
//                READ CHANNEL DATA
// ======================================================

// Read Channel 3 raw 28-bit data
uint32_t FDC2214_readCH3() {

  // Data registers
  uint16_t msb = FDC_readRegister(0x06);
  uint16_t lsb = FDC_readRegister(0x07);

  uint32_t raw =
      ((uint32_t)(msb & 0x0FFF) << 16) | lsb;

  return raw;
}

// ======================================================
//                OPTIONAL: DEVICE CHECK
// ======================================================

uint16_t FDC2214_deviceID() {
  return FDC_readRegister(0x7F);
}

uint16_t FDC2214_manufacturerID() {
  return FDC_readRegister(0x7E);
}

// ======================================================
//                SETUP
// ======================================================

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("Starting FDC2214...");

  Serial.print("Manufacturer ID: ");
  Serial.println(FDC2214_manufacturerID(), HEX);

  Serial.print("Device ID: ");
  Serial.println(FDC2214_deviceID(), HEX);

  FDC2214_init();
}

// ======================================================
//                LOOP
// ======================================================

void loop() {

  uint32_t value = FDC2214_readCH3();

  Serial.print("FDC CH3 Value: ");
  Serial.println(value);

  delay(500);
}
