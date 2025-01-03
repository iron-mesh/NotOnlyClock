
bool is_mode_available(Modes mode){
  byte am = settings.p14_active_modes;
  if (mode >= 0 && mode <= 4 && !bitRead(am, mode))
    return false;

  if (mode == WEATHER && connected_sensor == NO_SENSOR)
    return false;

  return true;
}

void set_mode(Modes mode){
  if (mode == current_mode || !is_mode_available(mode)) return;

  switch (current_mode){
    case TIMER:
    case STOPWATCH:
      dot_blink_timer.stop();
      show_active_units_dots = false;
    break;
    case WEATHER:
      wpage_timer.stop();
      weather_update_timer.stop();
    break;
    case CLOCK_TUNE:
      set_display_blinking(false, -1);
      try_change_brightness_mode(true);
      if (is_rtc_available){
        rtc.setHours((unsigned int) clock_time.h);
        rtc.setMinutes((unsigned int) clock_time.m);
        rtc.setSeconds((unsigned int) clock_time.s);           
      }
    break;
    case ALARM_TUNE:      
      set_display_blinking(false, -1);
      save_alarm();
    break;
    case TIMER_TUNE:
      timers[current_timer].time.h = timers[current_timer].start_time.h;
      timers[current_timer].time.m = timers[current_timer].start_time.m;
      timers[current_timer].time.s = timers[current_timer].start_time.s;
      set_display_blinking(false, -1);
    break;
    case TIMER_EXPIRED:
      set_display_blinking(false, -1);
      switch_sound(false, 0);
      for(int i = 0; i < UNIT_ARR_SIZE; i++){
        if (!timers[i].is_expired) continue;
        timers[i].is_expired = false;
        timers[i].time.h = timers[i].start_time.h;
        timers[i].time.m = timers[i].start_time.m;
        timers[i].time.s = timers[i].start_time.s;
      }
    break;
    case ALARM:
      set_display_blinking(false, -1);
      switch_sound(false, 0);
    break;
    case STOPWATCH_SELECT: 
    case TIMER_SELECT:
    case COUNTER_SELECT:
      set_display_blinking(false, -1);
    break;
   
    case POMODORO_SETTINGS:
      save_pomodoro_settings();
      apply_pomodoro_settings();
    break;
    case POMODORO_TWEAKTIME:
      set_display_blinking(false, -1);
    break;
  }

  switch (mode){
    case TIMER:
    case STOPWATCH:
      dot_blink_timer.start();
    break;    
    case WEATHER:
      if (settings.p7_wpage_change_freq > 0)
        wpage_timer.start();
      if (settings.p8_weather_update_freq > 0)
        weather_update_timer.start();
    break;
    case CLOCK_TUNE:
      set_display_blinking(true, -1);      
      clock_time.s = 0;
    break;
    case ALARM_TUNE:
      set_display_blinking(true, -1);
    break;
    case TIMER_TUNE:
      timers[current_timer].is_launched = false;
      set_display_blinking(true, 6);
    break;
    case TIMER_EXPIRED:
      set_display_blinking(true, -1);
      switch_display(true);
      if (settings.p11_use_speaker)
        switch_sound(true, 0);     
    break;
    case ALARM:
      set_display_blinking(true, -1);
      switch_display(true);
      switch_sound(true, 0);
      if (settings.p12_alarm_duration > 0)
        alarm_off_counter = 60 * settings.p12_alarm_duration;
    break;
    case STOPWATCH_SELECT: 
    case TIMER_SELECT:
    case COUNTER_SELECT:
      set_display_blinking(true, -1);
    break;

    case POMODORO_SETTINGS:
      pomodoro.is_timer_launched = false;
    break;

    case POMODORO_TWEAKTIME:      
      set_display_blinking(true, -1);
    break;
  }

  current_mode = mode;  
  clear_display_buffer();
  call_display_update();
}

void set_next_mode(){
  if (current_mode > CLOCK) return ;
  uint8_t mode = current_mode;

  while(1){
    mode = (mode == CLOCK)? 0 : mode + 1; 
    if(is_mode_available(mode)){
      set_mode(mode);
      return ;
    }
  }
}

String get_weather_param_str(WeatherUnit param){
  float value = NAN;
  String str;

  if (param == TEMPERATURE){
      BME280::TempUnit tempUnit((settings.p9_temperature_unit == 0) ? BME280::TempUnit_Celsius : BME280::TempUnit_Fahrenheit);
      value = bme.temp(tempUnit);
      str = String("t ");
      str.concat(String(value, 2));        
  } else if(param == HUMIDITY) {
      value = bme.hum();      
      str = String("h ");
      str.concat(String(value, 2));  
  }else if(param == PRESSURE){
      BME280::PresUnit pres_unit;
      switch(settings.p10_pres_unit){
        case 0:
          pres_unit = BME280::PresUnit_Pa;
        break;
        case 1:
          pres_unit = BME280::PresUnit_hPa;
        break;
        case 2:
          pres_unit = BME280::PresUnit_inHg;
        break;
      }
      
      value = bme.pres(pres_unit);
      if (pres_unit == BME280::PresUnit_Pa)
        value = value / 133.32;      
      str = String("p ");
      str.concat(String(value, 2));
  }
  return str;
}

template <typename T, typename T2>
  void in_de_crease_value(T &value, T2 min_value, T2 max_value, char direction){
    if (direction == '+'){
      if (value >= max_value) 
        value = min_value;
      else 
        value ++;  
    } else {
      if (value <= min_value) 
        value = max_value;
      else 
        value --;  
    }
  }


void switch_sound(bool status, uint32_t duration){
/* Switch on|off buzzer sound signal
    Parameters:
      status: true - on, false - off      
      duration: total duration of the signal (in ms) (if 0 - endless)
*/

  if (status){
    buzzer_timer.start();
    if (duration > 0){
      buzzer_switchoff_timer.setTime(duration);
      buzzer_switchoff_timer.start();
    }
  } else {
    buzzer_timer.stop();
    buzzer_switchoff_timer.stop();
  }
}

void switch_btnlight(bool status){
  digitalWrite(BTN_LIGHT_PIN, status);
}

