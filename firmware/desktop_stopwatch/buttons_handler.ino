void handle_buttons(){
  button1.tick();
  button2.tick();
  button3.tick();

  if (is_display_on && button1.pressing() && button2.release()){  
    button1.reset();
    button2.reset();
    switch_display(false);
    delay(2000);    
    return ;
  }

  if (!is_display_on && (button1.release() || button2.release() || button3.release())){
    switch_display(true);
    return ;
  }

  if (!is_display_on) return ;

  if (button1.release()){
    set_next_mode();
    button1.reset();
  }

  switch(current_mode){
    case STOPWATCH:
      if (button3.release()){
      if (is_stopwatch_launched){
        is_stopwatch_launched = false;
      }
      else{
        is_stopwatch_launched = true;
        call_display_update();
      }
    }

    if (button2.release()){
      if (!is_stopwatch_launched){
        sw_hour = 0;
        sw_min = 0;
        sw_sec = 0;
        call_display_update();
      }      
    }
    break;

    case WEATHER:
      if (button2.release()){
        if (current_weat_page == 0)
          current_weat_page = 2;
        else
          current_weat_page = current_weat_page - 1;
        wpage_timer.start();
        call_display_update();
      } 
    
      if (button3.release()){
        wpage_timer.force();
      }
    break;


  }

}
