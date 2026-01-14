/*
 * DFPlayer + MFRC522 RFID
 * Detecta targetes RFID i reprodueix àudio corresponent
 */

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

// Configuració DFPlayer
SoftwareSerial softSerial(2, 3); // RX, TX
#define FPSerial softSerial
DFRobotDFPlayerMini myDFPlayer;

// Configuració RFID
MFRC522DriverPinSimple ss_pin(10); // Pin SS
MFRC522DriverSPI driver{ss_pin};
MFRC522 reader{driver};

// UIDs de les targetes (format: "B3 2D 0D F8")
#define CARD_RED ""
#define CARD_GREEN "DB CD B7 03"
#define CARD_YELLOW ""
#define CARD_BLACK ""
#define CARD_BLUE "B3 2D 0D F8"

#define AUDIO_INICI   1
#define AUDIO_RED     2
#define AUDIO_GREEN   1
#define AUDIO_YELLOW  4
#define AUDIO_BLACK   5
#define AUDIO_BLUE    2



// Variable per evitar lectures repetides
String lastCardRead = "";
unsigned long lastReadTime = 0;
const unsigned long READ_DELAY = 1000; // Mil·lisegons entre lectures de la mateixa targeta

void setup() {
  // Inicialitzar comunicació serial
  FPSerial.begin(9600);
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(F("=== Sistema RFID + DFPlayer ==="));
  
  // Inicialitzar DFPlayer
  Serial.println(F("Inicialitzant DFPlayer..."));
  if (!myDFPlayer.begin(FPSerial, true, true)) {
    Serial.println(F("Error DFPlayer:"));
    Serial.println(F("1. Revisa les connexions"));
    Serial.println(F("2. Insereix la targeta SD"));
    while(true) {
      delay(100);
    }
  }
  
  Serial.println(F("DFPlayer OK"));
  myDFPlayer.volume(12); // Volum 0-30
  
  // Inicialitzar RFID
  reader.PCD_Init();
  Serial.print(F("Lector RFID: "));
  MFRC522Debug::PCD_DumpVersionToSerial(reader, Serial);


  // myDFPlayer.play(AUDIO_INICI); // Reproduir arxiu 0001.mp3
  
  Serial.println(F("\nSistema preparat. Acosta una targeta RFID..."));
}

void loop() {
  // Comprovar si hi ha una nova targeta present
  if (!reader.PICC_IsNewCardPresent()) {
    return; // No hi ha targeta, sortir
  }
  
  // Intentar llegir la targeta
  if (!reader.PICC_ReadCardSerial()) {
    return; // No s'ha pogut llegir, sortir
  }
  
  // Convertir UID a String
  String currentCard = getCardUID();
  
  // Evitar lectures repetides de la mateixa targeta
  if (currentCard == lastCardRead && 
      (millis() - lastReadTime) < READ_DELAY) {
    reader.PICC_HaltA();
    reader.PCD_StopCrypto1();
    return;
  }
  
  // Actualitzar última targeta llegida
  lastCardRead = currentCard;
  lastReadTime = millis();
  
  // Mostrar UID
  Serial.print(F("Targeta detectada: "));
  Serial.println(currentCard);
  
  // Comprovar si coincideix amb les targetes configurades
  if (currentCard == CARD_RED) {
    Serial.println(F("-> Targeta vermella reconeguda!"));
    myDFPlayer.play(AUDIO_RED); // Reproduir arxiu 0002.mp3
  }
  else if (currentCard == CARD_GREEN) {
    Serial.println(F("-> Targeta verda reconeguda!"));
    myDFPlayer.play(AUDIO_GREEN);
  }
  else if (currentCard == CARD_YELLOW) {
    Serial.println(F("-> Targeta groga reconeguda!"));
    myDFPlayer.play(AUDIO_YELLOW); // Reproduir arxiu 0004.mp3
  }
  else if (currentCard == CARD_BLACK) {
    Serial.println(F("-> Targeta negre reconeguda!"));
    myDFPlayer.play(AUDIO_BLACK); // Reproduir arxiu 0005.mp3
  }
  else if (currentCard == CARD_BLUE) {
    Serial.println(F("-> Targeta blava reconeguda!"));
    myDFPlayer.play(AUDIO_BLUE); // Reproduir arxiu 0006.mp3
  }
  else {
    Serial.println(F("-> Targeta desconeguda"));
  }
  
  // Aturar la comunicació amb la targeta
  reader.PICC_HaltA();
  reader.PCD_StopCrypto1();
}

// Funció per obtenir l'UID com a String
String getCardUID() {
  String uid = "";
  
  for (byte i = 0; i < reader.uid.size; i++) {
    // Afegir zero davant si és necessari
    if (reader.uid.uidByte[i] < 0x10) {
      uid += "0";
    }
    uid += String(reader.uid.uidByte[i], HEX);
    
    // Afegir espai entre bytes (excepte l'últim)
    if (i < reader.uid.size - 1) {
      uid += " ";
    }
  }
  
  uid.toUpperCase(); // Convertir a majúscules
  return uid;
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("SD Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("SD Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("SD Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Reproducció "));
      Serial.print(value);
      Serial.println(F(" finalitzada"));
      break;
    case DFPlayerError:
      Serial.print(F("Error DFPlayer: "));
      switch (value) {
        case Busy:
          Serial.println(F("SD no trobada"));
          break;
        case Sleeping:
          Serial.println(F("En repòs"));
          break;
        case SerialWrongStack:
          Serial.println(F("Error de comunicació"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Checksum incorrecte"));
          break;
        case FileIndexOut:
          Serial.println(F("Arxiu fora de rang"));
          break;
        case FileMismatch:
          Serial.println(F("Arxiu no trobat"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}