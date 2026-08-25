// FUNCIONS CORREGIDES PER BQ25756

#include <Wire.h>

#define BQ25756_I2C_ADDRESS  0x6B

// Escriu un registre de 8 bits (registres de control)
void escriuRegistre8(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(BQ25756_I2C_ADDRESS);
  Wire.write(reg);
  Wire.write(value);  // Només 1 byte
  Wire.endTransmission();
}

// Escriu un registre de 16 bits (límits i configuracions grans)
void escriuRegistre16(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(BQ25756_I2C_ADDRESS);
  Wire.write(reg);
  Wire.write((uint8_t)(value & 0xFF));      // LSB primer!
  Wire.write((uint8_t)(value >> 8));        // MSB després!
  Wire.endTransmission();
}

uint8_t llegeixRegistre(uint8_t reg) {
    Wire.beginTransmission(BQ25756_I2C_ADDRESS);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {
        Serial.println("Error: No es pot comunicar");
        return 0xFF;
    }

    Wire.requestFrom((int)BQ25756_I2C_ADDRESS, 1);
    if (Wire.available() < 1) {
        Serial.println("Error: No ha arribat resposta");
        return 0xFF;
    }

    return Wire.read();
}

uint16_t llegeixRegistreDoble(uint8_t adresa_MSB) {
  Wire.beginTransmission(BQ25756_I2C_ADDRESS);
  Wire.write(adresa_MSB);
  if (Wire.endTransmission(false) != 0) {
    Serial.println("Error: No es pot comunicar (Doble)");
  return 0xFFFF;
  }

  Wire.requestFrom((int)BQ25756_I2C_ADDRESS, 2);

  if (Wire.available() < 2) {
  Serial.println("Error: No han arribat 2 bytes");
  return 0xFFFF;
  }

  //  uint8_t regAlt = Wire.read();
  //  uint8_t regBaix = Wire.read();
  uint8_t regBaix = Wire.read();
  uint8_t regAlt = Wire.read();

  return ((uint16_t)regAlt << 8) | regBaix;
}

bool setBitRegistre(uint8_t adresa, uint8_t bit, bool valor_bool) {
    if (bit > 7) {
        return false;
    }

    uint8_t regVal = llegeixRegistre(adresa);

    if (regVal == 0xFF) {
        return false; 
    }

    if (valor_bool) {
        regVal |= (1 << bit);
    } else {
        regVal &= ~(1 << bit);
    }

    // USA LA FUNCIÓ DE 8 BITS!
    escriuRegistre8(adresa, regVal);

    return true;
}

void llegeixTotsElsRegistres() {
  Serial.println("\n========== TOTS ELS REGISTRES ==========");
  
  Serial.print("Charge Voltage Limit :              ");
  Serial.println(llegeixRegistreDoble(0x00), BIN);

  Serial.print("Charge Current Limit :              ");
  Serial.println(llegeixRegistreDoble(0x02), BIN);

  Serial.print("Input Current DPM Limit :           ");
  Serial.println(llegeixRegistreDoble(0x06), BIN);

  Serial.print("Input Voltage DPM Limit :           ");
  Serial.println(llegeixRegistreDoble(0x08), BIN);

  Serial.print("Reverse Mode Input Current Limit :  ");
  Serial.println(llegeixRegistreDoble(0x0A), BIN);

  Serial.print("Reverse Mode Input Voltage Limit :  ");
  Serial.println(llegeixRegistreDoble(0x0C), BIN);

  Serial.print("Precharge Current Limit :           ");
  Serial.println(llegeixRegistreDoble(0x10), BIN);

  Serial.print("Termination Current Limit :         ");
  Serial.println(llegeixRegistreDoble(0x12), BIN);

  Serial.print("Precharge and Termination Control  :");
  Serial.println(llegeixRegistreDoble(0x14), BIN);

  Serial.print("Timer Control :                     ");
  Serial.println(llegeixRegistre(0x15), BIN);

  Serial.print("Three-Stage Charge Control :        ");
  Serial.println(llegeixRegistre(0x16), BIN);

  Serial.print("Charger Control :                   ");
  Serial.println(llegeixRegistre(0x17), BIN);

  Serial.print("Pin Control :                       ");
  Serial.println(llegeixRegistre(0x18), BIN);

  Serial.print("Power Path and Reverse Mode Control:");
  Serial.println(llegeixRegistre(0x19), BIN);

  Serial.print("MPPT Control :                      ");
  Serial.println(llegeixRegistre(0x1A), BIN);

  Serial.print("Charger Status 1 :                  ");
  Serial.println(llegeixRegistre(0x21), BIN);

  Serial.print("Charger Status 2 :                  ");
  Serial.println(llegeixRegistre(0x22), BIN);

  Serial.print("Charger Status 3 :                  ");
  Serial.println(llegeixRegistre(0x23), BIN);

  Serial.print("Fault Status :                      ");
  Serial.println(llegeixRegistre(0x24), BIN);

  Serial.print("Charger Flag 1 :                    ");
  Serial.println(llegeixRegistre(0x25), BIN);

  Serial.print("Charger Flag 2 :                    ");
  Serial.println(llegeixRegistre(0x26), BIN);

  Serial.print("Fault Flag :                        ");
  Serial.println(llegeixRegistre(0x27), BIN);

  Serial.print("Fault Mask :                        ");
  Serial.println(llegeixRegistre(0x2A), BIN);

  Serial.print("ADC Control :                       ");
  Serial.println(llegeixRegistre(0x2B), BIN);

  Serial.print("ADC Channel Control  :              ");
  Serial.println(llegeixRegistre(0x2C), BIN);

  Serial.print("IAC ADC :                           ");
  Serial.println(llegeixRegistreDoble(0x2D), BIN);

  Serial.print("IBAT ADC :                          ");
  Serial.println(llegeixRegistreDoble(0x2F), BIN);

  Serial.print("VAC ADC :                           ");
  Serial.println(llegeixRegistreDoble(0x31), BIN);
  
  Serial.print("VBAT ADC :                          ");
  Serial.println(llegeixRegistreDoble(0x33), BIN);
  
  Serial.print("TS ADC :                            ");
  Serial.println(llegeixRegistreDoble(0x37), BIN);
  
  Serial.print("VFB ADC :                           ");
  Serial.println(llegeixRegistreDoble(0x39), BIN);
  
  Serial.print("Gate Driver Strength Control :      ");
  Serial.println(llegeixRegistre(0x3B), BIN);
  
  Serial.print("Part Information :                  ");
  Serial.println(llegeixRegistre(0x3D), BIN);
  
  Serial.println("========================================\n");
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(1000);
  
  pinMode(11, OUTPUT);    // pin /ChargeEnable
  pinMode(A2, INPUT);     // pin Feedbac

  Serial.println("Iniciant configuració...");

  // Desactiva watchdog
  setBitRegistre(0x15, 4, false);
  setBitRegistre(0x15, 5, false);
  delay(10);

  // Activa càrrega
  setBitRegistre(0x17, 0, true);    // EN_CHG
  delay(10);
  
  // Activa ADC
  setBitRegistre(0x2B, 7, true);    // ADC_EN
  delay(10);

  // Activa tots els canals ADC (desactiva tots els bits _DIS)
  setBitRegistre(0x2C, 7, false);   // IAC_ADC_DIS = 0
  setBitRegistre(0x2C, 6, false);   // IBAT_ADC_DIS = 0
  setBitRegistre(0x2C, 5, false);   // VAC_ADC_DIS = 0
  setBitRegistre(0x2C, 4, false);   // VBAT_ADC_DIS = 0
  setBitRegistre(0x2C, 1, false);   // TS_ADC_DIS = 0
  setBitRegistre(0x2C, 0, false);   // VFB_ADC_DIS = 0
  delay(10);

  Serial.println("Configuració completa!");
  
  // Verifica configuració
  Serial.print("ADC Control (0x2B): 0b");
  Serial.println(llegeixRegistre(0x2B), BIN);
  Serial.print("ADC Channel (0x2C): 0b");
  Serial.println(llegeixRegistre(0x2C), BIN);
  
  Serial.println("\nComandes disponibles:");
  Serial.println("  0 - Pin CE = LOW/activació");
  Serial.println("  1 - Pin CE = HIGH/desactivació");
  Serial.println("  3 - Llegir tots els registres");
  Serial.println("  4 - Llegir tots els ADC");
  Serial.println("  5 - Configurar càrrega bateria AGM 12V/7.2Ah");
  Serial.println("  6 - Llegir STATUS i ERRORS");
}

void loop() {
  if (Serial.available() > 0) {
    char dada = Serial.read();
    if (dada == '0') {
      digitalWrite(11, LOW);
      Serial.println("Pin CE = LOW");
    }
    if (dada == '5') {
      Serial.println("\n========== CONFIGURACIÓ CÀRREGA BATERIA AGM 12V/7.2Ah ==========");
      
      // Desactivem els pins de control per utilitzar I2C
      setBitRegistre(0x18, 7, false);   // EN_ICHG_PIN = 0 (disable)
      setBitRegistre(0x18, 6, false);   // EN_ILIM_HIZ_PIN = 0 (disable)
      delay(10);

      // 1. CHARGE VOLTAGE LIMIT (Reg 0x00-0x01)
      // AGM Cycle use: 14.6-14.8V → Usem 14.7V
      // VFB_REG controla el voltatge de FB pin
      // Per 14.7V amb divisor de 249k/24.88k: necessitem VFB ≈ 1.536V (per defecte)
      // Deixem VFB_REG per defecte (el divisor resistiu dona 14.7V)
      // Registre 0x00: bits [4:0], Offset: 1504mV, Step: 2mV
      // Per 1.536V: (1536-1504)/2 = 16 = 0x10
      uint16_t charge_voltage = 16;  // 1536mV al pin FB
      escriuRegistre16(0x00, charge_voltage);
      Serial.print("FB Voltage: 1.536V → Battery: 14.7V (0x");
      Serial.print(charge_voltage, HEX);
      Serial.println(")");
      
      // 2. CHARGE CURRENT LIMIT (Reg 0x02-0x03)
      // AGM 7.2Ah: màxim 2A (C/3.6)
      // Format: bits [10:2], Offset: bits×50mA amb RBAT_SNS=5mΩ
      // Per 2A: 2000mA/50mA = 40, shift 2 bits: 40<<2 = 160 = 0xA0
      uint16_t charge_current = (4 << 2);  // 2000mA
      escriuRegistre16(0x02, charge_current);
      Serial.print("Charge Current: 2.0A (0x");
      Serial.print(charge_current, HEX);
      Serial.println(")");
      
      // 3. INPUT CURRENT LIMIT (Reg 0x06-0x07)
      // Amb Vin=15V, per ~30W: 3A és suficient
      // Format: bits [10:2], igual que charge current
      // Per 3A: 3000mA/50mA = 60, shift: 60<<2 = 240 = 0xF0
      uint16_t input_current = (10 << 2);  // 3000mA
      escriuRegistre16(0x06, input_current);
      Serial.print("Input Current Limit: 3.0A (0x");
      Serial.print(input_current, HEX);
      Serial.println(")");
      
      // 4. INPUT VOLTAGE LIMIT (Reg 0x08-0x09) 
      // Per Vin=15V, configurem DPM a 13V per seguretat
      // Format: bits [13:2], Offset: LSB=20mV
      // Per 13V: 13000mV-4100mV/20mV = 440, shift: 440<<2 = 1760 = 0x06E0
      uint16_t input_voltage = (300 << 2);  // 13000mV
      escriuRegistre16(0x08, input_voltage);
      Serial.print("Input Voltage DPM: 13.0V (0x");
      Serial.print(input_voltage, HEX);
      Serial.println(")");
      
      // 5. PRECHARGE CURRENT (Reg 0x10-0x11)
      // 10-20% del charge current: 400mA
      // Per 400mA: 400-250/50 = 3, shift: 3<<2 = 12 = 0x0C
      uint16_t precharge_current = (3 << 2);  // 400mA
      escriuRegistre16(0x10, precharge_current);
      Serial.print("Precharge Current: 0.4A (0x");
      Serial.print(precharge_current, HEX);
      Serial.println(")");
      
      // 6. TERMINATION CURRENT (Reg 0x12-0x13)
      // 5-10% del charge current: 150mA
      // Per 250mA(min): 150/50 = 3, shift: 3<<2 = 12 = 0x0C
      uint16_t term_current = (0 << 2);  // 150mA
      escriuRegistre16(0x12, term_current);
      Serial.print("Termination Current: 0.15A (0x");
      Serial.print(term_current, HEX);
      Serial.println(")");
      
      // 7. PRECHARGE/TERMINATION CONTROL (Reg 0x14)
      // Habilitem precharge i termination
      uint8_t reg14 = llegeixRegistre(0x14);
      reg14 |= 0x09;  // EN_TERM=1 (bit3), EN_PRECHG=1 (bit0)
      escriuRegistre8(0x14, reg14);
      Serial.println("Precharge & Termination: ENABLED");
      
      // 8. TIMER CONTROL (Reg 0x15)
      uint8_t reg15 = llegeixRegistre(0x15);
      reg15 &= ~(0x30);  // Clear bits [5:4] (WATCHDOG)
      reg15 |= 0x0D;     // WATCHDOG=00 (disabled), EN_CHG_TMR=1, CHG_TMR=10 (12hr), EN_TMR2X=1
      escriuRegistre8(0x15, reg15);
      Serial.println("Watchdog: Disabled, Safety Timer: 12hr");
      
      // 9. CHARGER CONTROL (Reg 0x17)
      // Habilitem càrrega (però CE pin encara HIGH)
      uint8_t reg17 = llegeixRegistre(0x17);
      reg17 |= 0x21;   // EN_CHG=1 (bit0), WD_RST=1 (bit5)
      escriuRegistre8(0x17, reg17);
      Serial.println("Charging Control: ENABLED (awaiting CE pin LOW)");
      
      // 10. ADC CONTROL (Reg 0x2B)
      setBitRegistre(0x2B, 7, true);   // ADC_EN = 1
      Serial.println("ADC: ENABLED");
      
      delay(100);
      
      // Verificació detallada
      Serial.println("\n========== VERIFICACIÓ REGISTRES ==========");
      uint16_t val;
      
      val = llegeixRegistreDoble(0x00);
      Serial.print("REG 0x00 (FB Voltage): 0x");
      Serial.print(val, HEX);
      Serial.print(" → ");
      Serial.print((val & 0x1F) * 2 + 1504);
      Serial.println(" mV");
      
      val = llegeixRegistreDoble(0x02);
      Serial.print("REG 0x02 (Charge Current): 0x");
      Serial.print(val, HEX);
      Serial.print(" → ");
      Serial.print(((val >> 2) & 0x1FF) * 50);
      Serial.println(" mA");
      
      val = llegeixRegistreDoble(0x06);
      Serial.print("REG 0x06 (Input Current): 0x");
      Serial.print(val, HEX);
      Serial.print(" → ");
      Serial.print(((val >> 2) & 0x1FF) * 50);
      Serial.println(" mA");
      
      val = llegeixRegistreDoble(0x08);
      Serial.print("REG 0x08 (Input Voltage DPM): 0x");
      Serial.print(val, HEX);
      Serial.print(" → ");
      Serial.print(((val >> 2) & 0xFFF) * 20);
      Serial.println(" mV");
      
      Serial.print("REG 0x14: 0b");
      Serial.println(llegeixRegistre(0x14), BIN);
      Serial.print("REG 0x15: 0b");
      Serial.println(llegeixRegistre(0x15), BIN);
      Serial.print("REG 0x17: 0b");
      Serial.println(llegeixRegistre(0x17), BIN);
      
      Serial.println("\n✓✓✓ CONFIGURACIÓ COMPLETADA ✓✓✓");
      Serial.println("Paràmetres AGM 12V/7.2Ah:");
      Serial.println("  • Voltatge càrrega: 14.7V");
      Serial.println("  • Corrent càrrega: 2.0A");
      Serial.println("  • Input limit: 3.0A @ 13V DPM");
      Serial.println("  • Precharge: 400mA");
      Serial.println("  • Termination: 150mA");
      Serial.println("\n⚠ Ara prem '0' per activar CE (pin 11 LOW)");
      Serial.println("============================================\n");
    }
    // if (dada == '5') {
    //   Serial.println("\n========== CONFIGURACIÓ CÀRREGA BATERIA AGM 12V/7.2Ah ==========");
      
    //   setBitRegistre(0x18, 7, false);   // EN_ICHG_PIN
    //   setBitRegistre(0x18, 6, false);   // EN_ILIM_HIZ_PIN


    //   // 1. CHARGE VOLTAGE LIMIT (Reg 0x00-0x01)
    //   // Cycle use: 14.6-14.8V → Usem 14.7V
    //   // Format: 16-bit, LSB = 10mV, Range: 1-19200mV
    //   uint16_t charge_voltage = 0x0010;//14700 / 10;  // 14700mV / 10mV = 1470 (0x05BE)
    //   escriuRegistre16(0x00, charge_voltage);
    //   Serial.print("Charge Voltage: 14.7V (");
    //   Serial.print(charge_voltage);
    //   Serial.println(")");
      
    //   // 2. CHARGE CURRENT LIMIT (Reg 0x02-0x03)
    //   // Initial current < 2.16A → Usem 2.0A per seguretat
    //   // Format: 16-bit, LSB = 50mA, Range: 50-5000mA
    //   uint16_t charge_current = 0x0080;//2000 / 50;  // 2000mA / 50mA = 40 (0x0028)
    //   escriuRegistre16(0x02, charge_current);
    //   Serial.print("Charge Current: 2.0A (");
    //   Serial.print(charge_current);
    //   Serial.println(")");
      
    //   // 3. INPUT CURRENT LIMIT (Reg 0x06-0x07)
    //   // Limitem a 3A per protegir la font d'alimentació
    //   uint16_t input_current = 0x00D0;//3000 / 50;  // 3000mA / 50mA = 60 (0x003C)
    //   escriuRegistre16(0x06, input_current);
    //   Serial.print("Input Current Limit: 3.0A (");
    //   Serial.print(input_current);
    //   Serial.println(")");
      
    //   // 4. INPUT VOLTAGE LIMIT (Reg 0x08-0x09)
    //   // Mínim 14V per tenir marge sobre el voltatge de càrrega
    //   uint16_t input_voltage = 0x0230;//14000 / 100;  // 14000mV / 100mV = 140 (0x008C)
    //   escriuRegistre16(0x08, input_voltage);
    //   Serial.print("Input Voltage Limit: 14.0V (");
    //   Serial.print(input_voltage);
    //   Serial.println(")");
      
    //   // 5. PRECHARGE CURRENT (Reg 0x10-0x11)
    //   // 10% del corrent de càrrega = 200mA
    //   uint16_t precharge_current = 0;//200 / 50;  // 200mA / 50mA = 4 (0x0004)
    //   escriuRegistre16(0x10, precharge_current);
    //   Serial.print("Precharge Current: 0.2A (");
    //   Serial.print(precharge_current);
    //   Serial.println(")");
      
    //   // 6. TERMINATION CURRENT (Reg 0x12-0x13)
    //   // 5% del corrent de càrrega = 100mA
    //   uint16_t term_current = 0;//100 / 50;  // 100mA / 50mA = 2 (0x0002)
    //   escriuRegistre16(0x12, term_current);
    //   Serial.print("Termination Current: 0.1A (");
    //   Serial.print(term_current);
    //   Serial.println(")");
      
    //   // 7. TIMER CONTROL (Reg 0x15)
    //   // Desactivem watchdog i timers de seguretat (ja ho teníem)
    //   setBitRegistre(0x15, 5, false);  // WD_DIS = 1 (disable watchdog)
    //   setBitRegistre(0x15, 4, false);  // TMR2X_EN = 0 (timer normal)
    //   Serial.println("Watchdog: Disabled");
      
    //   // 8. CHARGER CONTROL (Reg 0x17)
    //   setBitRegistre(0x17, 0, true);   // EN_CHG = 1 (enable charging)
    //   setBitRegistre(0x17, 5, true);   // WD_RST (reset watchdog)
    //   Serial.println("Charging: ENABLED");
      
    //   // 9. ADC CONTROL (Reg 0x2B)
    //   setBitRegistre(0x2B, 7, true);   // ADC_EN = 1
    //   Serial.println("ADC: Enabled");
      
    //   delay(100);
      
    //   // Verificació
    //   Serial.println("\n--- VERIFICACIÓ ---");
    //   Serial.print("Charge Voltage Reg: 0x");
    //   Serial.println(llegeixRegistreDoble(0x00), HEX);
    //   Serial.print("Charge Current Reg: 0x");
    //   Serial.println(llegeixRegistreDoble(0x02), HEX);
    //   Serial.print("Input Current DPM Limit: 0x");
    //   Serial.println(llegeixRegistreDoble(0x06), HEX);
    //   Serial.print("Input Voltage DPM Limit: 0x");
    //   Serial.println(llegeixRegistreDoble(0x08), HEX);
    //   Serial.print("Charger Control: 0b");
    //   Serial.println(llegeixRegistre(0x17), BIN);
      
    //   Serial.println("\n✓ Configuració completada per AGM 12V/7.2Ah");
    //   Serial.println("  Voltatge: 14.7V | Corrent: 2.0A");
    //   Serial.println("================================================================\n");
    // }

    if (dada == '1') {
      digitalWrite(11, HIGH);
      Serial.println("Pin CE = HIGH");
    }

    if (dada == '2') {
        // Desactiva watchdog
      setBitRegistre(0x15, 4, false);
      setBitRegistre(0x15, 5, false);
      delay(10);

      // Activa càrrega
      setBitRegistre(0x17, 0, true);    // EN_CHG
      delay(10);
      
      // Activa ADC
      setBitRegistre(0x2B, 7, true);    // ADC_EN
      delay(10);

      // Activa tots els canals ADC (desactiva tots els bits _DIS)
      setBitRegistre(0x2C, 7, false);   // IAC_ADC_DIS = 0
      setBitRegistre(0x2C, 6, false);   // IBAT_ADC_DIS = 0
      setBitRegistre(0x2C, 5, false);   // VAC_ADC_DIS = 0
      setBitRegistre(0x2C, 4, false);   // VBAT_ADC_DIS = 0
      setBitRegistre(0x2C, 1, false);   // TS_ADC_DIS = 0
      setBitRegistre(0x2C, 0, false);   // VFB_ADC_DIS = 0
      delay(10);

      Serial.println("Configuració completa!");
      
      // Verifica configuració
      Serial.print("ADC Control (0x2B): 0b");
      Serial.println(llegeixRegistre(0x2B), BIN);
      Serial.print("ADC Channel (0x2C): 0b");
      Serial.println(llegeixRegistre(0x2C), BIN);
    }

    if (dada == '3') {
      llegeixTotsElsRegistres();
    }

    if (dada == '4') {
      Serial.println("\n========== LECTURA TOTS ELS ADC ==========");
      
      // Força una conversió ADC
      setBitRegistre(0x2B, 6, true);    // ADC_CONV = 1 (one-shot)
      delay(50);  // Espera conversió
      
      // IAC - Input Current
      uint16_t iac_raw = llegeixRegistreDoble(0x2D);
      Serial.print("IAC  (Input Current)  : ");
      Serial.print(iac_raw);
      Serial.print(" (0x");
      Serial.print(iac_raw, HEX);
      Serial.print(") = ");
      // LSB = 50mA segons datasheet típic (verifica!)
      float iac_ma = iac_raw * 50.0;
      Serial.print(iac_ma);
      Serial.println(" mA");
      
      // IBAT - Battery Current
      uint16_t ibat_raw = llegeixRegistreDoble(0x2F);
      Serial.print("IBAT (Battery Current): ");
      Serial.print(ibat_raw);
      Serial.print(" (0x");
      Serial.print(ibat_raw, HEX);
      Serial.print(") = ");
      // LSB = 50mA segons datasheet típic (verifica!)
      float ibat_ma = ibat_raw * 50.0;
      Serial.print(ibat_ma);
      Serial.println(" mA");
      
      // VAC - Input Voltage
      uint16_t vac_raw = llegeixRegistreDoble(0x31);
      Serial.print("VAC  (Input Voltage)  : ");
      Serial.print(vac_raw);
      Serial.print(" (0x");
      Serial.print(vac_raw, HEX);
      Serial.print(") = ");
      // LSB = 2mV segons datasheet (Format: 2s Complement, Range: 0-65534mV)
      float vac_mv = vac_raw * 2.0;
      Serial.print(vac_mv);
      Serial.print(" mV = ");
      Serial.print(vac_mv / 1000.0, 2);
      Serial.println(" V");
      
      // VBAT - Battery Voltage
      uint16_t vbat_raw = llegeixRegistreDoble(0x33);
      Serial.print("VBAT (Battery Voltage): ");
      Serial.print(vbat_raw);
      Serial.print(" (0x");
      Serial.print(vbat_raw, HEX);
      Serial.print(") = ");
      // LSB = 10mV segons datasheet típic (verifica!)
      float vbat_mv = vbat_raw * 2.0;
      Serial.print(vbat_mv);
      Serial.println(" mV = ");
      Serial.print(vbat_mv / 1000.0, 2);
      Serial.println(" V");
      
      // TS - Temperature Sensor
      uint16_t ts_raw = llegeixRegistreDoble(0x37);
      Serial.print("TS   (Temperature)    : ");
      Serial.print(ts_raw);
      Serial.print(" (0x");
      Serial.print(ts_raw, HEX);
      Serial.println(") [raw]");
      
      // VFB - Feedback Voltage
      uint16_t vfb_raw = llegeixRegistreDoble(0x39);
      Serial.print("VFB  (Feedback)       : ");
      Serial.print(vfb_raw);
      Serial.print(" (0x");
      Serial.print(vfb_raw, HEX);
      Serial.print(") = ");
      // LSB = 10mV segons datasheet típic (verifica!)
      float vfb_mv = vfb_raw * 1.0;
      Serial.print(vfb_mv);
      Serial.println(" mV = ");
      Serial.print(vfb_mv / 1000.0, 2);
      Serial.println(" V");
      
      Serial.println("==========================================\n");
    }
  

    if (dada == '6') {
      Serial.println("\n========== REGISTRES DE STATUS I ERRORS ==========");
      
      // CHARGER STATUS 1 (0x21)
      uint8_t status1 = llegeixRegistre(0x21);
      Serial.print("Charger Status 1 (0x21): 0b");
      Serial.println(status1, BIN);
      Serial.print("  [7] ADC_DONE_STAT: ");
      Serial.println((status1 & 0x80) ? "Conversion complete" : "Not complete");
      Serial.print("  [6] IAC_DPM_STAT: ");
      Serial.println((status1 & 0x40) ? "In Input Current regulation" : "Normal");
      Serial.print("  [5] VAC_DPM_STAT: ");
      Serial.println((status1 & 0x20) ? "In Input Voltage regulation" : "Normal");
      Serial.print("  [3] WD_STAT: ");
      Serial.println((status1 & 0x08) ? "WD timer expired" : "Normal");
      Serial.print("  [2:0] CHARGE_STAT: ");
      uint8_t chg_stat = status1 & 0x07;
      switch(chg_stat) {
        case 0b000: Serial.println("Not Charging"); break;
        case 0b001: Serial.println("Trickle Charge (VBAT < VBAT_SHORT)"); break;
        case 0b010: Serial.println("Pre-Charge (VBAT < VBAT_LOWV)"); break;
        case 0b011: Serial.println("Fast Charge (CC mode)"); break;
        case 0b100: Serial.println("Taper Charge (CV mode)"); break;
        case 0b101: Serial.println("Reserved"); break;
        case 0b110: Serial.println("Top-off Timer Charge"); break;
        case 0b111: Serial.println("Charge Termination Done"); break;
      }
      
      // CHARGER STATUS 2 (0x22)
      uint8_t status2 = llegeixRegistre(0x22);
      Serial.print("\nCharger Status 2 (0x22): 0b");
      Serial.println(status2, BIN);
      Serial.print("  [7] PG_STAT: ");
      Serial.println((status2 & 0x80) ? "Power Good" : "Not Power Good");
      Serial.print("  [6:4] TS_STAT: ");
      uint8_t ts_stat = (status2 >> 4) & 0x07;
      switch(ts_stat) {
        case 0b000: Serial.println("Normal"); break;
        case 0b001: Serial.println("TS Warm"); break;
        case 0b010: Serial.println("TS Cool"); break;
        case 0b011: Serial.println("TS Cold"); break;
        case 0b100: Serial.println("TS Hot"); break;
        default: Serial.println("Unknown"); break;
      }
      Serial.print("  [1:0] MPPT_STAT: ");
      uint8_t mppt_stat = status2 & 0x03;
      switch(mppt_stat) {
        case 0b00: Serial.println("MPPT Disabled"); break;
        case 0b01: Serial.println("MPPT Enabled, But Not Running"); break;
        case 0b10: Serial.println("Full Panel Sweep In Progress"); break;
        case 0b11: Serial.println("Max Power Voltage Detected"); break;
      }
      
      // CHARGER STATUS 3 (0x23)
      uint8_t status3 = llegeixRegistre(0x23);
      Serial.print("\nCharger Status 3 (0x23): 0b");
      Serial.println(status3, BIN);
      Serial.print("  [5:4] FSW_SYNC_STAT: ");
      uint8_t fsw_stat = (status3 >> 4) & 0x03;
      switch(fsw_stat) {
        case 0b00: Serial.println("Normal, no external clock"); break;
        case 0b01: Serial.println("Valid ext. clock detected"); break;
        case 0b10: Serial.println("Pin fault (freq out-of-range)"); break;
        case 0b11: Serial.println("Reserved"); break;
      }
      Serial.print("  [3] CV_TMR_STAT: ");
      Serial.println((status3 & 0x08) ? "CV Timer Expired" : "Normal");
      Serial.print("  [2] REVERSE_STAT: ");
      Serial.println((status3 & 0x04) ? "Reverse Mode ON" : "Reverse Mode OFF");
      
      // FAULT STATUS (0x24)
      uint8_t fault = llegeixRegistre(0x24);
      Serial.print("\nFault Status (0x24): 0b");
      Serial.println(fault, BIN);
      if (fault == 0) {
        Serial.println("  ✓ NO FAULTS");
      } else {
        Serial.println("  ⚠ FAULTS DETECTED:");
        if (fault & 0x80) Serial.println("    [7] VAC_UV_STAT: Input under-voltage");
        if (fault & 0x40) Serial.println("    [6] VAC_OV_STAT: Input over-voltage");
        if (fault & 0x20) Serial.println("    [5] IBAT_OCP_STAT: Battery over-current");
        if (fault & 0x10) Serial.println("    [4] VBAT_OV_STAT: Battery over-voltage");
        if (fault & 0x08) Serial.println("    [3] TSHUT_STAT: Thermal shutdown");
        if (fault & 0x04) Serial.println("    [2] CHG_TMR_STAT: Charge safety timer expired");
        if (fault & 0x02) Serial.println("    [1] DRV_OKZ_STAT: DRV_SUP pin out of range");
      }
      
      // CHARGER FLAG 1 (0x25) - Clear on read
      uint8_t flag1 = llegeixRegistre(0x25);
      Serial.print("\nCharger Flag 1 (0x25): 0b");
      Serial.println(flag1, BIN);
      if (flag1 != 0) {
        Serial.println("  Flags (Clear on Read):");
        if (flag1 & 0x80) Serial.println("    [7] ADC_DONE_FLAG: ADC conversion INT");
        if (flag1 & 0x40) Serial.println("    [6] IAC_DPM_FLAG: Entered Input Current regulation");
        if (flag1 & 0x20) Serial.println("    [5] VAC_DPM_FLAG: Entered Input Voltage regulation");
        if (flag1 & 0x08) Serial.println("    [3] WD_FLAG: WD_STAT rising edge");
        if (flag1 & 0x02) Serial.println("    [1] CV_TMR_FLAG: CV timer expired rising edge");
        if (flag1 & 0x01) Serial.println("    [0] CHARGE_FLAG: CHARGE_STAT changed");
      }
      
      // CHARGER FLAG 2 (0x26) - Clear on read
      uint8_t flag2 = llegeixRegistre(0x26);
      Serial.print("Charger Flag 2 (0x26): 0b");
      Serial.println(flag2, BIN);
      if (flag2 != 0) {
        Serial.println("  Flags (Clear on Read):");
        if (flag2 & 0x80) Serial.println("    [7] PG_FLAG: PG signal toggle");
        if (flag2 & 0x10) Serial.println("    [4] TS_FLAG: TS_STAT changed");
        if (flag2 & 0x08) Serial.println("    [3] REVERSE_FLAG: Reverse Mode toggle");
        if (flag2 & 0x02) Serial.println("    [1] FSW_SYNC_FLAG: FSW_SYNC status changed");
        if (flag2 & 0x01) Serial.println("    [0] MPPT_FLAG: MPPT_STAT changed");
      }
      
      // FAULT FLAG (0x27) - Clear on read
      uint8_t fault_flag = llegeixRegistre(0x27);
      Serial.print("Fault Flag (0x27): 0b");
      Serial.println(fault_flag, BIN);
      if (fault_flag != 0) {
        Serial.println("  ⚠ Fault Flags (Clear on Read):");
        if (fault_flag & 0x80) Serial.println("    [7] VAC_UV_FLAG: Input under-voltage fault");
        if (fault_flag & 0x40) Serial.println("    [6] VAC_OV_FLAG: Input over-voltage fault");
        if (fault_flag & 0x20) Serial.println("    [5] IBAT_OCP_FLAG: Battery over-current fault");
        if (fault_flag & 0x10) Serial.println("    [4] VBAT_OV_FLAG: Battery over-voltage fault");
        if (fault_flag & 0x08) Serial.println("    [3] TSHUT_FLAG: Thermal shutdown fault");
        if (fault_flag & 0x04) Serial.println("    [2] CHG_TMR_FLAG: Safety timer expired");
        if (fault_flag & 0x02) Serial.println("    [1] DRV_OKZ_FLAG: DRV_SUP pin fault");
      }
      
      Serial.println("==================================================\n");
    }
  }

  // Watchdog reset periòdic
  static unsigned long previousTime = 0;
  if (millis() - previousTime > 500) {
    setBitRegistre(0x17, 5, true);    // WD_RST
    previousTime = millis();
  }
}