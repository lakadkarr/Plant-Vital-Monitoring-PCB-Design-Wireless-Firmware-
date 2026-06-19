// ======================================================
//                FDC2214 I2C DRIVER (Teensy)
// ======================================================

#define FDC2214_ADDR 0x2B   // Default I2C address

#include <Wire.h>

// ---------------- WRITE REGISTER ----------------
void writeRegister_FDC(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(FDC2214_ADDR);
  Wire.write(reg);
  Wire.write(value >> 8);     // MSB
  Wire.write(value & 0xFF);   // LSB
  Wire.endTransmission();
}

// ---------------- READ REGISTER ----------------
uint16_t readRegister_FDC(uint8_t reg) {
  Wire.beginTransmission(FDC2214_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);   // repeated start

  Wire.requestFrom(FDC2214_ADDR, (uint8_t)2);

  if (Wire.available() == 2) {
    uint16_t data = Wire.read() << 8;
    data |= Wire.read();
    return data;
  }

  return 0;
}
