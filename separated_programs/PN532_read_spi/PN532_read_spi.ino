/*
 * ESP32-C3 -> PN532 NFC module V3 (SPI) : read tag UID
 *
 *   Board:  "ESP32C3 Dev Module"
 *   Lib:    "Adafruit PN532" (+ Adafruit BusIO)
 *   Also enable: Tools -> USB CDC On Boot -> Enabled  (Serial over USB)
 *
 * Module DIP switches -> SPI mode:  SEL0=OFF(0), SEL1=ON(1)  (check silkscreen)
 *
 * Wiring (SPI):
 *   ESP32-C3 GPIO3  -> MISO
 *   ESP32-C3 GPIO7  -> MOSI
 *   ESP32-C3 GPIO6  -> SCK
 *   ESP32-C3 GPIO10 -> SS/CS
 *   3V3            -> VCC   (PN532 V3 accepts 3.3V)
 *   GND            -> GND
 */

#include <SPI.h>
#include <Adafruit_PN532.h>

// ---- SPI pins ----
static const int PIN_MISO = 3;
static const int PIN_MOSI = 7;
static const int PIN_SCK  = 6;
static const int PIN_CS   = 10;

// Software-SPI constructor (bit-bang) -> works on any GPIOs, robust for PN532
Adafruit_PN532 nfc(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nPN532 init...");

  nfc.begin();

  uint32_t ver = nfc.getFirmwareVersion();
  if (!ver) {
    Serial.println("PN532 NOT found. Check DIP=SPI, wiring, power.");
    while (true) delay(1000);
  }
  Serial.printf("Found PN5%02X  fw %d.%d\n",
                (ver >> 24) & 0xFF, (ver >> 16) & 0xFF, (ver >> 8) & 0xFF);

  nfc.SAMConfig();   // configure Secure Access Module for reading
  Serial.println("Ready. Bring a tag closer...");
}

void loop() {
  uint8_t uid[7] = {0};
  uint8_t uidLen = 0;

  // Blocks until a card is seen (ISO14443A / Mifare)
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen)) {
    Serial.print("Tag UID (");
    Serial.print(uidLen);
    Serial.print(" bytes): ");
    for (uint8_t i = 0; i < uidLen; i++) {
      if (uid[i] < 0x10) Serial.print('0');
      Serial.print(uid[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
    delay(1000);   // debounce, avoid spamming same tag
  }
}