int i = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("holaa");
}

void loop() {
  Serial.println(i++);
  delay(500);
}



// In Arduino IDE, select "Enable CDC on boot".//ESP32c3 - No serial output from built in jtag hardware//https://github.com/espressif/arduino-esp32/issues/6545
