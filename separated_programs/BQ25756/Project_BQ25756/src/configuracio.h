#ifndef CONFIGURACIO_H
#define CONFIGURACIO_H

// Configuració HW
#define Rtop    249000.0
#define Rbot    30000.0
#define Rfbg    33.0

// Configuració SW
#define Vcharge         14.55       // segons HW: 13.97v - 14.55
#define Icharge         0.9
#define Iin_lim         3.00
#define Vin_lim         10.00
#define Iprecharge      0.25        // 0.25A - 10.0A
#define ITermination    0.25        // 0.25A - 10.0A

#define enable_precharge_control   true
#define enable_temination_control  true
#define enable_charge              true
#define watchdog_reset             false
#define enable_adc                 true
#define enable_mppt                true





#define WATCHDOG                   WATCHDOG_DISABLE

#define VBAT_LOWV                  VBAT_LOWV_30_PERCENT




#endif
