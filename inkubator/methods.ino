/*----------------------serial-initialization------------------------------------*/
void serial_initial() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

/*----------------------LCD-initialization------------------------------------*/
void lcd_initial() {
  lcd.begin(20,4);         // initialize the lcd for 16 chars 2 lines, turn on backlight

  lcd.setCursor(7,1); 
  lcd.print("Welcome"); // print text to lcd
  delay(2000);

  lcd.clear();
  lcd.setCursor(3,1); 
  lcd.print("Inicialization"); 
  delay(250);
  lcd.setCursor(6,2); 
  lcd.print("running");
}

/*----------------------LCD-print_end_of_init------------------------------------*/
void lcd_print_end_of_init() {

  lcd.clear();
  lcd.setCursor(3,1); 
  lcd.print("Inicialization"); 
  delay(250);
  lcd.setCursor(8,2); 
  lcd.print("done");
  delay(250);
}

/*----------------------pins-initialization------------------------------------*/
void pins_initial() {
  pinMode(cs_pin, OUTPUT);
  pinMode(led_indication, OUTPUT);
  pinMode(relay_humidity, OUTPUT);
  pinMode(relay_heater, OUTPUT);

}

/*----------------------init_outputs_set------------------------------------*/
void init_outputs_set() {
  digitalWrite(relay_humidity, LOW);
  digitalWrite(relay_heater, LOW);
  digitalWrite(led_indication, LOW);
  delay(100);
}

/*-----------------------Sensors-initialization--------------------------------*/
void sensors_initial(){
  dht.begin(); // humidity sensore - DHT22
}

/*----------------------bootup_led_indication------------------------------------*/
void bootup_led_indication() {
  for (int temp = 0;temp < 5; temp++) {
    digitalWrite(led_indication, HIGH);
    delay(100);
    digitalWrite(led_indication, LOW);
    delay(100);
  }
    digitalWrite(led_indication, LOW);
    delay(100);
}

/*--------------------------timer1-initialization------------------------------------------*/
void timer0_initial() {
  cli();  // disable all interrupts

  OCR0A = 50;                                         // compareregister 16MHz/64/5kHz = 50 (max 255)
  TCCR0B |= (1 << CS00) | (1 << CS01) | (0 << CS02);  // 64 prescaler
  TIMSK0 |= (1 << OCIE0A);                            // enable timer interrupt ( interrupt is active- when is value                           // enable timer interrupt ( interrupt is active- when is value of TCNT0 (value of counter) in match with OCR0A)

  sei();  // enable all interrupts
}

/*--------------------------interupt_function_timer_0----------------*/
ISR(TIMER0_COMPA_vect) {
  timer0_counter = timer0_counter + 1;
  if (timer0_counter == timer0_4s_count) {
    measure_flag = 1;
    timer0_counter = 0;
  }
}

/*------------------------------ sd_card_initial ------------------------------------*/
void sd_card_initial() {
  String message = "";

  while (!SD.begin(cs_pin)) { 
    Serial.println("SD_card_no_detected!");
    
    lcd.clear();
    lcd.setCursor(3,1); 
    lcd.print("Inicialization"); 
    delay(250);
    lcd.setCursor(6,2); 
    lcd.print("running");
    lcd.setCursor(0,3); 
    lcd.print("SD_card_no_detected!");
    
    digitalWrite(led_indication, HIGH);
    delay(500);
    digitalWrite(led_indication, LOW);
    delay(500);
  }

  if (SD.exists(file_name)) {
    Serial.println("soubor DATA.txt detected");
    } else {
        soubor = SD.open(file_name , FILE_WRITE);
        soubor.println("-----------------------------");
        soubor.println("-----------------------------");
        while (!soubor){
          Serial.println("error_SD_card!");
          
          lcd.clear();
          lcd.setCursor(3,1); 
          lcd.print("Inicialization"); 
          delay(250);
          lcd.setCursor(6,2); 
          lcd.print("running");
          lcd.setCursor(2,3); 
          lcd.print("error_SD_card!");
          
          digitalWrite(led_indication, HIGH);
          delay(500);
          digitalWrite(led_indication, LOW);
          delay(500);
        }

        soubor.println("air_temperatur[°C];  humidity[%];  watter_level[-];  heater[-];  humidifier[-];");    
        soubor.println("-----------------------------");
        soubor.println("-----------------------------");
        soubor.close();
        } 

  delay(250);            
}

/*----------------------watter_level_measure------------------------------------*/
void watter_level_measure() {
  int voltage_temp = 0;

  for (int temp =0;temp < measure_count; temp++) {
    voltage_temp = voltage_temp + analogRead(watter_level_mess_pin);
  }

  watter_level = voltage_temp / measure_count;
  //Serial.println(watter_level);
}

/*-----------------------reading-data-from-DHT22--------------------------------*/
void reading_data_DHT22(){
  humidity = dht.readHumidity();
  air_temp = dht.readTemperature();

  check_reads_data_DHT22();
  error_detection();
}

/*-----------------------Check if any reads failed and exit early--------------------------------*/
void check_reads_data_DHT22(){
  if (isnan(humidity)) {
    humidity = 9999;
    return;
  }
  if (isnan(air_temp)) {
    air_temp = 9999;
    return;
  }
}

/*-------------------------- error_detection  -------------------------*/
void error_detection() {
  if (humidity == 9999) {
    humidity_error_flag = 1;
  } else {
    humidity_error_flag = 0;
  }

  if (air_temp == 9999 ) {
    air_temp_error_flag = 1;
  } else {
    air_temp_error_flag = 0;
  }
}

/*------------------------ write_to_SD ---------------------------------------*/
void write_to_sd() {
  String message = "";
  
  soubor = SD.open(file_name, FILE_WRITE); 
  while (!soubor){
    Serial.println("error_SD_card!");
    digitalWrite(led_indication, HIGH);
    delay(200);
    digitalWrite(led_indication, LOW);
    delay(200);
    }

  if (air_temp_error_flag == 1) {
    soubor.print("err;");
  } else {
    message = String(air_temp) + ";";
    soubor.print(message);
  }

  if (humidity_error_flag == 1) {
    soubor.print("err;");
  } else {
    message = String(humidity) + ";";
    soubor.print(message);
  }
  

  message = String(watter_level) + ";" + String(heater_on_flag) + ";" + String(humidity_on_flag) + ";"; 
  soubor.println(message);
  soubor.close();
  delay(250);
}

/*------------------------ serial_output ---------------------------------------*/
void serial_output() {
  String message = "";

  Serial.println("-----------------------------");
  
  if (air_temp_error_flag == 1) {
    Serial.print("err;");
  } else {
    message = String(air_temp) + ";";
    Serial.print(message);
  }

  if (humidity_error_flag == 1) {
    Serial.print("err;");
  } else {
    message = String(humidity) + ";";
    Serial.print(message);
  }
  

  message = String(watter_level) + ";" + String(heater_on_flag) + ";" + String(humidity_on_flag) + ";"; 
  Serial.println(message);
  Serial.println("-----------------------------"); 

  delay(250);
}

/*-------------------------- control_heater_output  -------------------------*/
void control_heater_output() {

  if (air_temp_error_flag == 0) {
    if ((air_temp < air_temp_min) && (heater_on_flag == 0)) {
      heater_on_flag = 1;
      digitalWrite(relay_heater, HIGH);
      delay(100);
    } else if ((air_temp >= air_temp_max) && (heater_on_flag == 1)) {
        heater_on_flag = 0;
        digitalWrite(relay_heater, LOW);
        delay(100);
    }
  } else if (heater_on_flag == 1) {
      heater_on_flag = 0;
      digitalWrite(relay_heater, LOW);
      delay(100);
  }
}

/*-------------------------- control_humidity_output  -------------------------*/
void control_humidity_output() {

  if (humidity_error_flag == 0) {
    if ((humidity < humidity_min ) && (watter_level < watter_level_full) && (humidity_on_flag == 0)) {
      humidity_on_flag = 1;
      digitalWrite(relay_humidity, HIGH);
      delay(100);
    } else if (((humidity > humidity_max) && (humidity_on_flag == 1)) || (watter_level > watter_level_none)) {
        humidity_on_flag = 0;
        digitalWrite(relay_humidity, LOW);
        delay(100);
    }
  } else if (humidity_on_flag == 1) {
      heater_on_flag = 0;
      digitalWrite(relay_humidity, LOW);
      delay(100);
  }
}

/*----------------------lcd_print------------------------------------*/
void lcd_print() {

  lcd.clear();

  if (air_temp_error_flag == 1) {
    lcd.setCursor(0,0); 
    lcd.print("tep:error");  
  } else {
    lcd.setCursor(0,0); 
    lcd.print("tep:"); 
    lcd.setCursor(5,0); 
    lcd.print(air_temp);
    lcd.setCursor(11,0); 
    lcd.print("C");
  }
  
  lcd.setCursor(14,0); 
  lcd.print("Rt:"); 
  lcd.setCursor(19,0); 
  lcd.print(heater_on_flag);

  
  if (humidity_error_flag == 1) {
    lcd.setCursor(0,1); 
    lcd.print("hum:error");  
  } else {
    lcd.setCursor(0,1); 
    lcd.print("hum:"); 
    lcd.setCursor(5,1); 
    lcd.print(humidity);
    lcd.setCursor(11,1); 
    lcd.print("%");
  }

  lcd.setCursor(14,1); 
  lcd.print("Rh:"); 
  lcd.setCursor(19,1); 
  lcd.print(humidity_on_flag);

  lcd.setCursor(0,2); 
  lcd.print("Wat_lev:"); 
  lcd.setCursor(9,2);
  if (watter_level < watter_level_full) {
    lcd.print("FULL");
  } else if (watter_level > watter_level_none) {
      lcd.print("NONE");
  }
  
}

/*-------------------------- led_measure_indicator_set -------------------------*/
void led_measure_indicator_set() {
  if (led_measure_indication_flag == 0) {
    digitalWrite(led_indication, HIGH);
    led_measure_indication_flag = 1;
  } else {
    digitalWrite(led_indication, LOW);
    led_measure_indication_flag = 0;
  }
}
