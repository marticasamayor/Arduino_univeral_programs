// bq25756_registers.h
#pragma once


#define BQ25756_I2C_ADDRESS                       0x6B


// REGISTERS

#define REG_CHARGE_VOLTAGE_LIMIT                  0x00    // Charge Voltage Limit
#define REG_CHARGE_CURRENT_LIMIT                  0x02    // Charge Current Limit
#define REG_INPUT_CURRENT_DPM_LIMIT               0x06    // Input Current DPM Limit
#define REG_INPUT_VOLTAGE_DPM_LIMIT               0x08    // Input Voltage DPM Limit
#define REG_REVERSE_MODE_INPUT_CURRENT_LIMIT      0x0A    // Reverse Mode Input Current Limit
#define REG_REVERSE_MODE_INPUT_VOLTAGE_LIMIT      0x0C    // Reverse Mode Input Voltage Limit
#define REG_PRECHARGE_CURRENT_LIMIT               0x10    // Precharge Current Limit
#define REG_TERMINATION_CURRENT_LIMIT             0x12    // Termination Current Limit
#define REG_PRECHARGE_AND_TERMINATION_CONTROL     0x14    // Precharge and Termination Control
#define REG_TIMER_CONTROL                         0x15    // Timer Control
#define REG_THREE_STAGE_CHARGE_CONTROL            0x16    // Three-Stage Charge Control
#define REG_CHARGER_CONTROL                       0x17    // Charger Control
#define REG_PIN_CONTROL                           0x18    // Pin Control
#define REG_POWER_PATH_AND_REVERSE_MODE_CONTROL   0x19    // Power Path and Reverse Mode Control
#define REG_MPPT_CONTROL                          0x1A    // MPPT Control
#define REG_TS_CHARGING_THRESHOLD_CONTROL         0x1B    // TS Charging Threshold Control
#define REG_TS_CHARGING_REGION_BEHAVIOR_CONTROL   0x1C    // TS Charging Region Behavior Control
#define REG_TS_REVERSE_MODE_THRESHOLD_CONTROL     0x1D    // TS Reverse Mode Threshold Control
#define REG_REVERSE_UNDERVOLTAGE_CONTROL          0x1E    // Reverse Undervoltage Control
#define REG_VAC_MAX_POWER_POINT_DETECTED          0x1F    // VAC Max Power Point Detected
#define REG_CHARGER_STATUS_1                      0x21    // Charger Status 1
#define REG_CHARGER_STATUS_2                      0x22    // Charger Status 2
#define REG_CHARGER_STATUS_3                      0x23    // Charger Status 3
#define REG_FAULT_STATUS                           0x24   // Fault Status
#define REG_CHARGER_FLAG_1                         0x25   // Charger Flag 1
#define REG_CHARGER_FLAG_2                         0x26   // Charger Flag 2
#define REG_FAULT_FLAG                             0x27   // Fault Flag
#define REG_CHARGER_MASK_1                         0x28   // Charger Mask 1
#define REG_CHARGER_MASK_2                         0x29   // Charger Mask 2
#define REG_FAULT_MASK                             0x2A   // Fault Mask
#define REG_ADC_CONTROL                            0x2B   // ADC Control
#define REG_ADC_CHANNEL_CONTROL                    0x2C   // ADC Channel Control
#define REG_IAC_ADC                                0x2D   // IAC ADC
#define REG_IBAT_ADC                               0x2F   // IBAT ADC
#define REG_VAC_ADC                                0x31   // VAC ADC
#define REG_VBAT_ADC                               0x33   // VBAT ADC
#define REG_TS_ADC                                 0x37   // TS ADC
#define REG_VFB_ADC                                0x39   // VFB ADC
#define REG_GATE_DRIVER_STRENGTH_CONTROL           0x3B   // Gate Driver Strength Control
#define REG_GATE_DRIVER_DEAD_TIME_CONTROL          0x3C   // Gate Driver Dead Time Control
#define REG_PART_INFORMATION                       0x3D   // Part Information
#define REG_REVERSE_MODE_BATTERY_DISCHARGE_CURRENT 0x62   // Reverse Mode Battery Discharge Current




// BITS
#define FORCE_SWEEP_1       7
#define P_AND_O_TMR_2       5
#define FULL_SWEEP_TMR_2    1
#define EN_MPPT_1           0





// MASK

// Interrupt mask flags 1 for 0x28
#define ADC_DONE_MASK   (1 << 7)
#define IAC_DPM_MASK    (1 << 6)
#define VAC_DPM_MASK    (1 << 5)
#define WATCHDOG_MASK   (1 << 3)
#define CV_TMR_MASK     (1 << 1)
#define CHARGE_MASK     (1 << 0)

// Interrupt mask flags 2 for 0x29
#define PG_MASK         (1 << 7)
#define TS_MASK         (1 << 4)
#define REVERSE_MASK    (1 << 3)
#define FSW_SYNC_MASK   (1 << 1)
#define MPPT_MASK       (1 << 0)

// Interrupt mask flags 2 for 0x2A
#define VAC_UV_MASK     (1 << 7)
#define VAC_OV_MASK     (1 << 6)
#define IBAT_OCP_MASK   (1 << 5)
#define VBAT_OV_MASK    (1 << 4)
#define TSHUT_MASK      (1 << 3)
#define CHG_TMR_MASK    (1 << 2)
#define DRV_OKZ_MASK    (1 << 1)











// DEFINITIONS OF DIFERENTS STATS

// VBATLowV
#define VBAT_LOWV_30_PERCENT      0   // 30% x VFB_REG
#define VBAT_LOWV_55_PERCENT      1   // 55% x VFB_REG
#define VBAT_LOWV_66_7_PERCENT    2   // 66.7% x VFB_REG
#define VBAT_LOWV_71_4_PERCENT    3   // 71.4% x VFB_REG

// VRECHG
#define VRECHG_93_PERCENT         0   // 93.0% x VFB_REG
#define VRECHG_94_3_PERCENT       1   // 94.3% x VFB_REG
#define VRECHG_95_2_PERCENT       2   // 95.2% x VFB_REG
#define VRECHG_97_6_PERCENT       3   // 97.6% x VFB_REG

// TopOffTimer
#define TOP_OFF_DISABLE           0   // 00b = Disable
#define TOP_OFF_MINUTES_15        1   // 01b = 15 minutes
#define TOP_OFF_MINUTES_30        2   // 10b = 30 minutes
#define TOP_OFF_MINUTES_45        3   // 11b = 45 minutes

// WDTTimer
#define WATCHDOG_DISABLE               0   // Disable
#define WATCHDOG_SECONDS_40            1   // 40s
#define WATCHDOG_SECONDS_80            2   // 80s
#define WATCHDOG_SECONDS_160           3   // 160s

// CHGTMR
#define CHGTMR_HOURS_5            0   // 5 hours
#define CHGTMR_HOURS_8            1   // 8 hours
#define CHGTMR_HOURS_12           2   // 12 hours
#define CHGTMR_HOURS_24           3   // 24 hours

//FORCE_SWEEP
#define FORCE_SWEET_NORMAL        0
#define FORCE_SWEEP_START_FULL_SWEEP    1   // Start Full Panel Sweep (bit goes back to 0 after Full Panel Sweep complete)

// P_AND_O_TMR
#define P_AND_O_SECONDS_0         0   // 0 seconds
#define P_AND_O_SECONDS_0_5       1   // 0.5 seconds
#define P_AND_O_SECONDS_1         2   // 1 second
#define P_AND_O_SECONDS_10        3   // 10 seconds

// FULL_SWEEP_TMR
#define FULL_SWEEP_MIN_3          0   // 3 minutes
#define FULL_SWEEP_MIN_10         1   // 10 minutes
#define FULL_SWEEP_MIN_15         2   // 15 minutes
#define FULL_SWEEP_MIN_20         3   // 20 minutes

// ENABLE MPPT
#define DISABLE_MPPT              0
#define ENABLE_MPPT               1

// REVERSEIBATLIMIT  (no es poden usar noms que comencin amb número)
#define REVERSE_IBAT_20A          0   // 20A
#define REVERSE_IBAT_15A          1   // 15A
#define REVERSE_IBAT_10A          2   // 10A
#define REVERSE_IBAT_5A           3   // 5A

// ADC_BIT_DEPTH
#define ADC_SAMPLE_15BIT                 0   // 15-bit
#define ADC_SAMPLE_14BIT                 1   // 14-bit
#define ADC_SAMPLE_13BIT                 2   // 13-bit

// CHARGESTATE
#define CHARGE_STATE_NOT_CHARGING 0   // Not charging
#define CHARGE_STATE_TRICKLE      1   // Trickle Charge (VBAT < VBAT_SHORT)
#define CHARGE_STATE_PRE_CHARGE   2   // Pre-Charge (VBAT < VBAT_LOWV)
#define CHARGE_STATE_FAST_CHARGE  3   // Fast Charge (CC mode)
#define CHARGE_STATE_TAPER_CHARGE 4   // Taper Charge (CV mode)
#define CHARGE_STATE_RESERVED     5   // Reserved
#define CHARGE_STATE_TOP_OFF      6   // Top-off Timer Charge
#define CHARGE_STATE_DONE         7   // Charge Termination Done

// TSStatus
#define TS_STATUS_NORMAL          0   // Normal
#define TS_STATUS_WARM            1   // TS Warm
#define TS_STATUS_COOL            2   // TS Cool
#define TS_STATUS_COLD            3   // TS Cold
#define TS_STATUS_HOT             4   // TS Hot

// MPPTSTATUS
#define MPPT_DISABLED             0   // Disabled
#define MPPT_STANDBY              1   // MPPT Enabled, But Not Running
#define MPPT_PANELSWEEP           2   // Full Panel Sweep In Progress
#define MPPT_ENABLED              3   // Max Power Voltage Detected