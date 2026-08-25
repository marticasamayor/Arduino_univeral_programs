// I2C SRF10 or SRF08 Devantech Ultrasonic Ranger Finder
// by Nicholas Zambetti <http://www.zambetti.com>
// and James Tichenor <http://www.jamestichenor.net>
// Demonstrates use of the Wire library reading data from the
// Devantech Utrasonic Rangers SFR08 and SFR10
// Created 29 April 2006
// This example code is in the public domain.
#include <Wire.h>
float temp, adc, volt, resis, ohms, k;

void setup() {
  Wire.begin();                // join i2c bus (address optional for master)
  Serial.begin(9600);          // start serial communication at 9600bps
}

int reading = 0;
void loop() {
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(0x48); // transmit to device #112 (0x70) //                                  << i2c address
/*
  Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)
  Wire.write(byte(0x50));      // command sensor to measure in "inches" (0x50)
  // use 0x51 for centimeters
  // use 0x52 for ping microseconds
  Wire.endTransmission();      // stop transmitting
  // step 2: wait for readings to happen
  delay(70);                   // datasheet suggests at least 65 milliseconds
  // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(112); // transmit to device #112
  Wire.write(byte(0x02));      // sets register pointer to echo #1 register (0x02)
  */
  Wire.endTransmission();      // stop transmitting
  // step 4: request reading from sensor
  Wire.requestFrom(0x48, 2);    // request 2 bytes from peripheral device #112 //                     << i2c address
  // step 5: receive reading from sensor
  if (2 <= Wire.available()) { // if two bytes were received
    reading = Wire.read();  // receive high byte (overwrites previous reading)
    reading = reading << 8;    // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    /*Serial.print(reading, DEC);   // print the reading
    Serial.print(" - ");
    Serial.print(reading, HEX);   // print the reading
    Serial.print(" - ");*/
    //Serial.print(reading, BIN);   // print the reading
    //Serial.print(" - adc: ");
    
    adc = reading & 2047;//4095;//1023;
    //Serial.print(adc);

    ohms= 100000 * ((2047 / adc)-1);
    //Serial.print(" - Ohms: ");
    //Serial.print(ohms);
    k = 0.23; //25/100;
    temp=k*ohms;
    //Serial.print(" - temp: ");
    //Serial.print(temp);

    volt=3.3*(100000/(ohms+100000));
    //Serial.print(" - Vdrop: ");
    //Serial.println(volt);

    //Serial.print(">> temperatura = (25/100k)* ");
    //Serial.print(ohms);
    //Serial.print(" = ");
    Serial.println(temp/1000);

  }
  delay(50);                  // wait a bit since people have to read the output :)
}
