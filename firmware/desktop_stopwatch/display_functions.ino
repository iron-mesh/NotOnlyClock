void call_display_update(){
  // execute display processing on the next cycle
  do_display_update = true;
}

void switch_display(bool status){
  // switch on/off display; status: false - off, true - on
  if (status){
    is_display_on = true;
    call_display_update();
  }
  else{
    is_display_on = false;
    max7219.Clear();
  }  
}

void update_display(){
  if (!do_display_update || !is_display_on) return;

  switch (current_mode){
    case STOPWATCH:
      disp_buf[0] = (sw_hour / 10) + 48;
      disp_buf[1] = (sw_hour % 10) + 48; 
      disp_buf[2] ='-';
      disp_buf[3] = (sw_min / 10) + 48;
      disp_buf[4] = (sw_min % 10) + 48;
      disp_buf[5] ='-';
      disp_buf[6] = (sw_sec / 10) + 48;
      disp_buf[7] = (sw_sec % 10) + 48;
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
    disp_buf[i] = ' ';
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