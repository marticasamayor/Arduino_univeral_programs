/*
 * ESP32-C3 -> microSD (SPI) : list + read files
 *
 *   Board:  "ESP32C3 Dev Module"
 *
 * Wiring (SPI):
 *   ESP32-C3 GPIO3  -> MISO  (DO  on module)
 *   ESP32-C3 GPIO7  -> MOSI  (DI  on module)
 *   ESP32-C3 GPIO6  -> SCK   (CLK)
 *   ESP32-C3 GPIO9  -> CS    (chip select)
 *   3V3            -> VCC
 *   GND            -> GND
 *
 * NOTE: GPIO9 is a strapping pin (boot). SD card must NOT hold it low at
 *       reset or the board may enter download mode. If boot fails, add a
 *       10k pull-up to 3V3 on CS, or move CS to another free GPIO.
 */

#include <SPI.h>
#include <SD.h>

// ---- SPI pins ----
static const int PIN_MISO = 3;
static const int PIN_MOSI = 7;
static const int PIN_SCK  = 6;
static const int PIN_CS   = 9;

SPIClass spi(FSPI);   // ESP32-C3 general-purpose SPI bus

// Recursively print directory tree
void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  File root = fs.open(dirname);
  if (!root || !root.isDirectory()) {
    Serial.printf("  not a dir: %s\n", dirname);
    return;
  }
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("  [DIR]  %s\n", file.path());
      if (levels) listDir(fs, file.path(), levels - 1);
    } else {
      Serial.printf("  %-30s %8u B\n", file.path(), (unsigned)file.size());
    }
    file = root.openNextFile();
  }
}

// Dump a text file to Serial
void readFile(fs::FS &fs, const char *path) {
  File file = fs.open(path);
  if (!file) {
    Serial.printf("open failed: %s\n", path);
    return;
  }
  Serial.printf("---- %s (%u B) ----\n", path, (unsigned)file.size());
  while (file.available()) Serial.write(file.read());
  Serial.println("\n---- end ----");
  file.close();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nSD init...");

  spi.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);

  // 4th arg = SPI clock (Hz). Start slow (4 MHz) for reliability.
  if (!SD.begin(PIN_CS, spi, 4000000)) {
    Serial.println("Card mount FAILED. Check wiring / format (FAT32) / pull-up on CS.");
    return;
  }

  uint8_t type = SD.cardType();
  if (type == CARD_NONE) {
    Serial.println("No SD card attached.");
    return;
  }
  const char *t = (type == CARD_MMC) ? "MMC" :
                  (type == CARD_SD)  ? "SDSC" :
                  (type == CARD_SDHC)? "SDHC" : "UNKNOWN";
  Serial.printf("Card type: %s\n", t);
  Serial.printf("Card size: %llu MB\n", SD.cardSize() / (1024ULL * 1024ULL));
  Serial.printf("Used: %llu MB / Total: %llu MB\n",
                SD.usedBytes() / (1024ULL * 1024ULL),
                SD.totalBytes() / (1024ULL * 1024ULL));

  Serial.println("\nFiles:");
  listDir(SD, "/", 2);   // 2 levels deep

  // Example: uncomment to dump a file
  // readFile(SD, "/test.txt");
}

void loop() {
  // nothing
}