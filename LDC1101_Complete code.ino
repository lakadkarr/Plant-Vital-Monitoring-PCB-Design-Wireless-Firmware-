#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include "RTClib.h"
SdFat SD; //SD card variable
RTC_PCF8563 rtc;  // Create RTC object
const int chipSelect_SDCard = 9;//CS for SD Card
const int CS_PIN_LDC = 10;  // Chip Select pin for LDC1101
#define BTSerial Serial1  // Bluetooth via Serial1 (Pins 0 and 1)
#define READ_REG  0x80//read enable byte for LDC
#define WRITE_REG 0x00//write enable  byte for LDC

//write operation for LDC
void writeRegister_LDC(uint8_t reg, uint8_t value) {
  digitalWrite(CS_PIN_LDC, LOW);//enable CS of LDC1101, select as slave
  SPI.transfer(WRITE_REG | (reg & 0x3F));  // write command,enable writeoperation, register address must not increment above 3F
  SPI.transfer(value);//write data
  digitalWrite(CS_PIN_LDC, HIGH);//disable CS of LDC1101, deselect slave
}
//read operation for LDC
uint8_t readRegister_LDC(uint8_t reg) {
  digitalWrite(CS_PIN_LDC, LOW);//enable CS of LDC1101, select as slave
  SPI.transfer(READ_REG | (reg & 0x3F));  // read command, enable read operation, register address must not increment above 3F
  uint8_t val = SPI.transfer(0x00);//read data
  digitalWrite(CS_PIN_LDC, HIGH);//disable CS of LDC1101, deselect slave
  return val;
}

void LHR_modeSetup()
{
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));  // 1 MHz, Mode 0 (set Clock idle low, sample on rising edge), send MSB first
  pinMode(CS_PIN_LDC, OUTPUT);//set CS pin as output
  digitalWrite(CS_PIN_LDC, HIGH);//set LDC1101 slave, disable mode
  //Device in Power-On Reset – device should only be configured when POR_READ = 0.
  while(readRegister_LDC(0x20) & 0x01);// Device is currently in reset; wait until POR_READ = 0, Loop while bit 0 == 1
  writeRegister_LDC(0x01,0x05);// RPMIN = 3 kΩ
  writeRegister_LDC(0x0B,0x01);//(START_CONFIG) activate sleep mode, register contents are maintained
  writeRegister_LDC(0x34, 0x00);  //Sensor Frequency not divided (default value)
  // Set LHR_RCOUNT to 0xFFFF for high resolution
  writeRegister_LDC(0x30, 0xFF);  // LHR_RCOUNT_LSB 
  writeRegister_LDC(0x31, 0xFF);  // LHR_RCOUNT_MSB
  //set offset to LHR data 0x0000
  writeRegister_LDC(0x32, 0x00);  // LHR_OFFSET_LSB 
  writeRegister_LDC(0x33, 0x00);  // LHR_OFFSET_MSB 
  //configuration LHR mode
  writeRegister_LDC(0x05,0x01);//ALT_CONFIG.LOPTIMAL(register 0x05-bit0) = 1
  writeRegister_LDC(0x0C,0x01);//D_CONFIG.DOK_REPORT (register 0x0C-bit0) = 1
  writeRegister_LDC(0x0B,0x00);//(START_CONFIG) disable sleep mode
  Serial.print("LHR Setup Completed\n");
}

uint32_t read_LHR_Data()
{
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));  // 1 MHz, Mode 0 (set Clock idle low, sample on rising edge), send MSB first
  while (readRegister_LDC(0x3B) & 0x01);  // Determine LHR data ready, (LHR Status Register) Loop while bit 0 == 1
  uint8_t lsb = readRegister_LDC(0x38); // LHR_DATA_LSB
  uint8_t mid = readRegister_LDC(0x39); // LHR_DATA_MID
  uint8_t msb = readRegister_LDC(0x3A); // LHR_DATA_MSB
  uint32_t lhrValue = ((uint32_t)msb << 16) | ((uint32_t)mid << 8) | lsb;
  return lhrValue;
}

void SDcard_setup()
{
  pinMode(chipSelect_SDCard, OUTPUT);  // set CS for SDcard as output
  digitalWrite(chipSelect_SDCard, HIGH);//sd card CS as high
  if (!SD.begin(chipSelect_SDCard, SD_SCK_MHZ(10))) {  // Force SPI speed to 10 MHz
    Serial.println("Card failed or not present.");
    while (1);
  }
  Serial.println("Card initialized.");
}
void setup() {
  Serial.begin(9600);
  while (!Serial);
  BTSerial.begin(9600);    // BT-05 default baud rate
  Wire.begin();  // Initialize I2C,module as Master
//check the connection of RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // Set time (only run once to set)
   rtc.adjust(DateTime(2025, 6, 29, 13, 25, 0));//set starting date
   Serial.println("\nRTC is running.");
  SDcard_setup();//call SD Card setup function
  LHR_modeSetup();//Call LHR setup function
}

void loop() {
  
  Serial.print("\n");//Display
  uint32_t lhrValue = read_LHR_Data();
  //Read Time Value
  DateTime now = rtc.now();
  BTSerial.print("LHRData: " + String(lhrValue));//send data on bluethoot
  // --------- WRITE in FILE ----------
  FsFile file = SD.open("Data7.txt", FILE_WRITE);//open text file in SD card
  if (file) {
  file.println("  LHRData: " + String(lhrValue) + "   Time: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "  Date: " + String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()));
  file.close();//close write file
  }
  else {
      Serial.println("Failed to open file.");
   }
   delay(1000);//delay of 1000 msec

   // --------- READ FROM FILE ----------
 file = SD.open("Data7.txt", FILE_READ);
  if (file) {
    Serial.print("\n   *****SD Card Data***** \n");
    while (file.available()) {//check file is available or not
      char c = file.read();//read file
      Serial.print(c);
    }
    file.close();
    Serial.print("   *****SD Card END***** \n");
  } else {
    Serial.println("Failed to open file for reading.");
  }
}
