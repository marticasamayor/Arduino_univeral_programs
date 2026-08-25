// LLEGEIX TOTS ELS REGISTRES DE COP

// uint16_t readRegister(uint8_t reg) {
//   Wire.beginTransmission(BQ25756_I2C_ADDRESS);
//   Wire.write(reg);
//   Wire.endTransmission();
//   Wire.requestFrom(BQ25756_I2C_ADDRESS, (uint8_t)2);
//   uint16_t msb = Wire.read();
//   uint16_t lsb = Wire.read();
//   return (msb << 8) | lsb;
// }

// void writeRegister(uint8_t reg, uint16_t value) {
//   Wire.beginTransmission(BQ25756_I2C_ADDRESS);
//   Wire.write(reg);
//   Wire.write((uint8_t)(value >> 8));    // MSB
//   Wire.write((uint8_t)(value & 0xFF));  // LSB
//   Wire.endTransmission();
// }

#include <Wire.h>

#define BQ25756_I2C_ADDRESS  0x6B   // Exempe: revisar l’adreça real del dispositiu

uint8_t llegeixRegistre(uint8_t reg) {
    Wire.beginTransmission(BQ25756_I2C_ADDRESS);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {  // Manté el bus actiu
        Serial.println("Error: No es pot comunicar");
        return 0xFF;
    }

    Wire.requestFrom(BQ25756_I2C_ADDRESS, (uint8_t)1);
    if (Wire.available() < 1) {
        Serial.println("Error: No ha arribat resposta");
        return 0xFF;
    }

    return Wire.read();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(1000);
  

  for(int i=0; i < 62; i++){
    Serial.print("> 0x");
    Serial.print(i, HEX);
    Serial.print(" ");
    Serial.print(llegeixRegistre(i), BIN);
    Serial.print(" ");
    Serial.println(llegeixRegistre(i), HEX);
  }



  Serial.println("LLEGIT!.");
}

void loop() {
}
