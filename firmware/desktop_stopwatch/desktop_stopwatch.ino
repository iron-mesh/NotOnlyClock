#include <EncButton.h>
#include <max7219.h>
#include <TimerMs.h>
#include <GyverTimers.h>
#include <BME280I2C.h>
#include <Wire.h>
#include <EEPROM.h>
#include <I2C_RTC.h>

// firmware configuration
#define DEBUG 0  // 1 - debug is activated, 0 - deactivated
#define VERSION "1.3.0"
#define DISPLAY_INVERTED 1  // 1 - inverted connection of segments to MAX7219, 0 - not
#define USE_RTC_MODULE 0    // 1 - use RTC, 0 - don't use

#define INIT_KEY 7  // key for eeprom settings storage
#define INIT_KEY_ADDR 1023

#define DISPLAY_BLINK_PERIOD 200
#define DOT_BLINK_PERIOD 500
#define BTN_STEP_TIMEOUT 80

#define BUZZER_PIN 5         // speaker pin
#define BUZZER_FREQ 523      // frequency of the sound wave
#define BUZZER_DURATION 400  // duration of speaking (ms)
#define BUZZER_DELAY 300     // delay between speakings

#define BTN_LIGHT_PIN 6

#include "utils/utils.h"

#if (DISPLAY_INVERTED == 1)
  #define ALIGNMENT_DISP  1
#else
  #define ALIGNMENT_DISP  0
#endif


//types definitions
enum Modes {
  STOPWATCH,
  TIMER,
  WEATHER,
  POMODORO,
  COUNTER,
  CLOCK,
  ALARM,
  CLOCK_TUNE,
  TIMER_TUNE,
  ALARM_TUNE,
  TIMER_EXPIRED,
  STOPWATCH_SELECT,
  TIMER_SELECT,
  COUNTER_SELECT,
  POMODORO_SETTINGS,
  POMODORO_TWEAKTIME};

enum WeatherUnit{
  TEMPERATURE, 
  HUMIDITY,
  PRESSURE};

enum SensorType{
  NO_SENSOR, 
  SENSOR_BME280,
  SENSOR_BMP280};

struct Time{
  uint8_t h = 0;
  uint8_t m = 0;
  uint8_t s = 0;
};

struct StopwatchUnit{
  bool is_launched = false;
  Time time;
};

struct TimerUnit{
  bool is_launched = false;
  bool is_expired = false;
  Time time, start_time;
};

struct SetttingsData{
  uint8_t p1_display_brightness = 15;
  uint8_t p2_night_display_brightness = 15;
  uint8_t p3_nightbrightness_start_hour = 0;
  uint8_t p4_nightbrightness_end_hour = 0;
  uint8_t p5_show_seconds_clock = 1;
  uint32_t p6_maintimer_period = 1000000;
  uint8_t p7_wpage_change_freq = 5;
  uint8_t p8_weather_update_freq = 0;
  uint8_t p9_temperature_unit = 0;
  uint8_t p10_pres_unit = 0;
  uint8_t p11_use_speaker = 1;
  uint8_t p12_alarm_duration = 30;
  uint8_t p13_snooze_duration = 10;
  byte p14_active_modes = 31;
  uint8_t p15_pomodoro_signal_duration = 3;
  uint8_t p16_btnlight_mode = 0;
};

//eeprom addresses
const int addr_settings = 0;
const int addr_alarm_time = sizeof(SetttingsData);
const int addr_alarm_status = addr_alarm_time + sizeof(Time);
const int addr_pomodoro_settings = addr_alarm_status + sizeof(bool);

//global variables
MAX7219 max7219;
BME280I2C bme;
DS3231 rtc;

TimerMs btn_handler_timer(50, 1, 0);
TimerMs wpage_timer;
TimerMs weather_update_timer;
TimerMs display_blink_timer(DISPLAY_BLINK_PERIOD);
TimerMs buzzer_timer(BUZZER_DURATION + BUZZER_DELAY);
TimerMs buzzer_switchoff_timer(1000, 0, 1);
TimerMs dot_blink_timer(DOT_BLINK_PERIOD);

Modes current_mode = CLOCK;
WeatherUnit current_weat_page = TEMPERATURE;

volatile bool do_display_update = true;
bool is_display_on = true;
bool is_auto_brightness_allowed = true;
bool is_rtc_available = false;
bool is_alarm_active = false;
bool is_alarm_snooze = false;
bool show_active_units_dots = false;
int blinking_zone = -1;
SetttingsData settings;
SensorType connected_sensor = NO_SENSOR;
volatile uint16_t alarm_off_counter = 0;
volatile uint16_t alarm_snooze_counter = 0;
uint8_t current_stopwatch = 0;
uint8_t current_timer = 0;
uint8_t current_counter = 0;

Time clock_time;
Time alarm_time;
const uint8_t UNIT_ARR_SIZE = 8;
StopwatchUnit stopwatches[UNIT_ARR_SIZE];
TimerUnit timers[UNIT_ARR_SIZE];
long counter_mode_values[9];

char disp_buf[17];

Button button1 = Button(2);
Button button2 = Button(3);
Button button3 = Button(4);

struct Pomodoro{
  struct {
    uint16_t pomodoro_time = 25;
    uint16_t chill_time = 5;
    uint8_t pomodoro_count = 4;
    uint16_t big_chill_time = 30;
    uint8_t pomodoro_auto_start = 1;
    uint8_t chill_auto_start = 1;
  } settings;

  enum class TunePages{
    POMODORO_TIME,
    CHILL_TIME,
    POMODORO_COUNT,
    BIG_CHILL_TIME,
    POMODORO_AUTO_START,
    CHILL_AUTO_START
  };
  TunePages tune_page;

  enum class Stages{
    POMODORO_STAGE,
    CHILLING_STAGE,
    BIG_CHILLING_STAGE
  };
  Stages pomodoro_stage;

  bool is_timer_launched = false;
  Time current_time;
  uint8_t current_pomodoro = 0;
};
Pomodoro pomodoro;


void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_LIGHT_PIN, OUTPUT);

#if (DEBUG == 1)
  Serial.begin(9600);
  Serial.println(F("Setup stage"));
#endif

  max7219.Begin();

  load_eeprom_data();
  apply_settings();
  apply_pomodoro_settings();

  Wire.begin();
  Wire.setClock(10000);

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
#endif
  if (is_rtc_available){  
    if(rtc.isRunning()){
      clock_time.h = (uint8_t)rtc.getHours();
      clock_time.m = (uint8_t)rtc.getMinutes();
      clock_time.s = (uint8_t)rtc.getSeconds();
    } else {
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