// llibreries trovades
// https://github.com/theohg/bq25756e_multiplatform
// https://github.com/jacky4566/BQ25756_Driver_CPP/





// FUNCIONS CORREGIDES PER BQ25756

#include <Wire.h>
#include "registers_BQ25756.h"
#include "configuracio.h"




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
  uint8_t low_byte = Wire.read();
  uint8_t high_byte = Wire.read();

  return ((uint16_t)high_byte << 8) | low_byte;
}

bool llegeixBit(uint8_t reg, uint8_t bit){
  uint8_t valor_reg = llegeixRegistre(reg);
  if(bit > 7) return false;              // protecció contra bit fora de rang
  return (valor_reg & (1 << bit)) != 0;  // retorna true si el bit està a 1, false si és 0
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
  Serial.println(llegeixRegistreDoble(REG_CHARGE_VOLTAGE_LIMIT), BIN);

  Serial.print("Charge Current Limit :              ");
  Serial.println(llegeixRegistreDoble(REG_CHARGE_CURRENT_LIMIT), BIN);

  Serial.print("Input Current DPM Limit :           ");
  Serial.println(llegeixRegistreDoble(REG_INPUT_CURRENT_DPM_LIMIT), BIN);

  Serial.print("Input Voltage DPM Limit :           ");
  Serial.println(llegeixRegistreDoble(REG_INPUT_VOLTAGE_DPM_LIMIT), BIN);

  Serial.print("Reverse Mode Input Current Limit :  ");
  Serial.println(llegeixRegistreDoble(REG_REVERSE_MODE_INPUT_CURRENT_LIMIT), BIN);

  Serial.print("Reverse Mode Input Voltage Limit :  ");
  Serial.println(llegeixRegistreDoble(REG_REVERSE_MODE_INPUT_VOLTAGE_LIMIT), BIN);

  Serial.print("Precharge Current Limit :           ");
  Serial.println(llegeixRegistreDoble(REG_PRECHARGE_CURRENT_LIMIT), BIN);

  Serial.print("Termination Current Limit :         ");
  Serial.println(llegeixRegistreDoble(REG_TERMINATION_CURRENT_LIMIT), BIN);

  Serial.print("Precharge and Termination Control  :");
  Serial.println(llegeixRegistreDoble(REG_PRECHARGE_AND_TERMINATION_CONTROL), BIN);

  Serial.print("Timer Control :                     ");
  Serial.println(llegeixRegistre(REG_TIMER_CONTROL), BIN);

  Serial.print("Three-Stage Charge Control :        ");
  Serial.println(llegeixRegistre(REG_THREE_STAGE_CHARGE_CONTROL), BIN);

  Serial.print("Charger Control :                   ");
  Serial.println(llegeixRegistre(REG_CHARGER_CONTROL), BIN);

  Serial.print("Pin Control :                       ");
  Serial.println(llegeixRegistre(REG_PIN_CONTROL), BIN);

  Serial.print("Power Path and Reverse Mode Control:");
  Serial.println(llegeixRegistre(REG_POWER_PATH_AND_REVERSE_MODE_CONTROL), BIN);

  Serial.print("MPPT Control :                      ");
  Serial.println(llegeixRegistre(REG_MPPT_CONTROL), BIN);



  Serial.print("Charger Status 1 :                  ");
  Serial.println(llegeixRegistre(REG_CHARGER_STATUS_1), BIN);

  Serial.print("Charger Status 2 :                  ");
  Serial.println(llegeixRegistre(REG_CHARGER_STATUS_2), BIN);

  Serial.print("Charger Status 3 :                  ");
  Serial.println(llegeixRegistre(REG_CHARGER_STATUS_3), BIN);

  Serial.print("Fault Status :                      ");
  Serial.println(llegeixRegistre(REG_FAULT_STATUS), BIN);

  Serial.print("Charger Flag 1 :                    ");
  Serial.println(llegeixRegistre(REG_CHARGER_FLAG_1), BIN);

  Serial.print("Charger Flag 2 :                    ");
  Serial.println(llegeixRegistre(REG_CHARGER_FLAG_2), BIN);

  Serial.print("Fault Flag :                        ");
  Serial.println(llegeixRegistre(REG_FAULT_FLAG), BIN);



  Serial.print("Fault Mask :                        ");
  Serial.println(llegeixRegistre(REG_FAULT_MASK), BIN);

  Serial.print("ADC Control :                       ");
  Serial.println(llegeixRegistre(REG_ADC_CONTROL), BIN);

  Serial.print("ADC Channel Control  :              ");
  Serial.println(llegeixRegistre(REG_ADC_CHANNEL_CONTROL), BIN);

  


  Serial.print("Gate Driver Strength Control :      ");
  Serial.println(llegeixRegistre(0x3B), BIN);
  
  Serial.print("Part Information :                  ");
  Serial.println(llegeixRegistre(0x3D), BIN);
  
  Serial.println("========================================\n");
}

void configuracioCarregaBateria(){
  Serial.println("\n========== CONFIGURACIÓ CÀRREGA BATERIA AGM 12V/7.2Ah ==========");

  // Desactivem els pins de control per utilitzar I2C
  setBitRegistre(REG_PIN_CONTROL, 7, false);   // EN_ICHG_PIN = 0 (disable)
  setBitRegistre(REG_PIN_CONTROL, 6, false);   // EN_ILIM_HIZ_PIN = 0 (disable)
  delay(10);

  // 1. CHARGE VOLTAGE LIMIT (Reg 0x00-0x01)
  float Vfeedback  = Vcharge*(Rbot + Rfbg)/(Rtop + Rbot + Rfbg);
  int charge_voltage_steps = (int) round((Vfeedback - 1.504) * 1000.0 / 2.0);   // Vfeedback min = 0v --> step 0
  escriuRegistre16(REG_CHARGE_VOLTAGE_LIMIT, charge_voltage_steps);
  Serial.print("Charge Voltage:           "); Serial.print(Vcharge, 2); Serial.print("V    [steps: "); Serial.print(charge_voltage_steps); Serial.print(", Vfb: "); Serial.print(Vfeedback, 3); Serial.println("v]");

  // 2. CHARGE CURRENT LIMIT (Reg 0x02-0x03)
  int charge_current_steps = (int) round(Icharge/0.05);                 // Icharge min = 400mA --> step 8
  escriuRegistre16(REG_CHARGE_CURRENT_LIMIT, (charge_current_steps << 2));
  Serial.print("Charge Current:           "); Serial.print(Icharge); Serial.print(" A    [steps: "); Serial.print(charge_current_steps); Serial.println("]");

  // 3. INPUT CURRENT LIMIT (Reg 0x06-0x07)
  int input_current_steps = (int) round(Iin_lim/0.05);           // Iin_lim min = 400mA --> step 5
  escriuRegistre16(REG_INPUT_CURRENT_DPM_LIMIT, (input_current_steps << 2));
  Serial.print("Input Current:            "); Serial.print(Iin_lim); Serial.print(" A    [steps: "); Serial.print(input_current_steps); Serial.println("]");

  // 4. INPUT VOLTAGE LIMIT (Reg 0x08-0x09)
  int input_voltage_steps = (int) round(Vin_lim/0.02);                    // Vin_lim min = 4.2v --> step 210
  escriuRegistre16(REG_INPUT_VOLTAGE_DPM_LIMIT, (input_voltage_steps << 2));
  Serial.print("Input Voltage:            "); Serial.print(Vin_lim); Serial.print(" V   [steps: "); Serial.print(input_voltage_steps); Serial.println("]");

  // 5. PRECHARGE CURRENT (Reg 0x10-0x11)    >>> 10-20% del charge current
  int precharge_current_steps = (int) round(Iprecharge/0.05);         // Iprecharge min = 250mA --> step 5
  escriuRegistre16(REG_PRECHARGE_CURRENT_LIMIT, (precharge_current_steps << 2));
  Serial.print("Precharge Current:        "); Serial.print(Iprecharge); Serial.print(" A    [steps: "); Serial.print(precharge_current_steps); Serial.println("]");

  delay(50);
  Serial.println(llegeixRegistreDoble(REG_PRECHARGE_CURRENT_LIMIT)>>2);

  // 6. TERMINATION CURRENT (Reg 0x12-0x13)   >>> 5-10% del charge current
  int termination_current_steps = (int) round(ITermination/0.05);      // ITermination min = 250mA --> step 5
  escriuRegistre16(REG_TERMINATION_CURRENT_LIMIT, (termination_current_steps << 2));
  Serial.print("Termination Current:      "); Serial.print(ITermination); Serial.print(" A    [steps: "); Serial.print(termination_current_steps); Serial.println("]");

  delay(50);
  Serial.println(llegeixRegistreDoble(REG_TERMINATION_CURRENT_LIMIT)>>2);


  // 7. PRECHARGE/TERMINATION CONTROL (Reg 0x14)
  setBitRegistre(REG_PRECHARGE_AND_TERMINATION_CONTROL, 0, enable_precharge_control);
  setBitRegistre(REG_PRECHARGE_AND_TERMINATION_CONTROL, 3, enable_temination_control);

  Serial.println(enable_precharge_control ?  "Precharge:        ENABLED" : "Precharge:        DISABLED");
  Serial.println(enable_temination_control ? "Termination:      ENABLED" : "Termination:      DISABLED");


  // 8. TIMER CONTROL (Reg 0x15)
  setBitRegistre(REG_TIMER_CONTROL, 4, false);     // 1) Netejar bits 1:2
  setBitRegistre(REG_TIMER_CONTROL, 5, false);
  // reg15 &= ~0x30;     // 1) Netejar bits 5:4 (màscara 0b110000 = 0x30)
  uint8_t reg15 = llegeixRegistre(REG_TIMER_CONTROL);
  reg15 |= (WATCHDOG << 4);     // 2) Escriure el valor del watchdog (0..3) desplaçat als bits 5:4
  escriuRegistre8(REG_TIMER_CONTROL, reg15);

  switch (WATCHDOG) {
    case 0: Serial.println("Watchdog:         DISABLED"); break;
    case 1: Serial.println("Watchdog:         40s");     break;
    case 2: Serial.println("Watchdog:         80s");     break;
    case 3: Serial.println("Watchdog:         160s");    break;
  }

  // 9. CHARGER CONTROL (Reg 0x17)
  setBitRegistre(REG_CHARGER_CONTROL, 0, enable_charge);
  setBitRegistre(REG_CHARGER_CONTROL, 5, watchdog_reset);

  Serial.println(enable_charge ? "Charging Control: ENABLED" : "Charging Control: DISABLED");
  Serial.println(watchdog_reset ? "Watchdog reseted" : "");


  // 10. ADC CONTROL (Reg 0x2B)   enable_adc
  setBitRegistre(REG_ADC_CONTROL, 7, enable_adc);
  Serial.println(enable_adc ? "ADC:              ENABLED" : "ADC:              DISABLED");


  // 11. BATTERY THRESHOLD (Reg 0x14)
  setBitRegistre(REG_PRECHARGE_AND_TERMINATION_CONTROL, 1, false);     // 1) Netejar bits 1:2
  setBitRegistre(REG_PRECHARGE_AND_TERMINATION_CONTROL, 2, false);
  uint8_t reg14 = llegeixRegistre(REG_PRECHARGE_AND_TERMINATION_CONTROL);
  reg14 |= (VBAT_LOWV << 1);           // 2) Escriure el valor del Vbat_lowv (0..3) desplaçat als bits
  escriuRegistre8(REG_PRECHARGE_AND_TERMINATION_CONTROL, reg14);

  switch (VBAT_LOWV) {
    case 0: Serial.print("Vbat_lowv:        30% x VFB_REG --> ");Serial.println(Vfeedback*0.3); break;
    case 1: Serial.print("Vbat_lowv:        55% x VFB_REG");     Serial.println(Vfeedback*0.55); break;
    case 2: Serial.print("Vbat_lowv:        66.7% x VFB_REG");   Serial.println(Vfeedback*0.667); break;
    case 3: Serial.print("Vbat_lowv:        71.4% x VFB_REG");   Serial.println(Vfeedback*0.714); break;
  }


  Serial.println("\n✓✓✓ CONFIGURACIÓ COMPLETADA ✓✓✓");
}

void configuracioInicial(){
  // Desactiva watchdog
  // setBitRegistre(0x15, 4, false);
  // setBitRegistre(0x15, 5, false);
  // delay(10);

  // Activa càrrega
  // setBitRegistre(0x17, 0, true);    // EN_CHG
  // delay(10);

  // Activa ADC
  // setBitRegistre(0x2B, 7, true);    // ADC_EN
  // delay(10);

  // Activa tots els canals ADC (desactiva tots els bits _DIS)
  setBitRegistre(REG_ADC_CHANNEL_CONTROL, 7, false);   // IAC_ADC_DIS = 0
  setBitRegistre(REG_ADC_CHANNEL_CONTROL, 6, false);   // IBAT_ADC_DIS = 0
  setBitRegistre(REG_ADC_CHANNEL_CONTROL, 5, false);   // VAC_ADC_DIS = 0
  setBitRegistre(REG_ADC_CHANNEL_CONTROL, 4, false);   // VBAT_ADC_DIS = 0
  setBitRegistre(REG_ADC_CHANNEL_CONTROL, 1, false);   // TS_ADC_DIS = 0
  setBitRegistre(REG_ADC_CHANNEL_CONTROL, 0, false);   // VFB_ADC_DIS = 0


  setBitRegistre(REG_TS_CHARGING_REGION_BEHAVIOR_CONTROL, 0, false);   // EN_TS = 0
  delay(10);

  Serial.println("Configuració completa!");

  // // Verifica configuració
  // Serial.print("ADC Control (0x2B): 0b");
  // Serial.println(llegeixRegistre(0x2B), BIN);
  // Serial.print("ADC Channel (0x2C): 0b");
  // Serial.println(llegeixRegistre(0x2C), BIN);
}

void lecturaADCs(){
  setBitRegistre(REG_ADC_CONTROL, 7, true);    // ADC_EN
  delay(50);
  setBitRegistre(REG_ADC_CONTROL, 6, true);
  delay(50);

  while(!llegeixBit(REG_CHARGER_STATUS_1, 7)) delay(10);  // ADC_DONE_STAT: Conversion complete

  int16_t iac_raw = (int16_t) llegeixRegistreDoble(REG_IAC_ADC);
  delay(50);
  int16_t ibat_raw = (int16_t) llegeixRegistreDoble(REG_IBAT_ADC);    //Quan la bateria esta desconectada, pot donar valors erronis
  delay(50);
  uint16_t vac_raw = llegeixRegistreDoble(REG_VAC_ADC);
  delay(50);
  uint16_t vbat_raw = llegeixRegistreDoble(REG_VBAT_ADC);
  delay(50);
  uint16_t ts_raw = llegeixRegistreDoble(REG_TS_ADC);
  delay(50);
  uint16_t vfb_raw = llegeixRegistreDoble(REG_VFB_ADC);
  delay(50);
  
  Serial.print("IAC: ");      Serial.print(iac_raw * 0.8f);     Serial.print(" mA | ");
  Serial.print("IBAT: ");     Serial.print(ibat_raw * 2.0f);    Serial.print(" mA | ");
  Serial.print("VAC: ");      Serial.print(vac_raw * 0.002f);   Serial.print(" V | ");
  Serial.print("VBAT: ");     Serial.print(vbat_raw * 0.002f);  Serial.print(" V | ");
  Serial.print("TS: ");       Serial.print(ts_raw);             Serial.print(" | ");
  Serial.print("VFB: ");      Serial.print(vfb_raw * 0.001);    Serial.println(" V");

}

void lecturaStatus(){
  Serial.println("\n========== REGISTRES DE STATUS I ERRORS ==========");
  
  // CHARGER STATUS 1 (0x21)
  uint8_t status1 = llegeixRegistre(REG_CHARGER_STATUS_1);
  Serial.print("Charger Status 1 (0x21): 0b");
  Serial.println(status1, BIN);

  Serial.print("  [7] ADC_DONE_STAT: ");
  Serial.println((status1 & ADC_DONE_MASK) ? "Conversion complete" : "Not complete");

  Serial.print("  [6] IAC_DPM_STAT: ");
  Serial.println((status1 & IAC_DPM_MASK) ? "In Input Current regulation" : "Normal");

  Serial.print("  [5] VAC_DPM_STAT: ");
  Serial.println((status1 & VAC_DPM_MASK) ? "In Input Voltage regulation" : "Normal");

  Serial.print("  [3] WD_STAT: ");
  Serial.println((status1 & WATCHDOG_MASK) ? "WD timer expired" : "Normal");

  Serial.print("  [2:0] CHARGE_STAT: ");
  uint8_t chg_stat = status1 & CHARGE_MASK;
  switch(chg_stat) {
    case CHARGE_STATE_NOT_CHARGING  : Serial.println("Not Charging"); break;
    case CHARGE_STATE_TRICKLE       : Serial.println("Trickle Charge (VBAT < VBAT_SHORT)"); break;
    case CHARGE_STATE_PRE_CHARGE    : Serial.println("Pre-Charge (VBAT < VBAT_LOWV)"); break;
    case CHARGE_STATE_FAST_CHARGE   : Serial.println("Fast Charge (CC mode)"); break;
    case CHARGE_STATE_TAPER_CHARGE  : Serial.println("Taper Charge (CV mode)"); break;
    case CHARGE_STATE_RESERVED      : Serial.println("Reserved"); break;
    case CHARGE_STATE_TOP_OFF       : Serial.println("Top-off Timer Charge"); break;
    case CHARGE_STATE_DONE          : Serial.println("Charge Termination Done"); break;
  }
  
  // CHARGER STATUS 2 (0x22)
  uint8_t status2 = llegeixRegistre(REG_CHARGER_STATUS_2);
  Serial.print("\nCharger Status 2 (0x22): 0b");
  Serial.println(status2, BIN);

  Serial.print("  [7] PG_STAT: ");
  Serial.println((status2 & PG_MASK) ? "Power Good" : "Not Power Good");

  Serial.print("  [6:4] TS_STAT: ");
  uint8_t ts_stat = (status2 >> 4) & TS_MASK;
  switch(ts_stat) {
    case TS_STATUS_NORMAL : Serial.println("Normal"); break;
    case TS_STATUS_WARM   : Serial.println("TS Warm"); break;
    case TS_STATUS_COOL   : Serial.println("TS Cool"); break;
    case TS_STATUS_COLD   : Serial.println("TS Cold"); break;
    case TS_STATUS_HOT    : Serial.println("TS Hot"); break;
    default: Serial.println("Unknown"); break;
  }

  Serial.print("  [1:0] MPPT_STAT: ");
  uint8_t mppt_stat = status2 & MPPT_MASK;
  switch(mppt_stat) {
    case MPPT_DISABLED  : Serial.println("MPPT Disabled"); break;
    case MPPT_STANDBY   : Serial.println("MPPT Enabled, But Not Running"); break;
    case MPPT_PANELSWEEP: Serial.println("Full Panel Sweep In Progress"); break;
    case MPPT_ENABLED   : Serial.println("Max Power Voltage Detected"); break;
  }
  
  // CHARGER STATUS 3 (0x23)
  uint8_t status3 = llegeixRegistre(REG_CHARGER_STATUS_3);
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
  uint8_t fault = llegeixRegistre(REG_FAULT_STATUS);
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
  uint8_t flag1 = llegeixRegistre(REG_CHARGER_FLAG_1);
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
  uint8_t flag2 = llegeixRegistre(REG_CHARGER_FLAG_2);
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
  uint8_t fault_flag = llegeixRegistre(REG_FAULT_FLAG);
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

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(1000);
  
  pinMode(11, OUTPUT);    // pin /ChargeEnable
  pinMode(A2, INPUT);     // pin Feedbac

  configuracioInicial();

  digitalWrite(11, HIGH);
  Serial.println("Pin CE = HIGH");

  configuracioCarregaBateria();

  // Serial.println("\nComandes disponibles:");
  // Serial.println("  0 - ON");
  // Serial.println("  1 - OFF");
  // Serial.println("  3 - Llegir registres");
  // Serial.println("  4 - Llegir ADC");
  // Serial.println("  5 - Configurar càrrega bateria AGM 12V/7.2Ah");
  // Serial.println("  6 - STATUS i ERRORS");
}

void loop() {
  if (Serial.available() > 0) {
    char dada = Serial.read();
    if (dada == '0') {
      digitalWrite(11, LOW);
      Serial.println("Pin CE = LOW");
    }
    if (dada == '1') {
      digitalWrite(11, HIGH);
      Serial.println("Pin CE = HIGH");
    }

    if (dada == '2') {
      configuracioInicial();
    }

    if (dada == '3') {
      llegeixTotsElsRegistres();
    }

    if (dada == '4') {
      lecturaADCs();
    }

    if (dada == '5') {
      configuracioCarregaBateria();
    }

    if (dada == '6') {
      lecturaStatus();
    }

    if (dada == '7') {
      setBitRegistre(REG_MPPT_CONTROL, EN_MPPT_1, enable_mppt);   // IAC_ADC_DIS = 0
    }

    if (dada == '8') {
      setBitRegistre(REG_MPPT_CONTROL, FORCE_SWEEP_1, true);   // IAC_ADC_DIS = 0
    }
  }

  // Watchdog reset periòdic
  static unsigned long previousTime = 0;
  if (millis() - previousTime > 100) {
    setBitRegistre(REG_CHARGER_CONTROL, 5, watchdog_reset);
    Serial.println(watchdog_reset ? "Watchdog reseted" : "");

    lecturaADCs();
    previousTime = millis();
  }
}