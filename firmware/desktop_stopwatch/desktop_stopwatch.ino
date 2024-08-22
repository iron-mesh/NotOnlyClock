#include <EncButton.h>
#include <max7219.h>
#include <TimerMs.h>
#include <GyverTimers.h>
#include <BME280I2C.h>
#include <Wire.h>


enum Modes{STOPWATCH,
      CLOCK,
      TIMER,
      WEATHER};

enum WeatherUnit{PRESSURE,
      TEMPERATURE,
      HUMIDITY};

MAX7219 max7219;
TimerMs btn_handler_timer(50, 1, 0); 

Modes current_mode = STOPWATCH;

volatile bool do_display_update = true; 
bool is_stopwatch_launched = false; 
bool is_display_on = true; 

volatile uint8_t sw_hour = 0; //stop watch time elements
volatile uint8_t sw_min = 0;
volatile uint8_t sw_sec = 0;

Button button1 = Button(2);
Button button2 = Button(3);
Button button3 = Button(4);

BME280I2C bme;

void increase_sw_time(){
    sw_sec++;
    if (sw_sec > 59){
      sw_sec = 0;
      sw_min ++;
      if (sw_min > 59){
        sw_min = 0;
        sw_hour ++;
        if (sw_hour > 99){
          sw_sec = 0;
          sw_min = 0;
          sw_hour = 0;
        }
      }
    } 
  }

char* get_weather_param_str(WeatherUnit param){
  
}


void call_display_update(){
  do_display_update = true;
}

void switch_display(bool status){
  if (status){
    is_display_on = true;
    call_display_update();
  }
  else{
    is_display_on = false;
    max7219.Clear();
  }  
}

void set_next_mode(){
  if (current_mode == WEATHER){
    current_mode = STOPWATCH;
  }
  else{
    current_mode = current_mode + 1;
  }
  call_display_update();
}

void update_display(){
  if (!do_display_update || !is_display_on) return;

  switch (current_mode){
    case STOPWATCH:
      char buf[8];

      buf[0] = (sw_sec % 10) + 48;
      buf[1] = (sw_sec / 10) + 48;
      buf[2] ='-';
      buf[3] = (sw_min % 10) + 48;
      buf[4] = (sw_min / 10) + 48;
      buf[5] ='-';
      buf[6] = (sw_hour % 10) + 48;
      buf[7] = (sw_hour / 10) + 48;

      max7219.DisplayText(buf, 0);
    break;
  }
 
  do_display_update = false;
}

void handle_buttons(){
  button1.tick();
  button2.tick();
  button3.tick();

  if (is_display_on && button1.pressing() && button2.release()){
    switch_display(false);
    button1.reset();
    delay(2000);    
    return ;
  }

  if (!is_display_on && (button1.release() || button2.release() || button3.release())){
    switch_display(true);
    return ;
  }

  if (!is_display_on) return ;

  if(current_mode == STOPWATCH){

    if (button3.release()){
      if (is_stopwatch_launched){
        is_stopwatch_launched = false;
      }
      else{
        is_stopwatch_launched = true;
        call_display_update();
      }
    }

    if (button2.release()){
      if (!is_stopwatch_launched){
        sw_hour = 0;
        sw_min = 0;
        sw_sec = 0;
        call_display_update();
      }
      
    }

  }

}


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

  if (btn_handler_timer.tick()) handle_buttons();

}

ISR(TIMER1_A){
  if (is_stopwatch_launched)  {
    increase_sw_time();
    call_display_update();
  }
}
