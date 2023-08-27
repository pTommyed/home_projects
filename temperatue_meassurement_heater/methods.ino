/*----------------------serial-initialization------------------------------------*/
void serial_initial() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

/*----------------------LCD-initialization------------------------------------*/
void lcd_initial() {
  lcd.begin(16,2);         // initialize the lcd for 16 chars 2 lines, turn on backlight

  lcd.setCursor(4,0); 
  lcd.print("Welcome"); // print text to lcd
  delay(2000);

  lcd.clear();
  lcd.setCursor(1,0); 
  lcd.print("Inicialization"); 
  delay(250);
  lcd.setCursor(5,1); 
  lcd.print("running");
}

/*----------------------LCD-print_end_of_init------------------------------------*/
void lcd_print_end_of_init() {

  lcd.clear();
  lcd.setCursor(1,0); 
  lcd.print("Inicialization"); 
  delay(250);
  lcd.setCursor(6,1); 
  lcd.print("done");
  delay(250);
}

/*-----------------------Sensors-initialization--------------------------------*/
void sensors_initial(){
  sensor_1.begin(); // temperature od water
  sensor_2.begin(); // temperature od water
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
  if (timer0_counter == timer0_10s_count) {
    measure_flag = 1;
    timer0_counter = 0;
  }
}

/*----------------------pins-initialization------------------------------------*/
void pins_initial() {
  pinMode(led_measure_indication, OUTPUT);

  pinMode(relay_1_boiler_pin, OUTPUT);
  pinMode(relay_2_kotel_pin, OUTPUT);
}

/*----------------------init_relay_set------------------------------------*/
void init_relay_set() {
  digitalWrite(relay_1_boiler_pin, LOW);
  delay(100);
  digitalWrite(relay_2_kotel_pin, LOW);
  delay(100);
}

/*----------------------bootup_led_indication------------------------------------*/
void bootup_led_indication() {
  for (int temp = 0;temp < 5; temp++) {
    digitalWrite(led_measure_indication, HIGH);
    delay(100);
    digitalWrite(led_measure_indication, LOW);
    delay(100);
  }
    digitalWrite(led_measure_indication, LOW);
    led_measure_indication_flag = 0;
    delay(100);
}

/*-----------------------Sensors-requesting-temperature--------------------------------*/
void sensors_request(){
  sensor_1.requestTemperatures();
  sensor_2.requestTemperatures();
}

/*-----------------------Sensors-get-temperature--------------------------------*/
void sensors_get_temp(){
  temp_boiler=sensor_1.getTempCByIndex(0);
  temp_kotel=sensor_2.getTempCByIndex(0);
}

/*-------------------------- measure_temperatures  -------------------------*/
void measure_temperatures() {
  sensors_request();
  sensors_get_temp();
}

/*-------------------------- error_detection  -------------------------*/
void error_detection() {
  if (temp_boiler == -127) {
    temp_boiler_error_flag = 1;
  } else {
    temp_boiler_error_flag = 0;
  }

  if (temp_kotel == -127 ) {
    temp_kotel_error_flag = 1;
  } else {
    temp_kotel_error_flag = 0;
  }
}

/*-------------------------- control_boiler_output  -------------------------*/
void control_boiler_output() {
  
  float hysterezia_temporary = temp_boiler + hysterezia;  // computing of actual hysterezia value for boiler output setting

  if ((temp_boiler_error_flag == 0) && (temp_kotel_error_flag == 0)) {
    if (temp_kotel >= start_temp_kotel) { 
      if ((temp_kotel > temp_boiler) && (temp_boiler <= max_temperature)){
        relay_1_boiler_flag = 1;
        digitalWrite(relay_1_boiler_pin, HIGH);
        delay(100);

      } else if ((temp_kotel <= hysterezia_temporary) || (temp_boiler >= hysterezia_max_temperature)){
          relay_1_boiler_flag = 0;
          digitalWrite(relay_1_boiler_pin, LOW);
          delay(100);
          }

    } else if (temp_kotel <= hysterezia_temporary) {
          relay_1_boiler_flag = 0;
          digitalWrite(relay_1_boiler_pin, LOW);
          delay(100);
          }  
      
  } else if (relay_1_boiler_flag == 1){
      relay_1_boiler_flag = 0;
      digitalWrite(relay_1_boiler_pin, LOW);
      delay(100);
      }

}

/*-------------------------- control_kotel_output  -------------------------*/
void control_kotel_output() {

  if (temp_kotel_error_flag == 0) {
    if (temp_kotel >= start_temp_kotel) {
      relay_2_kotel_flag = 1;
      digitalWrite(relay_2_kotel_pin, HIGH);
      delay(100); 
      
    } else if (temp_kotel <= hysterezia_temp ) {
        relay_2_kotel_flag = 0;
        digitalWrite(relay_2_kotel_pin, LOW);
        delay(100);
        }  
      
  } else if (relay_2_kotel_flag == 1){
      // let output turn on!
      }
}

/*-----------------------Serial-print-water-temperature--------------------------------*/
void serial_print_temp(){
  
  if (temp_boiler_error_flag == 1) {
    Serial.println("teplota_boiler: error!");
  } else {
    Serial.print("teplota_boiler: ");
    Serial.print(temp_boiler);
    Serial.print(" °C   ###  rele_boiler: ");
    Serial.println (relay_1_boiler_flag);
  }

  if (temp_kotel_error_flag == 1) {
    Serial.println("teplota_kotel: error!");
  } else {
    Serial.print("teplota_kotel: ");
    Serial.print(temp_kotel);
    Serial.print(" °C   ###  rele_kotel: ");
    Serial.println (relay_2_kotel_flag);
  }
  
  Serial.println("---------------------------");
}

/*----------------------lcd_print------------------------------------*/
void lcd_print() {

  lcd.clear();

  if (temp_boiler_error_flag == 1) {
    lcd.setCursor(0,0); 
    lcd.print("T_b:error");  
  } else {
    lcd.setCursor(0,0); 
    lcd.print("T_b:"); 
    lcd.setCursor(5,0); 
    lcd.print(temp_boiler);
  }
  
  lcd.setCursor(11,0); 
  lcd.print("Rb:"); 
  lcd.setCursor(15,0); 
  lcd.print(relay_1_boiler_flag);

  
  if (temp_kotel_error_flag == 1) {
    lcd.setCursor(0,1); 
    lcd.print("T_k:error");  
  } else {
    lcd.setCursor(0,1); 
    lcd.print("T_k:"); 
    lcd.setCursor(5,1); 
    lcd.print(temp_kotel);
  }

  lcd.setCursor(11,1); 
  lcd.print("Rk:"); 
  lcd.setCursor(15,1); 
  lcd.print(relay_2_kotel_flag);
}

/*-------------------------- led_measure_indicator_set -------------------------*/
void led_measure_indicator_set() {
  if (led_measure_indication_flag == 0) {
    digitalWrite(led_measure_indication, HIGH);
    led_measure_indication_flag = 1;
  } else {
    digitalWrite(led_measure_indication, LOW);
    led_measure_indication_flag = 0;
  }
}