 
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
    tone(5, BUZZER_FREQ, BUZZER_DURATION);
  
}
