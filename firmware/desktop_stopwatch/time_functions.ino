
void increase_sw_time(){
  sw_sec++;
  if (sw_sec > 59){
    sw_sec = 0;
    sw_min ++;
    if (sw_min > 59){
      sw_min = 0;
      sw_hour ++;
      if (sw_hour > 99){
        sw_sec = 0;
        sw_min = 0;
        sw_hour = 0;
      }
    }
  } 
}

ISR(TIMER1_A){
  if (is_stopwatch_launched){
    increase_sw_time();
    if(current_mode == STOPWATCH)
      call_display_update();
  }
}