void save_settings(){
  SetttingsData temp;
  EEPROM.get(addr_settings, temp);
  if (memcmp(&temp, &settings, sizeof(SetttingsData)) != 0) {
    EEPROM.put(addr_settings, settings);
    DEBUG_PRINTLN(F("Saved settings"));
  }
}

void save_alarm(){
  Time temp;
  EEPROM.get(addr_alarm_time, temp);
  if (memcmp(&temp, &alarm_time, sizeof(Time)) != 0) {
    EEPROM.put(addr_alarm_time, alarm_time);
    DEBUG_PRINTLN(F("Saved alarm time"));
  }
  if (EEPROM.read(addr_alarm_status) != is_alarm_active){
    EEPROM.write(addr_alarm_status, is_alarm_active);
    DEBUG_PRINTLN(F("Saved alarm status"));
  }
}

void load_eeprom_data(){
  if (EEPROM.read(INIT_KEY_ADDR) != INIT_KEY){
    save_settings();
    save_alarm();
    EEPROM.write(INIT_KEY_ADDR, INIT_KEY);
  }else{
    EEPROM.get(addr_settings, settings);
    EEPROM.get(addr_alarm_time, alarm_time);
    is_alarm_active = EEPROM.read(addr_alarm_status);
  }
}


void apply_settings(){
  max7219.MAX7219_SetBrightness(settings.p1_display_brightness);
  Timer1.setPeriod(settings.p6_maintimer_period);

  if (settings.p7_wpage_change_freq > 0)
    wpage_timer.setTime(settings.p7_wpage_change_freq * 1000);
  else
    wpage_timer.stop();

  if (settings.p8_weather_update_freq > 0)
    weather_update_timer.setTime(settings.p8_weather_update_freq * 1000);
  else
    weather_update_timer.stop();

  try_change_brightness_mode(true);
  // parameter 4 doesnt exist
}

uint8_t input_uint8(uint8_t value,  const uint8_t min, const uint8_t max, const bool change_bright){
  if (change_bright) max7219.MAX7219_SetBrightness(value);
  char disp_text[4] = "";
  if (value < min)
    value = min;
  else if (value > max)
    value = max;
  display_text(itoa(value, disp_text, DEC));

  while(1){
    if (!(button1.tick() || button2.tick() || button3.tick())) continue;

    if (button1.release()) return value;

    if (button2.step() || button2.click()){
        if (value == min)
          value = max;
        else
          value --;
        display_text(itoa(value, disp_text, DEC));
        if (change_bright) max7219.MAX7219_SetBrightness(value);
    }

    if (button3.step() || button3.click()){
        if (value == max)
          value = min;
        else
          value ++;
        display_text(itoa(value, disp_text, DEC));
        if (change_bright) max7219.MAX7219_SetBrightness(value);
    }
  }

}

uint32_t input_ulong (uint32_t value){
  // return inputed uint32_t; allowed value from 0 to 9 999 999
  char val_str[8] = "";
  ultoa(value, val_str, DEC);
  int str_len = strlen(val_str);
  if (str_len < 7){
    for (int i = str_len - 1; i >= 0; i--){
      val_str[i + (7-str_len)] = val_str[i];
    }
    for (int i = 0; i < 7-str_len; i++){
      val_str[i] = '0';
    }
  }

  display_text(val_str);

  TimerMs blinking_timer (DISPLAY_BLINK_PERIOD, 1, 0);
  uint8_t blinking_chindex = 0;
  bool bl_state = false;

  while(1){

    if (blinking_timer.tick()){
      uint8_t act_bl_ch = (DISPLAY_INVERTED) ? blinking_chindex : (7 - blinking_chindex);
      bl_state = !bl_state;
      if (bl_state)
        max7219.DisplayChar(act_bl_ch, val_str[blinking_chindex], false);
      else
        max7219.DisplayChar(act_bl_ch, ' ', false);
    }

    if (!(button1.tick() || button2.tick() || button3.tick())) continue;

    if (button1.release()) {
      return (uint32_t)atol(val_str);
    }

    if (button2.release()){
        if (blinking_chindex == 6)
          blinking_chindex = 0;
        else
          blinking_chindex ++;
        display_text(val_str);
    }

    if (button3.step() || button3.click()){
        if (val_str[blinking_chindex] == '9')
          val_str[blinking_chindex] = '0';
        else
          val_str[blinking_chindex] ++;
        display_text(val_str);
    }
  }
}


void edit_settings(){
  display_text(VERSION);
  delay(3000);

  is_auto_brightness_allowed = false;
  max7219.MAX7219_SetBrightness(settings.p1_display_brightness);

  uint8_t current_param = 1;
  display_parameter(current_param);
  while(1){
    if (!(button1.tick() || button2.tick() || button3.tick())) continue;

    if (button1.holding() && button2.holding() && button3.holding()) {
      reset_buttons();
      display_text("b4E");
      delay(2000);
      call_display_update();
      is_auto_brightness_allowed = true;
      break;
    }

    if (button2.step() || button2.click()){
      if (current_param == 1)
        current_param = 13;
      else
        current_param --;
      display_parameter(current_param);
    }

    if (button3.step() || button3.click()){
      if (current_param == 13)
        current_param = 1;
      else
        current_param ++;
      display_parameter(current_param);
    }


    if (button1.release()){
      switch(current_param){
        case 1:
          settings.p1_display_brightness = input_uint8(settings.p1_display_brightness, 0, 15, true);
          max7219.MAX7219_SetBrightness(settings.p1_display_brightness);
        break;
        case 2:
          settings.p2_night_display_brightness = input_uint8(settings.p2_night_display_brightness, 0, 15, true);
          max7219.MAX7219_SetBrightness(settings.p1_display_brightness);
        break;
        case 3:
          settings.p3_nightbrightness_start_hour = input_uint8(settings.p3_nightbrightness_start_hour, 0, 23, false);
        break;
        case 4:
          settings.p4_nightbrightness_end_hour = input_uint8(settings.p4_nightbrightness_end_hour, 0, 23, false);
        break;
        case 5:
          settings.p5_show_seconds_clock = input_uint8(settings.p5_show_seconds_clock, 0, 1, false);
        break;
        case 6:
          settings.p6_maintimer_period = input_ulong(settings.p6_maintimer_period);
          if (settings.p6_maintimer_period < 1)
            settings.p6_maintimer_period = 1;
        break;
        case 7:
          settings.p7_wpage_change_freq = input_uint8(settings.p7_wpage_change_freq, 0, 255, false);
        break;
        case 8:
          settings.p8_weather_update_freq = input_uint8(settings.p8_weather_update_freq, 0, 255, false);
        break;
        case 9:
          settings.p9_temperature_unit = input_uint8(settings.p9_temperature_unit, 0, 1, false);
        break;
        case 10:
          settings.p10_pres_unit = input_uint8(settings.p10_pres_unit, 0, 2, false);
        break;
        case 11:
          settings.p11_use_speaker = input_uint8(settings.p11_use_speaker, 0, 1, false);
        break;
        case 12:
          settings.p12_alarm_duration = input_uint8(settings.p12_alarm_duration, 0, 255, false);
        break;
        case 13:
          settings.p13_snooze_duration = input_uint8(settings.p13_snooze_duration, 0, 255, false);
        break;
      }
      display_parameter(current_param);
    }
  }
}