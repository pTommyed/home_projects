/*----------------------serial-initialization------------------------------------*/
void serial_initial() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  printf_begin();
}

/*----------------------nrf24l01-initialization------------------------------------*/
void nrf24l01_initial() {
  Radio.begin();
  //Radio.setAddressWidth(5);
  Radio.openWritingPipe(address_tr);
  Radio.openReadingPipe(1, address_re);
  Radio.setChannel(115);  //115 band above WIFI signals
  Radio.setPALevel(RF24_PA_MAX); //MIN power low rage
  Radio.setDataRate(RF24_1MBPS) ;  //Minimum speed
  //Serial.println("Receive Setup Initialized");
  //Radio.printDetails();

  buf_tr_clear();
  buf_re_clear();

  Radio.startListening();
}

/*----------------------transmit_bufer_tr_clean------------------------------------*/
void buf_tr_clear() {
 for (int i=0;i<5;i++){
    buf_tr[i] = 100;
  }
}

/*----------------------transmit_bufer_re_clean------------------------------------*/
void buf_re_clear() {
 for (int i=0;i<5;i++){
    buf_re[i] = 100;
  }
}

/*----------------------log_data_init------------------------------------*/
void log_data_init() {
 for (int i=0;i<log_size;i++){
    for (int y=0;y<data_size;y++){
      log_data [i] [y] = 200;
    }
  }
}

/*----------------------remote_recieve_data------------------------------------*/

void remote_recieve_data()
{
  uint8_t bytes;

  if (Radio.available()){
    while (Radio.available()){ 
      bytes = Radio.getPayloadSize();
      Radio.read(&buf_re, sizeof(buf_re));

      for (int i =0; i<5;i++){
        log_data [buf_re[0]][i] = buf_re[i];
      }
      
      log_data [buf_re[0]][5]=1;
      log_data [buf_re[0]][6]=1;
    }
    
    buf_re_clear();
  }
}

/*----------------------remote_transmit_data------------------------------------*/
void remote_transmit_data(){
  delay(5);
  Radio.stopListening(); //Stop Receiving and start transminitng
  
  buf_tr[0] = log_data[0][0];
  buf_tr[1] = log_data[0][1];

  Radio.openWritingPipe(address_tr);//Sends data on this 40-bit address 
  Radio.write(&buf_tr, sizeof(buf_tr));
  delay(5);
  buf_tr_clear();

  for (int i=1;i<log_size;i++){
    if ((log_data[i][0]!=200) && (log_data[i][5]==1)){
      for (int y=0;y<5;y++){
        buf_tr[y] = log_data[i][y];
      }
      Radio.openWritingPipe(address_tr);//Sends data on this 40-bit address 
      Radio.write(&buf_tr, sizeof(buf_tr));
      delay(5);
      buf_tr_clear();
    }
  }

  Radio.startListening();
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
          digitalWrite(led_indication, HIGH);
          delay(500);
          digitalWrite(led_indication, LOW);
          delay(500);
        }
        
        soubor.println("master module");

        soubor.println("-----------------------------");
        soubor.println("-----------------------------");
        soubor.close();
        } 

  delay(250);            
}

/*------------------------------ serial_initial_output ------------------------------------*/
void serial_initial_output() {
  String message = "";

  Serial.println("-----------------------------");
  Serial.println("-----------------------------");

  Serial.println("master module");

  Serial.println("-----------------------------");
  Serial.println("-----------------------------");       
}

/*------------------------------ serial_output ------------------------------------*/
void serial_output() {
  String message = "";
  float temp = 0.0;

  Serial.println("-----------------------------");

  message ="0: "+ String(voltage_battery) + " V"; 
  Serial.println(message);

  for (int i=1;i<log_size;i++){
    if ((log_data[i][0]!=200) && (log_data[i][5]==1)){
      message = String(log_data[i][0]) + ": ";
      Serial.print(message);
      for (int y=1;y<4;y++){
        temp = log_data[i][y] / 100;
        if (temp == -127) {
          Serial.print("err ;");
        } else {
          message = String(temp) + " °C";
          Serial.print(message);
          }
        temp = log_data[i][4] / 100;
        message = String(temp) + " °C";
        Serial.println(message);
      }
    }
  }

  Serial.println("-----------------------------"); 

  delay(250);      
}

/*------------------------ write_to_SD ---------------------------------------*/
void write_to_sd() {
  String message = "";
  float temp = 0.0;
  
  soubor = SD.open(file_name, FILE_WRITE); 
  while (!soubor){
    Serial.println("error_SD_card!");
    digitalWrite(led_indication, HIGH);
    delay(500);
    digitalWrite(led_indication, LOW);
    delay(500);
    }

  message ="0;"+ String(voltage_battery) + ";"; 
  soubor.println(message);

  for (int i=1;i<log_size;i++){
    if ((log_data[i][0]!=200) && (log_data[i][5]==1)){
      message = String(log_data[i][0]) + ";";
      soubor.print(message);
      for (int y=1;y<4;y++){
        temp = log_data[i][y] / 100;
        if (temp == -127) {
          soubor.print("err;");
        } else {
          message = String(temp) + ";";
          soubor.print(message);
          }
        temp = log_data[i][4] / 100;
        message = String(temp) + ";";
        soubor.println(message);
      }
      log_data [i] [5] = 200;
    }
  }
  soubor.close();
  delay(250);
} 

/*----------------------voltage_measure------------------------------------*/
void voltage_measure() {
  int temp = 0;
  int voltage_temp = 0;

  for (int temp =0;temp < measure_count; temp++) {
    voltage_temp = voltage_temp + analogRead(voltage_battery_pin);
  }

  voltage_divided = voltage_temp / measure_count;
  voltage_divided = voltage_divided * (5.0 / 1023.0);

  voltage_battery = voltage_divided / (res_2 / res);
  
  temp = voltage_battery * 100;
  log_data[0][0] = index_device;
  log_data[0][1] = temp;
}

/*------------------------sleep_mode_to_8_sec_init -----------------------------------------*/

void sleep_mode_8s_init () {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  //set sleep mode

  MCUSR &= ~(1 << WDRF);                           // reset watch dog
  WDTCSR |= (1 << WDCE) | (1 << WDE);              // eneable watch dog setting
  WDTCSR = (1 << WDP0) | (1 << WDP3) ;             // set watch dog to 8 s
  WDTCSR |= (1 << WDIE);                           // enale interrupt mode
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
