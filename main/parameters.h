#ifndef PARAMETERS_H
#define PARAMETERS_H

#define VERSION "dev-6"

#define SLEEP_DELAY 100

#define DEBUG
// #define MONITOR

#define WORKING_MODE_DURATION 5000ul // ms

#define SENSOR 

#ifdef SENSOR
  #define SENSOR_SCL_PIN 2
  #define SENSOR_SDA_PIN 4
  #define SENSOR_I2C_ADDR 0x77 // for MS5611
  #define SENSOR_DELAY 1000 // ms
#endif

#define AP_SSID_PREFIX "curtain-switch-"
#define AP_SSID_PASSWORD "IoT-Curtain"

#define DEFAULT_LED_PIN 0         // D3
#define DEFAULT_UP_RELAY_PIN 15   // D8
#define DEFAULT_OPEN_BTN_PIN 5    // D1
#define DEFAULT_STOP_BTN_PIN 3    // RX 

#define DEFAULT_DOWN_RELAY_PIN 13 // D7
#define DEFAULT_CLOSE_BTN_PIN 12  // D6

#ifdef DEBUG
#define DEFAULT_DURATION 3000ul   // in milliseconds
#else
#define DEFAULT_DURATION 40000ul   // in milliseconds
#endif

#define BLINK_INTERVAL 700        // in milliseconds
#define BLINK_FAST_INTERVAL 350   // in milliseconds
#define BLINK_SLOW_INTERVAL 1000  // in milliseconds

#define DEFAULT_MIDDLE_COURSE_POSITION 0.5 // 0 to 1


#define DBL_CLICK_DURATION 800    // in milliseconds

#endif
