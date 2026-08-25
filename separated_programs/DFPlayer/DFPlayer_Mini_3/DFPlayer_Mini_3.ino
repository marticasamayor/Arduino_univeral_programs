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
MFRC522DriverPinSimple ss_pin(10);
MFRC522DriverSPI driver{ss_pin};
MFRC522 reader{driver};

// UIDs de les targetes (format: "B3 2D 0D F8")
#define CARD_RED    ""
#define CARD_GREEN  "DB CD B7 03"
#define CARD_YELLOW "3A 58 98 2E"
#define CARD_BLACK  ""
#define CARD_BLUE   "B3 2D 0D F8"

#define AUDIO_INICI  1
#define AUDIO_RED    2
#define AUDIO_GREEN  1
#define AUDIO_YELLOW 3
#define AUDIO_BLACK  5
#define AUDIO_BLUE   2

// Taula de mapping targeta → pista (afegir/treure targetes aquí)
struct CardEntry {
  const char* uid;
  uint8_t     track;
  const char* name;
};

const CardEntry CARDS[] = {
  { CARD_GREEN,  AUDIO_GREEN,  "verda"  },
  { CARD_YELLOW, AUDIO_YELLOW, "groga"  },
  { CARD_BLUE,   AUDIO_BLUE,   "blava"  },
  // CARD_RED i CARD_BLACK pendents de configurar
};
const uint8_t NUM_CARDS = sizeof(CARDS) / sizeof(CARDS[0]);

// Ignorar si la mateixa targeta que l'última llegida
char lastCardRead[25] = "";

// --- Funcions auxiliars ---

void haltCard() {
  reader.PICC_HaltA();
  reader.PCD_StopCrypto1();
}

// Converteix l'UID llegit a string "XX XX XX XX" (majúscules, zero-padded)
void getCardUID(char* uid) {
  char* ptr = uid;
  for (byte i = 0; i < reader.uid.size; i++) {
    ptr += sprintf(ptr, "%02X", reader.uid.uidByte[i]);
    if (i < reader.uid.size - 1) *ptr++ = ' ';
  }
  *ptr = '\0';
}

void initDFPlayer() {
  FPSerial.begin(9600);
  Serial.println(F("Inicialitzant DFPlayer..."));
  if (!myDFPlayer.begin(FPSerial, true, true)) {
    Serial.println(F("Error DFPlayer:"));
    Serial.println(F("1. Revisa les connexions"));
    Serial.println(F("2. Insereix la targeta SD"));
    while (true) delay(100);
  }
  Serial.println(F("DFPlayer OK"));
  myDFPlayer.volume(8); // Volum 0-30
}

void initRFID() {
  reader.PCD_Init();
  Serial.print(F("Lector RFID: "));
  MFRC522Debug::PCD_DumpVersionToSerial(reader, Serial);
}

// --- Setup / Loop ---

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("=== Sistema RFID + DFPlayer ==="));
  initDFPlayer();
  initRFID();
  // myDFPlayer.play(AUDIO_INICI); // Reproduir àudio d'inici
  Serial.println(F("\nSistema preparat. Acosta una targeta RFID..."));
}

void loop() {
  // Monitoritzar events DFPlayer (errors, SD extreta, fi de reproducció...)
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }

  if (!reader.PICC_IsNewCardPresent()) return;
  if (!reader.PICC_ReadCardSerial()) return;

  char currentCard[25];
  getCardUID(currentCard);

  // Ignorar si és la mateixa targeta que l'última llegida
  if (strcmp(currentCard, lastCardRead) == 0) {
    haltCard();
    return;
  }

  strncpy(lastCardRead, currentCard, sizeof(lastCardRead));

  Serial.print(F("Targeta detectada: "));
  Serial.println(currentCard);

  bool found = false;
  for (uint8_t i = 0; i < NUM_CARDS; i++) {
    if (strcmp(currentCard, CARDS[i].uid) == 0) {
      Serial.print(F("-> Targeta "));
      Serial.print(CARDS[i].name);
      Serial.println(F(" reconeguda!"));
      myDFPlayer.play(CARDS[i].track);
      found = true;
      break;
    }
  }
  if (!found) Serial.println(F("-> Targeta desconeguda"));

  haltCard();
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
        case Busy:             Serial.println(F("SD no trobada"));        break;
        case Sleeping:         Serial.println(F("En repòs"));             break;
        case SerialWrongStack: Serial.println(F("Error de comunicació")); break;
        case CheckSumNotMatch: Serial.println(F("Checksum incorrecte"));  break;
        case FileIndexOut:     Serial.println(F("Arxiu fora de rang"));   break;
        case FileMismatch:     Serial.println(F("Arxiu no trobat"));      break;
        default:               break;
      }
      break;
    default:
      break;
  }
}
