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
  Radio.openReadingPipe(1, address_re);
  Radio.setChannel(115);  //115 band above WIFI signals
  Radio.setPALevel(RF24_PA_MAX); //MIN power low rage
  Radio.setDataRate(RF24_1MBPS) ;  //Minimum speed
  //Serial.println("Receive Setup Initialized");
  //Radio.printDetails();

  buf_re_clear();

  Radio.startListening();
}

/*----------------------transmit_bufer_re_clean------------------------------------*/
void buf_re_clear() {
 for (int i=0;i<5;i++){
    buf_re[i] = 30000;
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
      
      log_data [buf_re[0]][5] = 1;
    }
    
    buf_re_clear();
    serial_output();
  }
}

/*------------------------------ serial_initial_output ------------------------------------*/
void serial_initial_output() {
  String message = "";

  Serial.println("-----------------------------");
  Serial.println("-----------------------------");

  Serial.println("reciever module");

  Serial.println("-----------------------------");
  Serial.println("-----------------------------");       
}

/*------------------------------ serial_output ------------------------------------*/
void serial_output() {
  String message = "";
  float temp = 0.0;

  for (int i=0;i<log_size;i++){
    if ((log_data[i][0]!=200) && (log_data[i][5]==1)){
      message = "module " + String(log_data[i][0]) + ": ";
      Serial.print(message);
      for (int y=1;y<4;y++){
        temp = log_data[i][y] / 100.00;
        if (temp == -127) {
          Serial.print("err; ");
        } else if (temp == 300) {
            Serial.print("NAN; ");
          } else {
              message = String(temp) + " °C ";
              Serial.print(message);
            }
      }
      temp = log_data[i][4] / 100.00;
      if (temp == 300) {
        Serial.println("NAN; ");
      } else {
          message = String(temp) + " V";
          Serial.println(message);
        }
      //log_data [i] [5] = 200;  // pokud se zruší komentář tak se vždy vypíší jen jednou nová data a ne opakovaně
    }
  }

  delay(250);      
}
