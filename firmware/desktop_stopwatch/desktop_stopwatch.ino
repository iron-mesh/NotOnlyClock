#include <EncButton.h>
#include <max7219.h>
#include <TimerMs.h>
#include <GyverTimers.h>
#include <BME280I2C.h>
#include <Wire.h>

#define WPAGE_DURATION 5000 // duration of a weather unit demonstration
#define VERSION "0_1_0"

enum Modes{STOPWATCH,
      CLOCK,
      TIMER,
      WEATHER,
      CLOCK_TUNE,
      OPTIONS};

enum WeatherUnit{TEMPERATURE, 
    HUMIDITY, 
    PRESSURE};

MAX7219 max7219;
BME280I2C bme;

TimerMs btn_handler_timer(50, 1, 0); 
TimerMs wpage_timer(WPAGE_DURATION); 

Modes current_mode = STOPWATCH;
WeatherUnit current_weat_page = TEMPERATURE;

volatile bool do_display_update = true; 
bool is_stopwatch_launched = false; 
bool is_display_on = true; 

volatile uint8_t sw_hour = 0; //stop watch time elements
volatile uint8_t sw_min = 0;
volatile uint8_t sw_sec = 0;

char disp_buf[9];

Button button1 = Button(2);
Button button2 = Button(3);
Button button3 = Button(4);


void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);

  Timer1.setPeriod(995853); 
  Timer1.enableISR();

  max7219.Begin();
  max7219.Clear();

  Wire.begin();
  bme.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  update_display();
  handle_timers();  

}


