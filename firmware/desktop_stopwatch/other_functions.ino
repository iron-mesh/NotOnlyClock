
void set_mode(Modes mode){
  switch (current_mode){
    case WEATHER:
      wpage_timer.stop();
      weather_update_timer.stop();
    break;
    case CLOCK_TUNE:
      set_display_blinking(false);
      try_change_brightness_mode(true);
      if (is_rtc_available){
        DEBUG_PRINTLN(F("RTS set time"));
        rtc.setHours((unsigned int) clock_time.h);
        rtc.setMinutes((unsigned int) clock_time.m);
        rtc.setSeconds((unsigned int) clock_time.s);           
      }
    break;
    case ALARM_TUNE:      
      set_display_blinking(false);
      save_alarm();
    break;
    case TIMER_TUNE:
    case TIMER_EXPIRED:
      blinking_zone = -1;
      set_display_blinking(false);
      buzzer_timer.stop();
      timer_time.h = timer_start_time.h;
      timer_time.m = timer_start_time.m;
      timer_time.s = timer_start_time.s;
    break;
    case ALARM:
      set_display_blinking(false);
      buzzer_timer.stop();  
    break;
  }

  switch (mode){
    case WEATHER:
      if (settings.p7_wpage_change_freq > 0)
        wpage_timer.start();
      if (settings.p8_weather_update_freq > 0)
        weather_update_timer.start();
    break;
    case CLOCK_TUNE:
      set_display_blinking(true);      
      clock_time.s = 0;
    break;
    case ALARM_TUNE:
      set_display_blinking(true);
      // is_alarm_snooze = false;
    break;
    case TIMER_TUNE:
      is_timer_launched = false;
      set_display_blinking(true);
      blinking_zone = 6;
    break;
    case TIMER_EXPIRED:
      is_timer_launched = false;
      set_display_blinking(true);
      switch_display(true);
      if (settings.p11_use_speaker)
        buzzer_timer.start();      
    break;
    case ALARM:
      set_display_blinking(true);
      switch_display(true);
      buzzer_timer.start();
      if (settings.p12_alarm_duration > 0)
        alarm_off_counter = 60 * settings.p12_alarm_duration;
    break;
  }

  current_mode = mode;
  clear_display_buffer();
  call_display_update();
}

void set_next_mode(){
  switch(current_mode){
    case CLOCK:
      set_mode(STOPWATCH);
    break;
    case STOPWATCH:
      set_mode(TIMER);
    break;
    case TIMER:
      if (connected_sensor != NO_SENSOR)
        set_mode(WEATHER);
      else
        set_mode(CLOCK);
    break;
    case WEATHER:
      set_mode(CLOCK);
    break;
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

