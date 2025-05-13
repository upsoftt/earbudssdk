#ifndef __temperature_detection__
#define __temperature_detection__
#include "includes.h"


#define NO_CONFIG_PORT                      (-1)

#define TEMPERATURE_DETECTION_ENABLE        0       //开关
#define TEMPERATURE_ABNORMAL_POWER_OFF      1
#define NOT_DETECTION_LOW_TEMPERATURE       0

#define ADC_MAXIMUM_VALUE                   (0X3FF)
#define PULL_UP_RESISTANCE                  (100)       //unit : K Ohm
// #define POWER_OFF_HIGHEST_TEMPERATURE       (50)
// #define POWER_OFF_LOW_TEMPERATURE           (0)

#define CHARGE_HIGHEST_TEMPERATURE          (45)  //����ʵ���¶��������¶����8��
#define CHARGE_LOW_TEMPERATURE              (4)   //����ʵ���¶��������¶����5��

#define DISCHARGE_HIGHEST_TEMPERATURE       (45)   //����ʵ���¶��������¶����2~3��
#define DISCHARGE_LOW_TEMPERATURE           (-15)  //����ʵ���¶��������¶����3��

#if TEMPERATURE_DETECTION_ENABLE
#define TEMPERATURE_DETECTION_PIN           IO_PORTB_02 //
#define TEMPERATURE_DETECTION_CHANNEL       AD_CH_PB2   //
#define TEMPERATURE_DETECTION_UP            1
#endif // TEMPERATURE_DETECTION_ENABLE

typedef enum {
    SYSTEM_POWER_ON,
    SYSTEM_POWER_OFF,
    SYSTEM_POWER_KEEP = 0XFF,
} system_power_event_list;

typedef struct struct_temperature_detection_value {
    const unsigned short temperature_number;
    int minimum_temperature;
    int maximum_temperature;
} temperature_detection_value_struct;

typedef struct struct_temperature_detection {
    void (*temperature_detection_switch)(unsigned char enable);
    int (*get_current_temperature)(void);
} temperature_detection_struct;
extern temperature_detection_struct temperature_detection_function();

#endif // __temperature_detection__
