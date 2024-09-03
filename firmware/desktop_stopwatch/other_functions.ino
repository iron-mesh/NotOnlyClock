

void set_next_mode(){
  switch(current_mode){
    case CLOCK:
      current_mode = STOPWATCH;
    break;
    case STOPWATCH:
      current_mode = WEATHER;
      wpage_timer.start();
    break;
    case WEATHER:
      current_mode = CLOCK;
      wpage_timer.stop();
    break;
  }
  call_display_update();
}

String get_weather_param_str(WeatherUnit param){
  float value = NAN;
  String str;

  if (param == TEMPERATURE){
      BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
      value = bme.temp(tempUnit);
      str = String("t ");
      str.concat(String(value, 2));        
  } else if(param == HUMIDITY) {
      value = bme.hum();      
      str = String("h ");
      str.concat(String(value, 2));  
  }else if(param == PRESSURE){
      BME280::PresUnit presUnit(BME280::PresUnit_Pa);
      value = bme.pres(presUnit);
      value = value / 133.32;      
      str = String("p ");
      str.concat(String(value, 2));
  }
  return str;

}

