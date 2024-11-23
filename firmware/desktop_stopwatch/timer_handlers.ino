 
void handle_timers(){

  if (btn_handler_timer.tick()) 
    handle_buttons();

  if (wpage_timer.tick())
    change_wpage(1, false);

  if (weather_update_timer.tick())
    call_display_update();  

  if (display_blink_timer.tick()) 
    blink_display();

  if(buzzer_timer.tick())
    tone(BUZZER_PIN, BUZZER_FREQ, BUZZER_DURATION);

  if(dot_blink_timer.tick()){
    bool do_update = false;
    for(int i = 0; i < UNIT_ARR_SIZE; i++){
      if (current_mode == STOPWATCH){
        if (i == current_stopwatch) continue;
        if (stopwatches[i].is_launched) {
          do_update = true;
          break;
        }
      } else {
        if (i == current_timer) continue;
        if (timers[i].is_launched) {
          do_update = true;
          break;
        }
      }
    }

    if (do_update){
      show_active_units_dots = !show_active_units_dots;
      call_display_update();
      DEBUG_PRINTLN(F("do update"));
    }    
  }  
}
