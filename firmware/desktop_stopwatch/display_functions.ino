void set_display_blinking(bool status){
  if (status){
    display_blink_timer.start();
  } else{
    display_blink_timer.stop();
    switch_display(true);
  }
}

void blink_display(){
  static bool state = false;  
  state = !state;
  if (state)
     max7219.DisplayText(disp_buf, ALIGNMENT_DISP);
  else
    if (blinking_zone < 0)
      max7219.Clear();
    else {
      char cutted_disp_buf[9];
      memcpy(cutted_disp_buf, disp_buf, sizeof(disp_buf));
      if (DISPLAY_INVERTED){
        cutted_disp_buf[7 - blinking_zone] = ' ';
        cutted_disp_buf[7 - (blinking_zone + 1)] = ' ';
      } else {
        cutted_disp_buf[blinking_zone] = ' ';
        cutted_disp_buf[blinking_zone + 1] = ' ';
      }
      max7219.DisplayText(cutted_disp_buf, ALIGNMENT_DISP);
    }
}

void call_display_update(){
  // execute display processing on the next cycle
  do_display_update = true;
}

void switch_display(bool status){
  // switch (on/off) display; status: false - off, true - on
  if (status){
    is_display_on = true;
    call_display_update();
  }
  else{
    is_display_on = false;
    max7219.Clear();
  }  
}

void send_time_to_dispbuff(Time &time, const char sep, bool show_sec){
  if (show_sec){
    disp_buf[0] = (time.h / 10) + 48;
    disp_buf[1] = (time.h % 10) + 48; 
    disp_buf[2] = sep;
    disp_buf[3] = (time.m / 10) + 48;
    disp_buf[4] = (time.m % 10) + 48;
    disp_buf[5] = sep;
    disp_buf[6] = (time.s / 10) + 48;
    disp_buf[7] = (time.s % 10) + 48;
  } else {
    disp_buf[0] = sep;
    disp_buf[1] = (time.h / 10) + 48;
    disp_buf[2] = (time.h % 10) + 48; 
    disp_buf[3] = sep;
    disp_buf[4] = sep;
    disp_buf[5] = (time.m / 10) + 48;
    disp_buf[6] = (time.m % 10) + 48;
    disp_buf[7] = sep;
  }  
}

void send_clocktime_to_dispbuff_doted(bool show_sec){
  if (show_sec){
    disp_buf[0] = (clock_time.h / 10) + 48;
    disp_buf[1] = (clock_time.h % 10) + 48;
    disp_buf[2] = '.';
    disp_buf[3] = ' ';
    disp_buf[4] = (clock_time.m / 10) + 48;
    disp_buf[5] = (clock_time.m % 10) + 48;
    disp_buf[6] = '.';
    disp_buf[7] = ' ';
    disp_buf[8] = (clock_time.s / 10) + 48;
    disp_buf[9] = (clock_time.s % 10) + 48;
    disp_buf[10] = '.';
  } else {
    disp_buf[0] = ' ';
    disp_buf[1] = (clock_time.h / 10) + 48;
    disp_buf[2] = (clock_time.h % 10) + 48;
    disp_buf[3] = '.';
    disp_buf[4] = ' ';
    disp_buf[5] = ' ';
    disp_buf[6] = (clock_time.m / 10) + 48;
    disp_buf[7] = (clock_time.m % 10) + 48;
    disp_buf[8] = '.';
    disp_buf[9] = ' ';
  }  
}

void update_display(){
  if (!do_display_update || !is_display_on) return;

  switch (current_mode){
    case CLOCK:
    case ALARM:
      if (is_alarm_active)
        send_clocktime_to_dispbuff_doted(settings.p5_show_seconds_clock);
      else      
        send_time_to_dispbuff(clock_time, ' ', settings.p5_show_seconds_clock);
    break;

    case CLOCK_TUNE:
      send_time_to_dispbuff(clock_time, ' ', false);
    break;

    case ALARM_TUNE:
      send_time_to_dispbuff(alarm_time, ' ', false);
      for(int i=2;i>0;i--)
        disp_buf[i+1] = disp_buf[i];
      disp_buf[0] = 'A';
      disp_buf[1] = ' ';
    break;

    case STOPWATCH:
      send_time_to_dispbuff(sw_time, '-', true);
    break;

    case TIMER:
    case TIMER_EXPIRED:
      send_time_to_dispbuff(timer_time, '_', true);    
    break;

    case TIMER_TUNE:
      send_time_to_dispbuff(timer_start_time, '_', true);
    break;

    case WEATHER:
      get_weather_param_str(current_weat_page).toCharArray(disp_buf, 9);
    break;
  }
  
  invert_buf();
  if (current_mode == WEATHER) max7219.Clear();
  max7219.DisplayText(disp_buf, ALIGNMENT_DISP);
 
  do_display_update = false;
}

void clear_display_buffer(){
  for (uint8_t i = 0; i < 17; i++){
    disp_buf[i] = '\0';
  }
}

void invert_buf(){
#if (DISPLAY_INVERTED == 1)
  char s[17];
  uint8_t buf_len = strlen(disp_buf);

  for (uint8_t i = 0; i < buf_len; i++){
    s[i] = disp_buf[i];
  }

  // clear_display_buffer();
  for (uint8_t i = 0; i < buf_len; i++){
    disp_buf[i] = s[buf_len - 1 - i];
  }

  uint8_t i = 0;
  while(i < buf_len){
    if (disp_buf[i] == '.') {
      disp_buf[i] = disp_buf[i + 1];
      disp_buf[i + 1] = '.';
      i += 2;
    } else 
      i++;
  }
#endif
}

void display_text(char *text){
  if (!is_display_on) return;
  clear_display_buffer();
  uint8_t index = 0;
  while(1){
    if (text[index] == '\0' || index > 15) break;
    disp_buf[index] = text[index];
    index ++;
  }
  invert_buf();
  max7219.Clear();
  max7219.DisplayText(disp_buf, ALIGNMENT_DISP);
}

void display_parameter(const uint8_t param_num){
  char text[4] = "P";
  if (param_num < 10){
    text[1] = param_num + 48;
  } else{
    text[1] = param_num / 10 + 48;
    text[2] = param_num % 10 + 48;
  }
  display_text(text);
}

void change_wpage(int8_t direction, bool reset_timers){
  // change current displayed weather parameter: direction (if less then 0 - show previous, if greater - next)
  if (direction > 0){
    if (current_weat_page == 0 && connected_sensor == SENSOR_BMP280)
      current_weat_page = 2;
    else
      if (current_weat_page == 2)
        current_weat_page = 0;
      else 
        current_weat_page = current_weat_page + 1;
  } else if (direction < 0){
    if (current_weat_page == 2 && connected_sensor == SENSOR_BMP280)
      current_weat_page = 0;
    else
      if (current_weat_page == 0)
        current_weat_page = 2;
      else 
        current_weat_page = current_weat_page - 1;
  }

  if (reset_timers){
    if (wpage_timer.active())
      wpage_timer.start();
    if (weather_update_timer.active())
      weather_update_timer.start();  
  }
  
  call_display_update();
}

void try_change_brightness_mode(const bool force_checking){
  if (!is_auto_brightness_allowed || settings.p3_nightbrightness_start_hour == settings.p4_nightbrightness_end_hour) return;
  volatile static uint8_t prev_hour = clock_time.h;
  volatile static bool is_night_mode = false;
  if (force_checking){
    DEBUG_PRINTLN(F("Force checking"));
    is_night_mode = curr_hour_in_range();
    if (is_night_mode)
      max7219.MAX7219_SetBrightness(settings.p2_night_display_brightness);
    else
      max7219.MAX7219_SetBrightness(settings.p1_display_brightness);
    return;
  }

  if (prev_hour != clock_time.h){
    if (!is_night_mode && curr_hour_in_range()){
      max7219.MAX7219_SetBrightness(settings.p2_night_display_brightness);
      is_night_mode = true;
      DEBUG_PRINTLN(F("Set night mode"));
    } 
    else if (is_night_mode && !curr_hour_in_range()){
      max7219.MAX7219_SetBrightness(settings.p1_display_brightness);
      is_night_mode = false;
      DEBUG_PRINTLN(F("Set day mode"));
    }
    prev_hour = clock_time.h;
  }
}