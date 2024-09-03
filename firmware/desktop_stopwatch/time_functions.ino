
void increase_time(Time &time, const int8_t inc_val, const uint8_t max_hour){
  time.s += inc_val;
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

ISR(TIMER1_A){
  if (is_stopwatch_launched){
    increase_time(sw_time, 1, 99);
    if(current_mode == STOPWATCH)
      call_display_update();
  }

  if (current_mode != CLOCK_TUNE){
    increase_time(clock_time, 1, 23);
    if(current_mode == CLOCK) call_display_update();
  }
}