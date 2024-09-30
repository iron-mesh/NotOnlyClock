void init_buttons(){
  button1.setStepTimeout(BTN_STEP_TIMEOUT);
  button2.setStepTimeout(BTN_STEP_TIMEOUT);
  button3.setStepTimeout(BTN_STEP_TIMEOUT);
}

void reset_buttons(){
  button1.reset();
  button2.reset();
  button3.reset(); 
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
  
  Modes m = current_mode;
  if(m == CLOCK || m == STOPWATCH || m == TIMER || m == WEATHER){    
    if (button1.holding() && button2.holding() && button3.holding()){    
      reset_buttons();
      edit_settings();
      apply_settings();
      save_settings();
    }   

    if (button1.release()){
      set_next_mode();
      button1.reset();
    } 
  }  

  switch(current_mode){
    case CLOCK:
      if (button2.holding() && button3.click()){ 
      set_mode(CLOCK_TUNE);
      return ;
      }
      if (button3.holding() && button2.click()){ 
      set_mode(ALARM_TUNE);
      return ;
      }
      if (button2.holding() && button3.holding()){
        is_alarm_active = !is_alarm_active;
        if (is_alarm_active)
          display_text("AL ON");
        else
          display_text("AL OFF");
        delay(2000); 
        save_alarm(); 
        call_display_update();
        return;
      }
    break;

    case CLOCK_TUNE:
      if (button1.release()){
        set_mode(CLOCK);       
      }
      if (button2.holding() && button3.click()){
        clock_time.h = 0;
        clock_time.m = 0;
        reset_buttons();
        call_display_update();
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

    case ALARM_TUNE:
      if (button1.release()){
        set_mode(CLOCK);       
      }
      if (button3.holding() && button2.click()){
        alarm_time.h = 0;
        alarm_time.m = 0;
        reset_buttons();
        call_display_update();
      }    
      if (button2.click() || button2.step()){
        if (alarm_time.h >= 23) alarm_time.h = 0; 
        else alarm_time.h ++;
        call_display_update();
      }      
      if (button3.click() || button3.step()){
        if (alarm_time.m >= 59) alarm_time.m = 0;
        else alarm_time.m ++;
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

    case TIMER:
      if (button2.holding() && button3.click()){ 
        set_mode(TIMER_TUNE);
        return ;
      }
      if (button3.release()){
        if (!is_timer_launched && (timer_time.h + timer_time.m + timer_time.s) > 0){
          is_timer_launched = true;
        }else{
          is_timer_launched = false;          
        }
        call_display_update();
      }

      if (button2.release() && !is_timer_launched){
          timer_time.h = timer_start_time.h;
          timer_time.m = timer_start_time.m;
          timer_time.s = timer_start_time.s;
          call_display_update();
      }
    break;

    case TIMER_TUNE:
      if (button1.release()){
        set_mode(TIMER);       
      }      
      if (button2.holding() && button3.click()){
        timer_start_time.h = 0;
        timer_start_time.m = 0;
        timer_start_time.s = 0;
        button3.reset();
        call_display_update();
      }
      if (button2.click()){
        if (blinking_zone < 6)
          blinking_zone += 3;
        else
          blinking_zone = 0;
        call_display_update();
      }      
      if (button3.click() || button3.step()){
        switch(blinking_zone){
          case 0:
            if (timer_start_time.h >= 99) 
              timer_start_time.h = 0;
            else 
              timer_start_time.h ++;            
          break;
          case 3:
            if (timer_start_time.m >= 59) 
              timer_start_time.m = 0;
            else 
              timer_start_time.m ++;            
          break;
          case 6:
            if (timer_start_time.s >= 59) 
              timer_start_time.s = 0;
            else 
              timer_start_time.s ++;            
          break;
        }
        call_display_update();
      }    
    break;

    case TIMER_EXPIRED:
      if(button1.release() || button2.release() || button3.release())
        set_mode(TIMER);
    break;

    case WEATHER:
      if (button2.release())
        change_wpage(-1, true);
    
      if (button3.release()){
        change_wpage(1, true);
    break;

    case ALARM:
      if(button2.release())
        set_mode(CLOCK);
    break;

  }

}
}
