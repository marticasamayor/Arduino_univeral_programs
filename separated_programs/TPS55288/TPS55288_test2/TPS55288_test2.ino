// test configuration to use TPS55288 with internal registers

#include <Wire.h>

// Adreça per defecte del TPS55288
#define TPS55288_ADDR 0x74  // Adreça I2C correcta del TPS55288

// Pins I2C per ESP32-S3
#define I2C_SDA 8
#define I2C_SCL 9

// Registres
#define REG_VREF_LOW 0x00
#define REG_VREF_HIGH 0x01
#define REG_CURRENT_LIMIT 0x02      // Iout limit
#define REG_RESPONCE_TIME 0x03       // sets the slew rate of the output voltage change
#define REG_FEEDBACK 0x04         // Feedback output & ratio
#define REG_PROTECTION 0x05    // Control del xip
#define REG_MODE 0x06     // mode del xip
#define REG_STATUS 0x07     // Estat del xip


#define PIN_BUTTON_OK 0
#define PIN_EN_BATTERY 38
#define PIN_EN_CAR 35
#define PIN_OUT_FRIDGE 16




float Vout = 5.0;       // Exemple de valor real de Vout
float Vout_max = 15.0;  // Select Vout_max: 5V, 10V, 15V, 20V maximum Vout with Vref set to 1.129V
float Iout_limit = 1.5; // Exemple: 1.5A
float Shunt = 10.0;     //mR Exemple: 10 mΩ → 0.01 Ω


unsigned long temps = 0;



// Registres a llegir abans i després de la configuració
const uint8_t registres[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
const char* noms[] = {"REF (L)", "REF (H)", "IOUT_LIMIT", "VOUT_SR", "VOUT_FS", "CDC", "MODE", "STATUS"};



uint16_t VoutToValue(float Vout, float Vout_max /*5, 10, 15, 20*/) {
  float factor = ((Vout / Vout_max * 1.129) - 0.045) * 1000 / 1.129;
  return round(factor);
}
uint16_t IoutToValue(float Iout_limit, float Shunt) {
  float raw = Iout_limit * Shunt / 0.5;
  return ceil(raw);  // ROUNDUP(...)
}
void escriuRegistre(uint8_t reg, uint16_t valor) {
    Wire.beginTransmission(TPS55288_ADDR);
    Wire.write(reg);
    //Wire.write(valor >> 8);   // Byte alt
    Wire.write(valor & 0xFF); // Byte baix  // només byte baix (els registres són de 8 bits)
    Wire.endTransmission();
    delay(10);
}
void escriuRegistre2(uint8_t reg, uint16_t valor) {
    Wire.beginTransmission(TPS55288_ADDR);
    Wire.write(reg);
    Wire.write((valor >> 8) & 0xFF); // Byte alt
    Wire.write(valor & 0xFF);        // Byte baix
    Wire.endTransmission();
    delay(100);
}
void escriuIComprova(uint8_t reg, uint16_t valor) {
    escriuRegistre(reg, valor);
    delay(10); // Petita pausa per donar temps al xip
    uint8_t llegit = llegeixRegistre(reg);
    Serial.print("Confirmació -> Registre 0x");
    Serial.print(reg, HEX);
    Serial.print(": Escrit 0x");
    Serial.print(valor, HEX);
    Serial.print(" | Llegit 0x");
    Serial.println(llegit, HEX);
}
uint8_t llegeixRegistre(uint8_t reg) {
    Wire.beginTransmission(TPS55288_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {  // Manté el bus actiu
        Serial.println("Error: No es pot comunicar amb el TPS55288!");
        return 0xFF;
    }

    Wire.requestFrom(TPS55288_ADDR, (uint8_t)1);
    if (Wire.available() < 1) {
        Serial.println("Error: No ha arribat resposta del TPS55288!");
        return 0xFF;
    }

    return Wire.read();
}

void mostraTotsElsRegistres() {
    Serial.println("Llegint registres del TPS55288...\n");

    for (int i = 0; i < sizeof(registres); i++) {
        uint8_t valor = llegeixRegistre(registres[i]);
        Serial.print(noms[i]);
        Serial.print(" (0x");
        Serial.print(registres[i], HEX);
        Serial.print("): 0x");
        Serial.println(valor, HEX);
        delay(100);
    }
    Serial.println("\n--- Fi de la lectura ---\n");
}
bool comprovaConnexioI2C() {
    Wire.beginTransmission(TPS55288_ADDR);
    return (Wire.endTransmission() == 0);
}
void setOutputEnabled(bool enable) {
    uint8_t regVal = llegeixRegistre(0x06);
    if (regVal == 0xFF) return; // error de lectura

    if (enable) {
        regVal |= 0b10000000; // activa bit 7
    } else {
        regVal &= 0b01111111; // desactiva bit 7
    }

    escriuRegistre(0x06, regVal);

    // Confirmació opcional
    uint8_t confirm = llegeixRegistre(0x06);
    Serial.print("Sortida ");
    Serial.print(enable ? "activada" : "desactivada");
    Serial.print(" -> Registre 0x06: ");
    Serial.println(confirm, BIN);
}
void configuraTPS55288() {
  escriuRegistre(REG_VREF_LOW,      0x53);   // REG 0
  escriuRegistre(REG_VREF_HIGH,     0x02);   // REG 1
  escriuRegistre(REG_CURRENT_LIMIT, 0x78);   // REG 2
  escriuRegistre(REG_RESPONCE_TIME, 0x01);   // REG 3
  escriuRegistre(REG_FEEDBACK,      0x03);   // REG 4
  escriuRegistre(REG_PROTECTION,    0xE0);   // REG 5
  escriuRegistre(REG_MODE,          0x23);   // REG 6
  delay(100);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9);  // SDA = 8, SCL = 9
  delay(1000);

  pinMode(PIN_BUTTON_OK, INPUT);
  pinMode(PIN_EN_BATTERY, OUTPUT);
  pinMode(PIN_EN_CAR, OUTPUT);
  pinMode(PIN_OUT_FRIDGE, OUTPUT);

  digitalWrite(PIN_EN_BATTERY, HIGH);
  digitalWrite(PIN_EN_CAR, HIGH);
  
  delay(200);
  if (!comprovaConnexioI2C()) {
    Serial.println("No s'ha detectat el TPS55288! Comprova la connexió.");
    // return;
  }else{
    Serial.println("TPS55288 detectat correctament!");
  }

  // mostraTotsElsRegistres();
  configuraTPS55288();
  mostraTotsElsRegistres();

  // uint16_t value = VoutToValue(Vout, Vout_max);
  // Serial.print("Hexadecimal: 0x");
  // Serial.println(value, HEX);


  // value = IoutToValue(Iout_limit, Shunt);

  // Serial.print("Hexadecimal: 0x");
  // Serial.println(value, HEX);

  // setOutputEnabled(true);
  delay(1000);
  // setOutputEnabled(false);

  digitalWrite(PIN_EN_CAR, HIGH);
  digitalWrite(PIN_OUT_FRIDGE, HIGH);
  // escriuRegistre(REG_MODE, 0xA3);

  temps = millis();
}

void loop() {

  if(!digitalRead(PIN_BUTTON_OK)){
    // Serial.println("true");
    // setOutputEnabled(true);
    // digitalWrite(PIN_EN_CAR, HIGH);
    escriuRegistre(REG_VREF_LOW,      0x53);   // REG 0
    escriuRegistre(REG_VREF_HIGH,     0x02);   // REG 1
    escriuRegistre(REG_MODE, 0xA3);
    
    // digitalWrite(PIN_EN_CAR, HIGH);
    delay(500);
  }else{
    // Serial.println("false");
    // setOutputEnabled(false);
    escriuRegistre(REG_MODE, 0x23);
    delay(500);
  }
  // escriuRegistre(REG_MODE, 0x21);
  if (millis() > temps + 1000) {
    // Serial.println("Ha passat 1 segon!");
    Serial.println(llegeixRegistre(REG_MODE), HEX);
    Serial.println(llegeixRegistre(REG_STATUS), HEX);

    mostraTotsElsRegistres();
    Serial.println("-------------------");


    temps = millis(); // actualitza per al següent cicle
  }
}
