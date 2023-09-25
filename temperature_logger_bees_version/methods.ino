/*----------------------serial-initialization------------------------------------*/
void serial_initial() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

/*----------------------pins-initialization------------------------------------*/
void pins_initial() {
  pinMode(cs_pin, OUTPUT);
  pinMode(led_indication, OUTPUT);

  digitalWrite(led_indication, LOW);
  delay(100);
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

/*------------------------------ sd_card_initial ------------------------------------*/
void sd_card_initial() {
  String message = "";

  while (!SD.begin(cs_pin)) { 
    Serial.println("SD_card_no_detected!");
    digitalWrite(led_indication, HIGH);
    delay(500);
    digitalWrite(led_indication, LOW);
  }

  if (SD.exists(file_name)) {
    Serial.println("soubor DATA.txt detected");
    } else {
        soubor = SD.open(file_name , FILE_WRITE);
        soubor.println("-----------------------------");
        soubor.println("-----------------------------");
        while (!soubor){
          Serial.println("error_SD_card!");
          digitalWrite(led_indication, HIGH);
          delay(100);
        }

        digitalWrite(led_indication, LOW);
        delay(100);

        for(int i=0;i<3;i++){
          int number = i + 1;
          if (temp_control[i] == 1) {
            message = "temp_" + String(number) + ": init OK;" ;
            } else {
                message = "temp_" + String(number) + ": no conected!;" ;
              }
            soubor.println(message);
            }
        soubor.println("-----------------------------");
        soubor.println("-----------------------------");
        soubor.close();
        }        
}

/*------------------------------ serial_initial_output ------------------------------------*/
void serial_initial_output() {
  String message = "";

  Serial.println("-----------------------------");
  Serial.println("-----------------------------");

  for(int i=0;i<3;i++) {
    int number = i + 1;
    if (temp_control[i] == 1) {
      message = "temp_" + String(number) + ": init OK" ;
    } else {
        message = "temp_" + String(number) + ": no conected!" ;
      }
    Serial.println(message);
  }

  Serial.println("-----------------------------");
  Serial.println("-----------------------------");       
}

/*------------------------------ serial_output ------------------------------------*/
void serial_output() {
  String message = "";

  Serial.println("-----------------------------");

  for(int i=0;i<3;i++) {
    if (temp_control[i] == 1) {
      int number = i + 1;
      message = "temp_" + String(number) + ": ";
      if (temperature[i] == -127) {
        message = message + "err";
        Serial.println(message);
      }else {
        message = message + String(temperature[i]) + " °C" ;
        Serial.println(message);
      }
    }
  }
  message = "battery voltage: " + String(voltage_battery) + " V"; 
  Serial.println(message);
  Serial.println("-----------------------------"); 

  delay(200);      
}

/*------------------------ write_to_SD ---------------------------------------*/
void write_to_sd() {
  String message = "";
  
  soubor = SD.open(file_name, FILE_WRITE); 
  while (!soubor){
    Serial.println("error_SD_card!");
    digitalWrite(led_indication, HIGH);
    delay(100);
    }

  digitalWrite(led_indication, LOW);
  delay(100);

  for(int i=0;i<3;i++){
    if (temp_control[i] == 1) {
      if (temperature[i] == -127) {
        soubor.print("err ;");
      } else {
        message = String(temperature[i]) + ";";
        soubor.print(message);
        }
      }
    }
  message = String(voltage_battery) + ";"; 
  soubor.println(message);
  soubor.close();
} 

/*-----------------------Sensors-initialization--------------------------------*/
void sensors_initial() {
  float temp[3] = {100.0,100.0,100.0}; // temperature in°C
  
  sensor_1.begin(); // temperature 1 init
  sensor_2.begin(); // temperature 2 init
  sensor_3.begin(); // temperature 3 init

  sensor_1.requestTemperatures();
  sensor_2.requestTemperatures();
  sensor_3.requestTemperatures();

  temp[0]=sensor_1.getTempCByIndex(0);
  temp[1]=sensor_2.getTempCByIndex(0);
  temp[2]=sensor_3.getTempCByIndex(0);

  for(int i=0;i<3;i++){
    if (temp[i] != -127) {
      temp_control[i] = 1;
    }
  }
}

/*-----------------------Sensors-requesting-temperature--------------------------------*/
void sensors_request() {
  sensor_1.requestTemperatures();
  sensor_2.requestTemperatures();
  sensor_3.requestTemperatures();
}

/*-----------------------Sensors-get-temperature--------------------------------*/
void sensors_get_temp() {
  temperature[0]=sensor_1.getTempCByIndex(0);
  temperature[1]=sensor_2.getTempCByIndex(0);
  temperature[2]=sensor_3.getTempCByIndex(0);
}

/*----------------------voltage_measure------------------------------------*/
void voltage_measure() {
  int voltage_temp = 0;

  for (int temp =0;temp < measure_count; temp++) {
    voltage_temp = voltage_temp + analogRead(voltage_battery_pin);
  }

  voltage_divided = voltage_temp / measure_count;
  voltage_divided = voltage_divided * (5.0 / 1023.0);

  voltage_battery = voltage_divided / (res_2 / res);
}

/*------------------------sleep_mode_to_8_sec_init -----------------------------------------*/

void sleep_mode_8s_init () {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  //set sleep mode

  MCUSR &= ~(1 << WDRF);                           // reset watch dog
  WDTCSR |= (1 << WDCE) | (1 << WDE);              // eneable watch dog setting
  WDTCSR = (1 << WDP0) | (1 << WDP3) ;             // set watch dog to 8 s
  WDTCSR |= (1 << WDIE);                           // enale interrupt mode
}

/*-----------------------tpl_110_reset--------------------------------*/
void tpl_110_reset() {
  for (int i=0; i<2; i++) {
    digitalWrite(tpl110_reset_pin, HIGH);
    delay(50);
    digitalWrite(tpl110_reset_pin, LOW);
    delay(50);
  } 
}

/*------------------------run _leep_mode_to_8_sec -----------------------------------------*/

void run_sleep_8s () {
  sleep_enable(); // enable sleep
  sleep_mode();  // start sleep
  sleep_disable();  // probuzení
}

/*-------------------------- wake_up_from_Watchdog ----------------*/

ISR( WDT_vect ) {
  measure_flag = 1;
}

/*-------------------------- measure_temperatures  -------------------------*/
void measure_temperatures() {
  sensors_request();
  sensors_get_temp();
}