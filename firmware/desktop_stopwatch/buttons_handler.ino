
bool ask_questions(char *msg){
  bool answer = false;
  reset_buttons();
  display_text(msg);
  while(1){
    if (!(button1.tick() || button3.tick())) continue;

    if (button1.click())
      break;

    if (button3.click()){
      answer = true;
      break;
    }      
  }
  reset_buttons();
  call_display_update();
  return answer;
}

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
  if(m == CLOCK || m == STOPWATCH || m == TIMER || m == WEATHER || m == COUNTER){    
    if (button1.holding() && button2.holding() && button3.holding()){    
      reset_buttons();
      edit_settings();
      apply_settings();
      save_settings();
    }   

    if (button1.click()){
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
        if (!is_alarm_snooze){
          is_alarm_active = !is_alarm_active;
          if (is_alarm_active)
            display_text("AL ON");
          else{
            display_text("AL OFF");
            is_alarm_snooze = false;
          }
          save_alarm();           
        } else {
          is_alarm_snooze = false;
          display_text("SN OFF");
        }
        delay(2000); 
        call_display_update();
        return;
      }
    break;

    case CLOCK_TUNE:
      if (button1.release()){
        set_mode(CLOCK);
        button1.reset();       
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
        button1.reset();      
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

      if (button1.pressFor(1000) && button1.release()){
        set_mode(STOPWATCH_SELECT);
        button1.reset();
      }

      if (button3.release()){
        stopwatches[current_stopwatch].is_launched = !stopwatches[current_stopwatch].is_launched;
        call_display_update();
      }

      if (button2.release() && !stopwatches[current_stopwatch].is_launched){        
          stopwatches[current_stopwatch].time.h = 0;
          stopwatches[current_stopwatch].time.m = 0;
          stopwatches[current_stopwatch].time.s = 0;
          call_display_update();            
      }
    break;

    case STOPWATCH_SELECT:
      if (button1.release()){
        set_mode(STOPWATCH);
        button1.reset();
      }

      if (button2.release()){
        current_stopwatch = (current_stopwatch > 0) ? current_stopwatch - 1 : UNIT_ARR_SIZE - 1;
        call_display_update();
      }

      if (button3.release()){
        current_stopwatch = (current_stopwatch < (UNIT_ARR_SIZE - 1)) ? current_stopwatch + 1 : 0;
        call_display_update();
      }
    break;

    case TIMER:
      if (button1.pressFor(1000) && button1.release()){
        set_mode(TIMER_SELECT);
        button1.reset();
      }

      if (button2.holding() && button3.click()){ 
        set_mode(TIMER_TUNE);
        reset_buttons();
      }
      if (button3.release()){
        Time t = timers[current_timer].time;
        if (!timers[current_timer].is_launched && (t.h + t.m + t.s) > 0){
          timers[current_timer].is_launched = true;
        }else{
          timers[current_timer].is_launched = false;          
        }
        call_display_update();
      }

      if (button2.release() && !timers[current_timer].is_launched){
          timers[current_timer].time.h = timers[current_timer].start_time.h;
          timers[current_timer].time.m = timers[current_timer].start_time.m;
          timers[current_timer].time.s = timers[current_timer].start_time.s;
          call_display_update();
      }
    break;

    case TIMER_SELECT:
      if (button1.release()){        
        set_mode(TIMER);
        button1.reset();
      }        

      if (button2.release()){
        current_timer = (current_timer > 0) ? current_timer - 1 : UNIT_ARR_SIZE - 1;
        call_display_update();
      }

      if (button3.release()){
        current_timer = (current_timer < (UNIT_ARR_SIZE - 1)) ? current_timer + 1 : 0;
        call_display_update();
      }
    break;

    case TIMER_TUNE:
      if (button1.release()){
        set_mode(TIMER);
        button1.reset();  
      }      
      if (button2.holding() && button3.click()){
        timers[current_timer].start_time.h = 0;
        timers[current_timer].start_time.m = 0;
        timers[current_timer].start_time.s = 0;
        reset_buttons();
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
            if (timers[current_timer].start_time.h >= 99) 
              timers[current_timer].start_time.h = 0;
            else 
              timers[current_timer].start_time.h ++;            
          break;
          case 3:
            if (timers[current_timer].start_time.m >= 59) 
              timers[current_timer].start_time.m = 0;
            else 
              timers[current_timer].start_time.m ++;            
          break;
          case 6:
            if (timers[current_timer].start_time.s >= 59) 
              timers[current_timer].start_time.s = 0;
            else 
              timers[current_timer].start_time.s ++;            
          break;
        }
        call_display_update();
      }    
    break;

    case TIMER_EXPIRED:
      if(button1.release() || button2.release() || button3.release()){
        set_mode(TIMER);
        reset_buttons();
      }
    break;

    case WEATHER:
      if (button2.release())
        change_wpage(-1, true);
    
      if (button3.release())
        change_wpage(1, true);
    break;

    case ALARM:
      if (button1.release() || button2.release() || button3.release()){        
        if(settings.p13_snooze_duration > 0){
          alarm_snooze_counter = 60 * settings.p13_snooze_duration;
          is_alarm_snooze = true;
          set_mode(CLOCK);
          display_text("SNOO2E");
          delay(2000);
        }
      }

      if (button2.holding() && button3.holding()){ 
        reset_buttons();
        set_mode(CLOCK);
        display_text("9OOd dA4");
        delay(2000);
      }        
    break;
    
    case COUNTER:
      if (button1.pressFor(1000) && button1.release()){
        set_mode(COUNTER_SELECT);
        button1.reset();
      }

      if (counter_mode_values[current_counter] != 0 && button2.pressing() && button3.release()){
        if (ask_questions("reset"))
          counter_mode_values[current_counter] = 0;      
      }
      
      if(button2.release()){
        if (counter_mode_values[current_counter] > -99999)
          counter_mode_values[current_counter] --;
        else
          counter_mode_values[current_counter] = 0;
        call_display_update();
      }

      if(button3.release()){
        if (counter_mode_values[current_counter] < 999999)
          counter_mode_values[current_counter] ++;
        else
          counter_mode_values[current_counter] = 0;
        call_display_update();
      }
    break;

    case COUNTER_SELECT:
      if (button1.release()){
        set_mode(COUNTER);
        button1.reset();
      }

      if (button2.release()){
        current_counter = (current_counter > 0) ? current_counter - 1 : 8;
        call_display_update();
      }

      if (button3.release()){
        current_counter = (current_counter < (8)) ? current_counter + 1 : 0;
        call_display_update();
      }
    break;   

  }

}

