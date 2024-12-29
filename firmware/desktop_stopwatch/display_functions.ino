void set_display_blinking(bool status, int zone){
  blinking_zone = zone;
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

void send_time_to_dispbuff(){
  int i = 0;
  uint8_t time_unit;
  Time t;
  char sep;
  clear_display_buffer();
  switch (current_mode){
    case STOPWATCH:
    case STOPWATCH_SELECT:
      t = stopwatches[current_stopwatch].time;
      sep = '-';
    break;
    case TIMER:
    case TIMER_SELECT:
      t = timers[current_timer].time;
      sep = '_';
    break;
    case TIMER_EXPIRED:
      sep = '_';
    break;
    case TIMER_TUNE:
      t = timers[current_timer].start_time;
      sep = '_';
    break;
  }

  for (int a = 0; a < UNIT_ARR_SIZE; a++){ 
    switch(a){
      case 0: time_unit = t.h; break;
      case 3: time_unit = t.m; break;
      case 6: time_unit = t.s; break;
    }

    if (a != 2 && a != 5){
      if (a == 0 || a == 3 || a == 6)
        disp_buf[i] = (time_unit / 10) + 48;
      else
        disp_buf[i] = (time_unit % 10) + 48;
    }
    else
      disp_buf[i] = sep;
    i++;

    switch (current_mode){
      case STOPWATCH:
      case STOPWATCH_SELECT:
        if ((a == current_stopwatch) || (stopwatches[a].is_launched && show_active_units_dots))
          disp_buf[i++] = '.';
      break;
      case TIMER:
      case TIMER_SELECT:
        if ((a == current_timer) || (timers[a].is_launched && show_active_units_dots))
          disp_buf[i++] = '.';
      break;
      case TIMER_EXPIRED:
        if (timers[a].is_expired)
          disp_buf[i++] = '.';
      break;
    }  
  }
  
}

void send_clocktime_to_dispbuff(Time &t, bool show_dots, bool show_secs){
  int i = 0;
  if (!show_secs)
     disp_buf[i++] = ' ';
   
  for (int a = 0; a < 3; a++){
    if (a == 2 && !show_secs) break;
    uint8_t time_unit;
    switch(a){
      case 0: time_unit = t.h; break;
      case 1: time_unit = t.m; break;
      case 2: time_unit = t.s; break;
    }
    disp_buf[i++] = (time_unit / 10) + 48;
    disp_buf[i++] = (time_unit % 10) + 48;
    if(show_dots)
      disp_buf[i++] = '.';
    if (a == 2) break;
    disp_buf[i++] = ' ';
    if (a == 1 && !show_secs) break;
    if (!show_secs) disp_buf[i++] = ' ';    
  }
}

void update_display(){ 
  if (!do_display_update || !is_display_on) return; 

  switch (current_mode){
    case CLOCK:
    case ALARM:
      send_clocktime_to_dispbuff(clock_time, is_alarm_active, settings.p5_show_seconds_clock);
    break;

    case CLOCK_TUNE:
      send_clocktime_to_dispbuff(clock_time, false, false);
    break;

    case ALARM_TUNE:      
      send_clocktime_to_dispbuff(alarm_time, false, false);
      for(int i=2;i>0;i--)
        disp_buf[i+1] = disp_buf[i];
      disp_buf[0] = 'A';
      disp_buf[1] = ' ';
    break;

    case STOPWATCH:
    case STOPWATCH_SELECT:
    case TIMER:
    case TIMER_SELECT:
    case TIMER_EXPIRED:
    case TIMER_TUNE:
      send_time_to_dispbuff();
    break;
    
    case WEATHER:
      get_weather_param_str(current_weat_page).toCharArray(disp_buf, 9);
    break;

    case COUNTER:
    case COUNTER_SELECT:
      clear_display_buffer();
      ltoa(counter_mode_values[current_counter], disp_buf, DEC);
      for (int i = strlen(disp_buf); i < 7; i++){
        disp_buf[i] = ' ';
      }
      disp_buf[7] = (current_counter + 1) + 48;      
    break;

    case POMODORO_SETTINGS:
      clear_display_buffer();  
      switch(pomodoro.tune_page){
        case Pomodoro::TunePages::POMODORO_TIME:
          strcat(disp_buf, "Pt ");
          itoa((int)pomodoro.settings.pomodoro_time, disp_buf + 3, DEC);              
        break;
        case Pomodoro::TunePages::CHILL_TIME:
          strcat(disp_buf, "Ct ");
          itoa((int)pomodoro.settings.chill_time, disp_buf + 3, DEC);
        break;
        case Pomodoro::TunePages::POMODORO_COUNT:
          strcat(disp_buf, "PC ");
          itoa((int)pomodoro.settings.pomodoro_count, disp_buf + 3, DEC);
        break;
        case Pomodoro::TunePages::BIG_CHILL_TIME:
          strcat(disp_buf, "bCt ");
          itoa((int)pomodoro.settings.big_chill_time, disp_buf + 4, DEC);
        break;
        case Pomodoro::TunePages::POMODORO_AUTO_START:
          strcat(disp_buf, "PAS ");
          itoa((int)pomodoro.settings.pomodoro_auto_start, disp_buf + 4, DEC);
        break;
        case Pomodoro::TunePages::CHILL_AUTO_START:
          strcat(disp_buf, "CAS ");
          itoa((int)pomodoro.settings.chill_auto_start, disp_buf + 4, DEC);
        break;  
      }
    break;

    case POMODORO:
    case POMODORO_TWEAKTIME:     
      clear_display_buffer();  

      switch (pomodoro.pomodoro_stage){
        case Pomodoro::Stages::BIG_CHILLING_STAGE:
          strcat(disp_buf, "bC");
        break;
        case Pomodoro::Stages::CHILLING_STAGE:
          strcat(disp_buf, "C");
        break;       
        case Pomodoro::Stages::POMODORO_STAGE:
           itoa((int)pomodoro.current_pomodoro, disp_buf, DEC);
        break;        
      }    
      

      int min = pomodoro.current_time.h * 60 + pomodoro.current_time.m;
      char str_min[4];
      itoa(min, str_min, DEC);  
    
      int free_segs = (6 - strlen(disp_buf) - strlen(str_min));     
      int space_amount = free_segs > 1 ? free_segs - 1 : 1;

      for (int i=0; i < space_amount; i++)
        strcat(disp_buf, " "); 
      strcat(disp_buf, str_min); 
      strcat(disp_buf, free_segs > 1 ? "_" : ".");
      disp_buf[strlen(disp_buf)] = pomodoro.current_time.s / 10 + 48;
      disp_buf[strlen(disp_buf)] = pomodoro.current_time.s % 10 + 48;              
    break;
  }
   
  invert_buf();
  if (current_mode == WEATHER  || current_mode == POMODORO_SETTINGS) 
      max7219.Clear(); 
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
    } 
    else if (is_night_mode && !curr_hour_in_range()){
      max7219.MAX7219_SetBrightness(settings.p1_display_brightness);
      is_night_mode = false;
    }
    prev_hour = clock_time.h;
  }
}