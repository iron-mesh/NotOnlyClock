
bool ask_questions(char *msg){
  bool answer = false;
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

  Modes m = current_mode;

  if (is_display_on && m == CLOCK && button1.holding() && button2.click()){    
      switch_display(false);
      return ;    
  }

  if (!is_display_on && (button1.click() || button2.click() || button3.click())){
    switch_display(true);
    return ;
  }

  if (!is_display_on) return ;  
  
                                           
  if (m == CLOCK && button1.holding() && button2.holding() && button3.holding()){    
    reset_buttons();
    edit_settings();
    apply_settings();
    save_settings();
  }   

  if (m <= CLOCK && button1.click()){
    set_next_mode();
    button1.reset();
  } 
  

  bool do_calc = false;
  char operation;  

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
        reset_buttons();
        call_display_update();
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
        in_de_crease_value(clock_time.h, 0, 23, '+');
        call_display_update();}

      if (button3.click() || button3.step()){
        in_de_crease_value(clock_time.m, 0, 59, '+');
        call_display_update();}
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
        in_de_crease_value(alarm_time.h, 0, 23, '+');
        call_display_update();}

      if (button3.click() || button3.step()){
        in_de_crease_value(alarm_time.m, 0, 59, '+');
        call_display_update();}
    break;

    case STOPWATCH:

      if (button1.hold()){
        set_mode(STOPWATCH_SELECT);
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
      if (button1.click()){
        set_mode(STOPWATCH);
      }

      if (button2.release()){
        in_de_crease_value(current_stopwatch, 0, UNIT_ARR_SIZE - 1, '-');
        call_display_update();}

      if (button3.release()){
        in_de_crease_value(current_stopwatch, 0, UNIT_ARR_SIZE - 1, '+');
        call_display_update();}
    break;

    case TIMER:
      if (button1.hold()){
        set_mode(TIMER_SELECT);
      }

      if (button2.holding() && button3.click()){ 
        set_mode(TIMER_TUNE);
        reset_buttons();
      }

      if (button3.release()){
        Time t = timers[current_timer].time;
        timers[current_timer].is_launched = !timers[current_timer].is_launched && (t.h + t.m + t.s) > 0;
      }

      if (button2.release() && !timers[current_timer].is_launched){
          timers[current_timer].time.h = timers[current_timer].start_time.h;
          timers[current_timer].time.m = timers[current_timer].start_time.m;
          timers[current_timer].time.s = timers[current_timer].start_time.s;
          call_display_update();
      }
    break;

    case TIMER_SELECT:
      if (button1.click()){        
        set_mode(TIMER);
      }        

      if (button2.release()){
        in_de_crease_value(current_timer, 0, UNIT_ARR_SIZE - 1, '-');
        call_display_update();}

      if (button3.release()){
        in_de_crease_value(current_timer, 0, UNIT_ARR_SIZE - 1, '+');
        call_display_update();}
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
      }      
      if (button3.click() || button3.step()){
        switch(blinking_zone){
          case 0:
            in_de_crease_value(timers[current_timer].start_time.h, 0, 99, '+');
          break;
          case 3:
            in_de_crease_value(timers[current_timer].start_time.m, 0, 59, '+');
          break;
          case 6:
            in_de_crease_value(timers[current_timer].start_time.s, 0, 59, '+');
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
      if (button1.hold()){
        set_mode(COUNTER_SELECT);
      }

      if (button2.holding() && button3.holding()){
        if (ask_questions("reset"))
          counter_mode_values[current_counter] = 0;      
      }
      
      if(button2.release()){
        in_de_crease_value(counter_mode_values[current_counter], -99999, 999999, '-');
        call_display_update();}

      if(button3.release()){
        in_de_crease_value(counter_mode_values[current_counter], -99999, 999999, '+');
        call_display_update();}
    break;

    case COUNTER_SELECT:
      if (button1.click()){
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

    case POMODORO:  

      if (button2.holding() && button3.click()){ 
        set_mode(POMODORO_SETTINGS);
        reset_buttons();
      }

      if (button2.holding() && button3.holding()){
        if (ask_questions("reset"))
          apply_pomodoro_settings();   
      }

      if (button1.hold())
        set_mode(POMODORO_TWEAKTIME);

      if(button2.release()){
        Pomodoro::Stages st = pomodoro.pomodoro_stage;
        if (st == Pomodoro::Stages::POMODORO_STAGE){
          if (pomodoro.is_timer_launched)
            pomodoro.current_time = convert_minutes_to_time(pomodoro.settings.pomodoro_time);
          else
            switch_pomodoro_stage(true);
        } else {
          switch_pomodoro_stage(true);
        }
        call_display_update();
      }

      if(button3.release())
        pomodoro.is_timer_launched = !pomodoro.is_timer_launched;
    break;

    case POMODORO_SETTINGS:
      
      if (button1.hold()){
        set_mode(POMODORO);
      }        
      
      if (button1.click()){ 
        int value = (int)pomodoro.tune_page;            
        in_de_crease_value(value, 0, 5, '+');
        pomodoro.tune_page = (Pomodoro::TunePages)value;
        call_display_update();
      }
      
      if (button2.click() || button2.step()){ 
        do_calc = true;
        operation = '-';
      }

      if (button3.click() || button3.step()){ 
        do_calc = true;
        operation = '+';
      }

      if(do_calc){
        switch(pomodoro.tune_page){
          case Pomodoro::TunePages::POMODORO_TIME:
            in_de_crease_value(pomodoro.settings.pomodoro_time, 1, 999, operation);
          break;
          case Pomodoro::TunePages::CHILL_TIME:
            in_de_crease_value(pomodoro.settings.chill_time, 1, 999, operation);
          break;
          case Pomodoro::TunePages::POMODORO_COUNT:
            in_de_crease_value(pomodoro.settings.pomodoro_count, 1, 99, operation);
          break;
          case Pomodoro::TunePages::BIG_CHILL_TIME:
            in_de_crease_value(pomodoro.settings.big_chill_time, 1, 999, operation);
          break;
          case Pomodoro::TunePages::POMODORO_AUTO_START:
            in_de_crease_value(pomodoro.settings.pomodoro_auto_start, 0, 1, operation);
          break;
          case Pomodoro::TunePages::CHILL_AUTO_START:
            in_de_crease_value(pomodoro.settings.chill_auto_start, 0, 1, operation);
          break;
        }
      call_display_update();
      }
    break;

    case POMODORO_TWEAKTIME:  

      if (button1.click())
        set_mode(POMODORO);

      if(button2.click() || button2.step()){
        do_calc = true;
        operation = '-';        
      }

      if(button3.click() || button3.step()){
        do_calc = true;
        operation = '+';   
      }

      if (do_calc){
        uint16_t mins = 60 * pomodoro.current_time.h + pomodoro.current_time.m;        
        in_de_crease_value(mins, 1, 999, operation);
        uint8_t secs = pomodoro.current_time.s;
        pomodoro.current_time = convert_minutes_to_time(mins);
        pomodoro.current_time.s = secs;
        call_display_update();
      }
    break;       
  }
}

