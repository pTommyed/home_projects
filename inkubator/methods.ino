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

/*-------------------------- wake_up_from_Watchdog ----------------*/

ISR( WDT_vect ) {
  measure_flag = 1;
}

/*------------------------------ sd_card_initial ------------------------------------*/
void sd_card_initial() {
  String message = "";

  while (!SD.begin(cs_pin)) { 
    Serial.println("SD_card_no_detected!");
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
          lcd.setCursor(1,0); 
          lcd.print("Inicialization"); 
          delay(250);
          lcd.setCursor(1,1); 
          lcd.print("error_SD_card!");
          
          digitalWrite(led_indication, HIGH);
          delay(500);
          digitalWrite(led_indication, LOW);
          delay(500);
        }

        soubor.println("air_temperatur[°C];  humidity[%];  watter_level[-];  watter_level[%];  heater[-];  humidifier[-];");    
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

  watter_level_convert_percent();
}

/*----------------------watter_level_percent------------------------------------*/
void watter_level_convert_percent() {
  
  watter_level_percent = (-1*(watter_level_max - watter_level)) / one_percent;
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
  

  message = String(watter_level) + ";" + String(watter_level_percent) + ";" + String(heater_on_flag) + ";" + String(humidity_on_flag) + ";"; 
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
  

  message = String(watter_level) + ";" + String(watter_level_percent) + ";" + String(heater_on_flag) + ";" + String(humidity_on_flag) + ";"; 
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
    if ((humidity < humidity_min ) && (watter_level > watter_level_min) && (humidity_on_flag == 0)) {
      humidity_on_flag = 1;
      digitalWrite(relay_humidity, HIGH);
      delay(100);
    } else if (((air_temp > humidity_max) && (humidity_on_flag == 1)) || (watter_level <= watter_level_min)) {
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

