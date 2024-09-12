
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

ISR(TIMER1_A){
  if (is_stopwatch_launched){
    increase_time(sw_time, 99);
    if (current_mode == STOPWATCH) call_display_update();
  }

  if (current_mode != CLOCK_TUNE){
    uint8_t prev_min = clock_time.m;
    increase_time(clock_time, 23);
    if ((current_mode == CLOCK) && (settings.p5_show_seconds_clock || prev_min != clock_time.m))
        call_display_update();    
  }

  if (is_timer_launched){
    decrease_time(timer_time);
    if ((timer_time.h + timer_time.m + timer_time.s) == 0) set_mode(TIMER_EXPIRED);
    if (current_mode == TIMER) call_display_update();
  }

}