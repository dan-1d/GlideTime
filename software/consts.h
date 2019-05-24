
#define VERSION "v.1.0.2"
#define FLIGHT_HISTORY_MAX  50   // limit of statically-allocated flight time history (intervalA)
#define FLIGHT_HISTORY_DISPLAY_LENGTH  5  // number of previous flights to display at a time. Limited by LCD size and UI choices
#define LCD_CONTRAST 60 // Specific value for each particular Nokie 5110 LCD vendor. It varies depending on supplier.
#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 3
#define BUTTON_3_PIN 4
#define BUTTON_4_PIN 5
#define BUTTON_5_PIN 6
#define NUMBER_OF_BUTTONS 5
#define VBATT_PIN A0
#define BUTTON_DEBOUNCE_TIME  10  //hold time of rising edge. 'high' required for this long before triggering
#define SCREEN_REFRESH_PERIOD 100
#define EEPROM_ADDR_CALIBRATION_DATA 0x10
#define CALIBRATION_DURATION_SECONDS 60 * 1
#define BATT_V_ADC_CORRECTION 0.05 // VOLTS..   //float vbatt = vbatt_adc * 3.3 * 2.0 / 1024.0;

//Global Defines
typedef unsigned long ulong_t;
typedef unsigned int uint_t;
