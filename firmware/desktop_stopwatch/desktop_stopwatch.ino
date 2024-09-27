#include <EncButton.h>
#include <max7219.h>
#include <TimerMs.h>
#include <GyverTimers.h>
#include <BME280I2C.h>
#include <Wire.h>
#include <EEPROM.h>
#include <I2C_RTC.h>

// firmware configuration
#define DEBUG 0 // 1 - debug is activated, 0 - deactivated
#define VERSION "0.2.0" 
#define DISPLAY_INVERTED 0 // 1 - inverted connection of segments to MAX7219, 0 - no
#define USE_RTC_MODULE 0 // 1 - use RTC, 0 - don't use

#define INIT_KEY 129// key for eeprom settings storage
#define INIT_KEY_ADDR 1023

#define DISPLAY_BLINK_PERIOD 200 
#define BTN_STEP_TIMEOUT 80

#define BUZZER_PIN 5 // speaker pin
#define BUZZER_FREQ 523 // frequency of the sound wave
#define BUZZER_DURATION 400 // duration of speaking (ms)
#define BUZZER_DELAY 300 // delay between speakings

#include "utils/utils.h"

#if (DISPLAY_INVERTED == 1)
  #define ALIGNMENT_DISP  1
#else
  #define ALIGNMENT_DISP  0
#endif

enum Modes{STOPWATCH,
      CLOCK,
      TIMER,
      WEATHER,
      CLOCK_TUNE, 
      TIMER_TUNE,
      TIMER_EXPIRED};

enum WeatherUnit{TEMPERATURE, 
    HUMIDITY, 
    PRESSURE};

enum SensorType{NO_SENSOR, 
    SENSOR_BME280, 
    SENSOR_BMP280};

struct Time{
  uint8_t h = 0;
  uint8_t m = 0;
  uint8_t s = 0;
};

struct SetttingsData{
  uint8_t p1_display_brightness = 15;
  uint8_t p2_night_display_brightness = 15;
  uint8_t p3_nightbrightness_start_hour = 0;
  uint8_t p4_nightbrightness_end_hour= 0;
  uint8_t p5_show_seconds_clock = 1;
  uint32_t p6_maintimer_period = 1000000;
  uint8_t p7_wpage_change_freq = 5;
  uint8_t p8_weather_update_freq = 0;
  uint8_t p9_temperature_unit = 0;
  uint8_t p10_pres_unit = 0;
  uint8_t p11_use_speaker = 0;
};

MAX7219 max7219;
BME280I2C bme;
DS3231 rtc;

TimerMs btn_handler_timer(50, 1, 0); 
TimerMs wpage_timer;
TimerMs weather_update_timer; 
TimerMs display_blink_timer(DISPLAY_BLINK_PERIOD); 
TimerMs buzzer_timer(BUZZER_DURATION + BUZZER_DELAY); 

Modes current_mode = CLOCK;
WeatherUnit current_weat_page = TEMPERATURE;

volatile bool do_display_update = true; 
bool is_stopwatch_launched = false; 
bool is_timer_launched = false; 
bool is_display_on = true;
bool is_auto_brightness_allowed = true;
bool is_rtc_available = false;
int blinking_zone = -1;
SetttingsData settings;
SensorType connected_sensor = NO_SENSOR;

Time sw_time;
Time clock_time;
Time timer_time;
Time timer_start_time;

char disp_buf[17];

Button button1 = Button(2);
Button button2 = Button(3);
Button button3 = Button(4);


void setup() {
  pinMode(BUZZER_PIN, OUTPUT);

  #if (DEBUG == 1)
    Serial.begin(9600);
    Serial.println(F("Setup stage"));
  #endif

  load_settings(); 
  apply_settings();   

  max7219.Begin();

  Wire.begin();
  Wire.setClock(10000);

  DEBUG_PRINTLN(F("Sensor init"));
  if (bme.begin()){
    switch(bme.chipModel()) {
      case BME280::ChipModel_BME280:
        connected_sensor = SENSOR_BME280;
      break;
      case BME280::ChipModel_BMP280:
        connected_sensor = SENSOR_BMP280;
      break;
    }
  }   

  #if (USE_RTC_MODULE == 1)
    delay(1000); 
    is_rtc_available = rtc.begin();
    DEBUG_PRINTLN(F("RTС init"));
  #endif  
  if (is_rtc_available){  
    if(rtc.isRunning()){
      DEBUG_PRINTLN(F("RTС is running"));
      clock_time.h = (uint8_t)rtc.getHours();
      clock_time.m = (uint8_t)rtc.getMinutes();
      clock_time.s = (uint8_t)rtc.getSeconds();
    } else {
      DEBUG_PRINTLN(F("RTС is NOT running"));
      rtc.setHours((unsigned int) clock_time.h);
      rtc.setMinutes((unsigned int) clock_time.m);
      rtc.setSeconds((unsigned int) clock_time.s);
      rtc.startClock();
    }
  }
  Timer1.enableISR();  

  init_buttons();  
}

void loop() {
  // put your main code here, to run repeatedly:
  update_display();
  handle_timers(); 
}