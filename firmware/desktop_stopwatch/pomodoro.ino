
void save_pomodoro_settings(){
  EEPROM.put(addr_pomodoro_settings, pomodoro.settings);
}

Time convert_minutes_to_time(uint16_t mins){
  Time res;
  res.h = mins / 60;
  res.m = mins - res.h * 60;
  return res;
}

void switch_pomodoro_stage(bool is_manual_switch){
  switch(pomodoro.pomodoro_stage){
    case Pomodoro::Stages::POMODORO_STAGE: 
      if (pomodoro.current_pomodoro > 1){        
        pomodoro.current_pomodoro -= 1;
        pomodoro.current_time = convert_minutes_to_time(pomodoro.settings.chill_time);
        pomodoro.pomodoro_stage = Pomodoro::Stages::CHILLING_STAGE;
      } else {        
        pomodoro.current_time = convert_minutes_to_time(pomodoro.settings.big_chill_time);
        pomodoro.pomodoro_stage = Pomodoro::Stages::BIG_CHILLING_STAGE; 
      }        

      if(is_manual_switch)
        pomodoro.is_timer_launched = true;
      else
        pomodoro.is_timer_launched = pomodoro.settings.chill_auto_start;  
    break;

    case Pomodoro::Stages::CHILLING_STAGE:
    case Pomodoro::Stages::BIG_CHILLING_STAGE:
      pomodoro.current_time = convert_minutes_to_time(pomodoro.settings.pomodoro_time);
      if(is_manual_switch)
        pomodoro.is_timer_launched = true;
      else
        pomodoro.is_timer_launched = pomodoro.settings.pomodoro_auto_start && !(pomodoro.pomodoro_stage == Pomodoro::Stages::BIG_CHILLING_STAGE);

      if (pomodoro.pomodoro_stage == Pomodoro::Stages::BIG_CHILLING_STAGE)
        pomodoro.current_pomodoro = pomodoro.settings.pomodoro_count;
      pomodoro.pomodoro_stage = Pomodoro::Stages::POMODORO_STAGE;      
    break;
  }
  call_display_update();
}

void apply_pomodoro_settings(){
  pomodoro.is_timer_launched = false;
  pomodoro.current_pomodoro = pomodoro.settings.pomodoro_count;
  pomodoro.current_time = convert_minutes_to_time(pomodoro.settings.pomodoro_time);
  pomodoro.pomodoro_stage = Pomodoro::Stages::POMODORO_STAGE;
  pomodoro.tune_page = Pomodoro::TunePages::POMODORO_TIME;
}