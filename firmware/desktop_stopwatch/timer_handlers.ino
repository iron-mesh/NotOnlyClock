 
void handle_timers(){

  if (btn_handler_timer.tick()) handle_buttons();

  if (wpage_timer.tick()){
    if (current_weat_page == 2) 
      current_weat_page = 0;
    else
      current_weat_page = current_weat_page + 1;
    call_display_update();
  }

  if (display_blink_timer.tick()) blink_display();
  
}
