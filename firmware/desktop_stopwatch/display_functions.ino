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
  switch_display(state);
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

void send_time_to_dispbuff(Time &time, const char sep){
  disp_buf[0] = (time.h / 10) + 48;
  disp_buf[1] = (time.h % 10) + 48; 
  disp_buf[2] = sep;
  disp_buf[3] = (time.m / 10) + 48;
  disp_buf[4] = (time.m % 10) + 48;
  disp_buf[5] = sep;
  disp_buf[6] = (time.s / 10) + 48;
  disp_buf[7] = (time.s % 10) + 48;
}

void update_display(){
  if (!do_display_update || !is_display_on) return;

  switch (current_mode){
    case CLOCK:
    case CLOCK_TUNE:
      send_time_to_dispbuff(clock_time, ' ');
    break;
    case STOPWATCH:
      send_time_to_dispbuff(sw_time, '-');
    break;
    case WEATHER:
      get_weather_param_str(current_weat_page).toCharArray(disp_buf, 9);
    break;
  }
  
  invert_buf();
  if (current_mode == WEATHER) max7219.Clear();
  max7219.DisplayText(disp_buf, 1);
 
  do_display_update = false;
}

void clear_display_buffer(){
  for (uint8_t i = 0; i < 9; i++){
    disp_buf[i] = '\0';
  }
}

void invert_buf(){
  char s[9];
  uint8_t buf_len = strlen(disp_buf);

  for (uint8_t i = 0; i < buf_len; i++){
    s[i] = disp_buf[i];
  }

  // clear_display_buffer();
  for (uint8_t i = 0; i < buf_len; i++){
    disp_buf[i] = s[buf_len - 1 - i];
  }

  uint8_t dot_pos = -1;
  for (uint8_t i = 0; i < buf_len; i++){
    if (disp_buf[i] == '.') dot_pos = i;
  }

  if (dot_pos != -1){
    char c = disp_buf[dot_pos + 1];
    disp_buf[dot_pos] = c;
    disp_buf[dot_pos + 1] = '.';
  }
}