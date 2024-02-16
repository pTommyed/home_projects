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
  pinMode(relay_humidity, OUTPUT);
  pinMode(relay_heater, OUTPUT);

  digitalWrite(relay_humidity, LOW);
  digitalWrite(relay_heater, LOW);
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

/*----------------------voltage_measure------------------------------------*/
void voltage_measure() {
  int voltage_temp = 0;

  for (int temp =0;temp < measure_count; temp++) {
    voltage_temp = voltage_temp + analogRead(watter_level_mess_pin);
  }

  watter_level = voltage_temp / measure_count;
}