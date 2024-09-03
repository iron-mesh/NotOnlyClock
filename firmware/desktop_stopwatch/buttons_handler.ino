void init_buttons(){
  button1.setStepTimeout(BTN_STEP_TIMEOUT);
  button2.setStepTimeout(BTN_STEP_TIMEOUT);
  button3.setStepTimeout(BTN_STEP_TIMEOUT);
}

void handle_buttons(){
  if (!(button1.tick() || button2.tick() || button3.tick()) ) return;

  if (is_display_on && button1.holding() && button2.click()){    
      switch_display(false);
      return ;    
  }

  if (!is_display_on && (button1.click() || button2.click() || button3.click())){
    switch_display(true);
    return ;
  }

  if (!is_display_on) return ;

  if (button1.release()){
    Modes m = current_mode;
    if(m == CLOCK || m == STOPWATCH || m == TIMER || m == WEATHER){
      set_next_mode();
      button1.reset();
    }    
  }

  switch(current_mode){
    case CLOCK:
      if (button2.holding() && button3.click()){ 
      set_display_blinking(true);
      current_mode = CLOCK_TUNE;
      clock_time.s = 0;
      return ;
      }
    break;

    case CLOCK_TUNE:
      if (button1.release()){
        current_mode = CLOCK;        
        set_display_blinking(false);        
      }    
      if (button2.click() || button2.step()){
        if (clock_time.h >= 23) clock_time.h = 0; 
        else clock_time.h ++;
        call_display_update();
      }      
      if (button3.click() || button3.step()){
        if (clock_time.m >= 59) clock_time.m = 0;
        else clock_time.m ++;
        call_display_update();
      }
    break;

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
        sw_time.h = 0;
        sw_time.m = 0;
        sw_time.s = 0;
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
