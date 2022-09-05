/*----------------------serial-initialization------------------------------------*/
void serial_initial() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

/*--------------------------timer0-initialization------------------------------------------*/
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
    voltage_measure();
    pressure_calculate();
    led_measure_indicator_set();
    serial_print_pressure();
  }
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