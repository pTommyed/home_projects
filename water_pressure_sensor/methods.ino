/*----------------------serial-initialization------------------------------------*/
void serial_initial() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

/*------------------------------ sd_card_initial ------------------------------------*/
void sd_card_initial() {
  // inicializace SD Modulu
  while (!SD.begin(cs_pin)) { 
    delay(2000);
  }

  Soubor = SD.open(file_name , FILE_WRITE);

  if (Soubor) {
    Soubor.print("---------------------- file_initialized _ " + file_name + " ----------------------" + "\r\n");
    Soubor.close();
  } 
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
  if (timer0_counter == timer0_1s_count) {
    measure_flag = 1;
    timer0_counter = 0;
  }
}

/*----------------------pins-initialization------------------------------------*/
void pins_initial() {
  pinMode(cs_pin, OUTPUT);
  pinMode(led_on_indication, OUTPUT);
  pinMode(led_measure_indication, OUTPUT);
}

/*----------------------bootup_led_indication------------------------------------*/
void bootup_led_indication() {
  for (int temp = 0;temp < 5; temp++) {
    digitalWrite(led_measure_indication, HIGH);
    digitalWrite(led_on_indication, HIGH);
    delay(100);
    digitalWrite(led_measure_indication, LOW);
    digitalWrite(led_on_indication, LOW);
    delay(100);
  }
    digitalWrite(led_measure_indication, LOW);
    led_measure_indication_flag = 0;
    digitalWrite(led_on_indication, HIGH);
    delay(100);
}

/*----------------------voltage_measure------------------------------------*/
void voltage_measure() {
  int voltage_temp = 0;
  for (int temp =0;temp < measure_count; temp++) {
    voltage_temp = voltage_temp + analogRead(pressure_pin);
  }

  voltage = voltage_temp / measure_count;
  voltage = voltage * (5.0 / 1023.0);
}

/*----------------------pressure_calculate------------------------------------*/
void pressure_calculate() {
  pressure = (voltage/(vcc*0.6667)) - (0.1/0.6667); 
}

/*-------------------------- measure_flag_status_check  -------------------------*/

void measure_flag_check() {
  if (measure_flag == 1) {
    measure_flag = 0;
    
     // retrieve data from DS323   
    readDS3231time(&Sec, &Min, &Hour, &DayWeek, &DayMonth, &Month, &Year);
    time_date_processing(); 

    voltage_measure();
    pressure_calculate();
    write_to_SD();
    led_measure_indicator_set();
    //serial_print_pressure();
  }
}

/*------------------------------- read_time_from_RTC --------------------------------------------*/
void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year){
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}

/*---------------------------- time_date_processing --------------------------------------------------------------*/
void time_date_processing(){
  date = "";
  
  if (Hour<10){
      date = date + "0";
  }
  date = date + String(Hour);
  date = date + ":";
  if (Min<10){
      date = date + "0";
  }
  date = date + String(Min);
  date = date + ":";
  if (Sec<10){
      date = date + "0";
  }
  date = date + String(Sec);
  date = date + " ";
  date = date + String(DayMonth);
  date = date + "-";
  date = date + String(Month);
  date = date + "-";
  date = date + String(Year);
  date = date + " - ";

  Serial.print(date);
}

/*---------------------------- file_name_creation --------------------------------------------------------------*/
void file_name_creation(){
  readDS3231time(&Sec, &Min, &Hour, &DayWeek, &DayMonth, &Month, &Year);

  if (Hour<10){
      file_name = file_name + "0";
  }
  file_name = file_name + String(Hour);
  if (Min<10){
      file_name = file_name + "0";
  }
  file_name = file_name + String(Min);
  if (Sec<10){
      file_name = file_name + "0";
  }
  file_name = file_name + String(Sec);
  //file_name = file_name + String(DayMonth);
  //file_name = file_name + String(Month);
  //file_name = file_name + String(Year);

  file_name = file_name + ".txt";

  Serial.println(file_name);
}

/*----------------------serial-print_pressure------------------------------------*/
void serial_print_pressure() {
  Serial.print("pressure = ");
  Serial.print(pressure);
  Serial.print(" MPa");
  Serial.print(" ## ");
  Serial.print("voltage = ");
  Serial.print(voltage);
  Serial.println(" V");
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

/*------------------------ write_to_SD ---------------------------------------*/
void write_to_SD(){
  String message = "";

  message = date + String(pressure) + "," + String(voltage) + ";" + "\r\n";
  
  Soubor = SD.open(file_name, FILE_WRITE); 
  
  if (Soubor) {
    Soubor.print(message);
    Serial.print(message);
    Soubor.close();
  }
}