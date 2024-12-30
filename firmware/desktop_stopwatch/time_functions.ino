
void increase_time(Time &time, const uint8_t max_hour){
  time.s += 1;
  if (time.s > 59){
    time.s = 0;
    time.m ++;
    if (time.m > 59){
      time.m = 0;
      time.h ++;
      if (time.h > max_hour){
        time.s = 0;
        time.m = 0;
        time.h = 0;
      }
    }
  } 
}

void decrease_time(Time &time){
  if (time.h <= 0 && time.m <= 0 && time.s <= 0) return ;  
  if (time.s == 0){
    time.s = 60;    
    if (time.m == 0){
      time.m = 59;
      time.h --;
    } else {
      time.m --;
    }
  } 
  time.s -= 1;
}

bool curr_hour_in_range(){
  uint8_t curr_h = clock_time.h;
  uint8_t start_h = settings.p3_nightbrightness_start_hour;
  uint8_t end_h = settings.p4_nightbrightness_end_hour;

  if (start_h > end_h)
    return (curr_h >= start_h && curr_h <= 23) || (curr_h >= 0 && curr_h < end_h);
  else
    return (curr_h >= start_h && curr_h < end_h);
}

ISR(TIMER1_A){

  if (current_mode != CLOCK_TUNE){
    uint8_t prev_min = clock_time.m;
    increase_time(clock_time, 23);
    try_change_brightness_mode(false); 
    if ((current_mode == CLOCK || current_mode == ALARM) && (settings.p5_show_seconds_clock || prev_min != clock_time.m))
        call_display_update();
    if (is_alarm_active && (current_mode != ALARM_TUNE) && prev_min != clock_time.m ){
      if (clock_time.h == alarm_time.h && clock_time.m == alarm_time.m)
        set_mode(ALARM);
    }
  }

  for(int i = 0; i < UNIT_ARR_SIZE; i++){
    //handle stopwatches
    if (stopwatches[i].is_launched)
      increase_time(stopwatches[i].time, 99);
    //handle timers
    if (timers[i].is_launched){
      decrease_time(timers[i].time);
      Time t = timers[i].time;
      if ((t.h + t.m + t.s) == 0 && current_mode != ALARM){
        timers[i].is_expired = true;
        timers[i].is_launched = false;
        set_mode(TIMER_EXPIRED);
        call_display_update();
      }        
    }
  }

  if (pomodoro.is_timer_launched){
    decrease_time(pomodoro.current_time);
    Time t = pomodoro.current_time;
    if((t.h + t.m + t.s) == 0 && current_mode != ALARM && current_mode != TIMER_EXPIRED){
      switch_pomodoro_stage(false);       
      set_mode(POMODORO);
      if (settings.p15_pomodoro_signal_duration > 0)
        switch_sound(true, settings.p15_pomodoro_signal_duration * 1000);
    }   
  }

  if ( ((current_mode == STOPWATCH || current_mode == STOPWATCH_SELECT) && stopwatches[current_stopwatch].is_launched) ||
    ((current_mode == TIMER || current_mode == TIMER_SELECT) && timers[current_timer].is_launched) ||
    ((current_mode == POMODORO || current_mode == POMODORO_TWEAKTIME) && pomodoro.is_timer_launched)  )  
      call_display_update();

  if (is_alarm_snooze && is_alarm_active){
    if (alarm_snooze_counter > 0)
      alarm_snooze_counter --;
    else {
      is_alarm_snooze = false;
      set_mode(ALARM);
    }
  }

  if (current_mode == ALARM && settings.p12_alarm_duration > 0){
    if (alarm_off_counter > 0)
      alarm_off_counter --;
    else
      set_mode(CLOCK);
  }

}