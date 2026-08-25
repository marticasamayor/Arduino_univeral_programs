// test configuration to use TPS55288 with internal registers

#include <Wire.h>

#define TPS55288_ADDR 0x74  // Adreça I2C correcta del TPS55288

// Registres
#define REG_VSET 0x03       // Voltge de sortida
#define REG_FB 0x04         // Retroalimentació
#define REG_CONTROL 0x05    // Control del xip
#define REG_STATUS 0x07     // Estat del xip
#define REG_MODE 0x06     // mode del xip

#define VREF_DESITJAT 1.0   // VREF per obtenir 18V

// Registres a llegir abans i després de la configuració
const uint8_t registres[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
const char* noms[] = {"REF (L)", "REF (H)", "IOUT_LIMIT", "VOUT_SR", "VOUT_FS", "CDC", "MODE", "STATUS"};

void escriuRegistre(uint8_t reg, uint16_t valor) {
    Wire.beginTransmission(TPS55288_ADDR);
    Wire.write(reg);
    //Wire.write(valor >> 8);   // Byte alt
    Wire.write(valor & 0xFF); // Byte baix
    Wire.endTransmission();
    delay(100);
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
    if (Wire.endTransmission() != 0) {
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

void configuraTPS55288() {
    if (!comprovaConnexioI2C()) {
        Serial.println("No s'ha detectat el TPS55288! Comprova la connexió.");
        return;
    }
    Serial.println("TPS55288 detectat correctament!");

    // 🟢 1. Llegeix tots els registres abans de configurar
    delay(500);
    mostraTotsElsRegistres();

    // escriuIComprova(0x06, 0b00000001); //MODE Register
    escriuIComprova(0x00, 0x94);//0b11000000); //REF (L)
    escriuIComprova(0x01, 0x02);//0b00000011); //REF (H)
    // escriuRegistre2(0x00, 0xC03);
    escriuIComprova(0x02, 0b01100100); //IOUT_LIMIT Register
    escriuIComprova(0x03, 0b00000011); //VOUT_SR Register
    escriuIComprova(0x04, 0b00000011); //VOUT_FS Register  // INTFB[1:0] = 11 (1:18)
    escriuIComprova(0x05, 0b11100000); //CDC register
    // escriuIComprova(0x06, 0b00000000); //MODE Register

    delay(500);

    uint8_t registre = llegeixRegistre(0x06);
    registre |= (1<<7);      // bit 7 a 1
    //Serial.println(registre);

    escriuIComprova(0x06, registre);

    // // registre &= ~(1<<7);     // bit 7 a 0
    // // Serial.println(registre);

    
    Serial.println("output enabled!.");

    delay(500);

    mostraTotsElsRegistres();
}

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);  // SDA = 8, SCL = 9    //arduino uno SDA:A4	SCL:A5
    delay(500);

    Serial.println("Hola");

    configuraTPS55288();
}

void loop() {
    // Aquí pots afegir codi per comprovar el voltatge de sortida si tens un ADC
}
