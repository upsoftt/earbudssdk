#include "temperature_detection/temperature_detection.h"
#include "asm/charge.h"
#include "ui_manage.h"
#include "timer.h"


#define temperature_detection_log(format, ...)  \
printf("[temperature_detection]"format, ##__VA_ARGS__)

//unit : K Ohm
float temperature_resistance_list[] = {
    //  -20 ~ +79   temperature
#if 0
		981.85, 926.18, 874.02, 825.12, 779.27, 736.25, 695.88, 657.97, 622.36, 588.90,
		557.44, 527.85, 500.02, 473.82, 449.15, 425.92, 403.99, 383.33, 363.85, 345.47,
		328.13, 311.79, 296.35, 281.77, 267.99, 254.96, 242.62, 230.95, 219.91, 209.46,
		199.57, 190.21, 181.34, 172.94, 164.97, 157.42, 150.25, 143.44, 136.99, 130.86,
		125.04, 119.51, 114.25, 109.26, 104.51, 100.00, 95.704, 91.617, 87.726, 84.021,
		80.493, 77.133, 73.932, 70.881, 67.973, 65.199, 62.554, 60.030, 57.621, 55.322,
		53.127, 51.030, 49.028, 47.115, 45.285, 43.537, 41.867, 40.270, 38.743, 37.281,
		35.882, 34.543, 33.260, 32.032, 30.856, 29.729, 28.649, 27.613, 26.620, 25.669,
		24.755, 23.881, 23.042, 22.237, 21.464, 20.721, 20.007, 19.321, 18.661, 18.028,
		17.419, 16.836, 16.276, 15.738, 15.220, 14.721, 14.240, 13.777, 13.332, 12.903,
#else

#if 0
    971.07, 916.30, 865.01, 816.97, 771.93, 729.68, 689.83, 652.42, 617.32, 584.34,
    553.36, 524.07, 496.54, 470.64, 446.28, 423.34, 401.62, 381.17, 361.89, 343.73,
    326.60, 310.35, 295.02, 280.56, 266.89, 253.99, 241.74, 230.15, 219.20, 208.85,
    199.04, 189.73, 180.91, 172.56, 164.65, 157.14, 150.00, 143.23, 136.81, 130.71,
    124.93, 119.42, 114.18, 109.21, 104.49, 100.00, 95.710, 91.641, 87.761, 84.072,
    80.562, 77.202, 74.013, 70.963, 68.063, 65.303, 62.664, 60.144, 57.744, 55.464,
    53.274, 51.175, 49.185, 47.275, 45.445, 43.705, 42.035, 40.425, 38.895, 37.435,
    36.036, 34.696, 33.406, 32.176, 30.996, 29.866, 28.786, 27.746, 26.756, 25.796,
    24.886, 24.006, 23.156, 22.356, 21.576, 20.836, 20.116, 19.436, 18.766, 18.136,
    17.526, 16.936, 16.376, 15.826, 15.306, 14.806, 14.315, 13.855, 13.405, 12.765,
#else
#if 1
//187.44, 175.84, 165.04, 154.97, 145.59, 136.83, 128.66, 121.03, 113.91, 107.24,
101.02, 95.190, 89.740, 84.630, 79.850, 75.370, 71.170, 67.220, 63.530, 60.060,
56.800, 53.740, 50.860, 48.150, 45.610, 43.220, 40.960, 38.840, 36.840, 34.960,
33.180, 31.510, 29.930, 28.440, 27.030, 25.700, 24.440, 23.250, 22.130, 20.070,
19.120, 18.220, 17.360, 16.560, 15.790, 15.070, 14.380, 13.730, 13.110, 12.520,
11.960, 11.440, 10.930, 10.450, 10.000, 9.570,  9.160,	8.770,	8.390,	8.040,
7.700,	7.380,	7.080,	6.790,	6.510,	6.240,	5.990,	5.750,	5.520,	5.300,
5.090,	4.890,	4.700,	4.520,	4.340,	4.355,	4.170,	4.010,	3.860,	3.720,
3.580,	3.450,	3.320,	3.190,	3.080,	2.960,	2.860,	2.750,	2.650,	2.560,
2.470,	2.380,	2.300,	2.220,	2.140,	2.070,	1.990,	1.930,	1.860,	1.800,
1.740,	1.682,	1.626,	1.572,	1.520,	1.470,	1.422,	1.375,	1.331,	1.288,

#else
    97.603, 92.094, 86.930, 82.087, 77.544, 73.279, 69.275, 65.514, 61.980, 58.658,
    55.534, 52.595, 49.829, 47.225, 44.772, 42.462, 40.284, 38.230, 36.294, 34.466,
    32.742, 31.113, 29.575, 28.122, 26.749, 25.451, 24.223, 23.061, 21.962, 20.921,
    19.936, 19.002, 18.118, 17.280, 16.485, 15.731, 15.016, 14.337, 13.693, 13.081,
    12.500, 11.948, 11.423, 10.925, 10.451, 10.000, 9.570,  9.162,  8.773,  8.403,
    8.051,  7.715,  7.395,  7.090,  6.799,  6.522,  6.257,  6.005,  5.764,  5.534,
    5.315,  5.105,  4.905,  4.713,  4.530,  4.355,  4.188,  4.028,  3.875,  3.729,
    3.589,  3.455,  3.326,  3.203,  3.086,  2.973,  2.865,  2.761,  2.662,  2.566,
    2.475,  2.387,  2.303,  2.223,  2.145,  2.071,  1.999,  1.931,  1.865,  1.801,
    1.741,  1.682,  1.626,  1.572,  1.520,  1.470,  1.422,  1.375,  1.331,  1.288,
 #endif
#endif
#endif
};

const unsigned short temperature_number = sizeof(temperature_resistance_list)/sizeof(temperature_resistance_list[0]);
/*
temperature_detection_value_struct temperature_detection_value = {
    .temperature_number = sizeof(temperature_resistance_list)/sizeof(temperature_resistance_list[0]),
    .minimum_temperature = POWER_OFF_LOW_TEMPERATURE,
    .maximum_temperature = POWER_OFF_HIGHEST_TEMPERATURE,
};
*/

static float get_float_current_resistance()
{
#if TEMPERATURE_DETECTION_CHANNEL != NO_CONFIG_PORT && defined TEMPERATURE_DETECTION_CHANNEL
    unsigned int adc_value = adc_get_value(TEMPERATURE_DETECTION_CHANNEL);
    if(ADC_MAXIMUM_VALUE - adc_value) {
        return (float)((TEMPERATURE_DETECTION_UP ? 10 : PULL_UP_RESISTANCE) * adc_value) / (float)(ADC_MAXIMUM_VALUE - adc_value);
    }
#endif
    return 0xffff;
}

static int get_current_temperature(void)
{
    if(temperature_number) {
        float resistance = get_float_current_resistance();
        if(resistance >= temperature_resistance_list[0] || resistance < temperature_resistance_list[temperature_number - 1]) {
            return -0xffff;
        }
        for(unsigned short i = 0; i < temperature_number; i++) {
            if (resistance >= temperature_resistance_list[i]) {
                return i - 20;
            }
        }
    }
    return -0xffff;
}

static void temperature_detection_power_off()
{
    temperature_detection_log("%s\n", __func__);
    extern void power_set_soft_poweroff();
    if(get_charge_online_flag()) {
#if TEMPERATURE_ABNORMAL_POWER_OFF
        power_set_soft_poweroff();
#else
        charge_close();
        ui_update_status(STATUS_CHARGE_CLOSE);
#endif // TEMPERATURE_ABNORMAL_POWER_OFF
    } else {
#if TEMPERATURE_ABNORMAL_POWER_OFF
        power_set_soft_poweroff();
#endif // TEMPERATURE_ABNORMAL_POWER_OFF
    }
}

static void temperature_detection_power_on()
{
    temperature_detection_log("%s\n", __func__);
    extern void power_set_soft_poweroff();
    if(get_charge_online_flag()) {
#if TEMPERATURE_ABNORMAL_POWER_OFF
#else
        charge_start();
        ui_update_status(STATUS_CHARGE_START);
#endif // TEMPERATURE_ABNORMAL_POWER_OFF
    }
}

static void temperature_detection_main(unsigned short cycle)
{
    static unsigned char charge_flag = 0xff;

    static unsigned short event_delay_counter = 0;
    static unsigned short event_delay_time = 0;

    static unsigned char previous_power_event = 0xff;
    static unsigned char saved_power_event = 0xff;
    unsigned char current_power_event = 0xff;

    static unsigned char detection_counter = 0;
    static int saved_temperature = -0xffff;
    static int current_temperature = -0xffff;

    if(saved_temperature != get_current_temperature()) {
        saved_temperature = get_current_temperature();
        detection_counter = 0;
    }
    if(detection_counter < 3) {
        detection_counter++;
        if(detection_counter == 3) {
            current_temperature = saved_temperature;
        }
    }

    temperature_detection_log("detection_counter = %d, current_temperature = %d\n", detection_counter, current_temperature);

//    if(current_temperature != -0xffff && detection_counter >= 3) {
        if(get_charge_online_flag()) {
            if(current_temperature >= CHARGE_HIGHEST_TEMPERATURE || (current_temperature <= CHARGE_LOW_TEMPERATURE)) {
                event_delay_time = 2000 / cycle;
                current_power_event = SYSTEM_POWER_OFF;
            } else {
                event_delay_time = 2000 / cycle;
                current_power_event = SYSTEM_POWER_ON;
            }
        } else {
            if(current_temperature >= DISCHARGE_HIGHEST_TEMPERATURE ||(current_temperature <= DISCHARGE_LOW_TEMPERATURE)) {
                event_delay_time = 2000 / cycle;
                current_power_event = SYSTEM_POWER_OFF;
            } else {
                event_delay_time = 2000 / cycle;
                current_power_event = SYSTEM_POWER_ON;
            }
        }

        if(current_power_event != saved_power_event || charge_flag != get_charge_online_flag()) {
            charge_flag = get_charge_online_flag();
            event_delay_counter = 0;
            previous_power_event = saved_power_event;
            saved_power_event = current_power_event;
        }

        if(event_delay_counter < event_delay_time) {
            event_delay_counter++;
            if(event_delay_counter == event_delay_time) {
                switch(current_power_event) {
                case SYSTEM_POWER_OFF:
                    temperature_detection_power_off();
                    break;
                case SYSTEM_POWER_ON:
                    temperature_detection_power_on();
                    break;
                case SYSTEM_POWER_KEEP:
                default:
                    break;
                }
            }
        }
//    }
}

static void temperature_detection_device_init()
{
#if TEMPERATURE_DETECTION_PIN != NO_CONFIG_PORT && defined TEMPERATURE_DETECTION_PIN
#if TEMPERATURE_DETECTION_CHANNEL != NO_CONFIG_PORT && defined TEMPERATURE_DETECTION_CHANNEL
    adc_add_sample_ch(TEMPERATURE_DETECTION_CHANNEL);
#endif
    gpio_set_die(TEMPERATURE_DETECTION_PIN, 0);
    gpio_set_direction(TEMPERATURE_DETECTION_PIN, 1);
    gpio_set_pull_down(TEMPERATURE_DETECTION_PIN, 0);
    gpio_set_pull_up(TEMPERATURE_DETECTION_PIN, TEMPERATURE_DETECTION_UP);
#endif
}

static void temperature_detection_switch(unsigned char enable)
{
#if TEMPERATURE_DETECTION_ENABLE
    static unsigned short temperature_detection_timer = 0;
    if(enable) {
        if(!temperature_detection_timer) {
            temperature_detection_device_init();
            temperature_detection_timer = sys_timer_add((void*)500, temperature_detection_main, 500);
        }
    } else {
        if(temperature_detection_timer) {
            sys_timer_del(temperature_detection_timer);
            temperature_detection_timer = 0;
        }
    }
#endif // TEMPERATURE_DETECTION_ENABLE
}

static temperature_detection_struct this_function = {
    .temperature_detection_switch = temperature_detection_switch,
    .get_current_temperature = get_current_temperature,
};

temperature_detection_struct temperature_detection_function()
{
    return this_function;
}

