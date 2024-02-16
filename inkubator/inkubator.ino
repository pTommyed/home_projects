//####################################################
//#   temperature_logger_bees_version
//#   Copyright (c) 2024 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 8 s
//               -   watchdog used

// HW: Aruino nano, temperatrue sensor DHT22, capacitive moiscure sensor, micros SD card module, ssr relay modul 4ch

/*----------------------- DEPENDENCES ----------------------------------*/
#include <avr/sleep.h>      // library for sleep
#include <avr/power.h>      // library for power control
#include <avr/wdt.h>        // library for default watchdog functions
#include <avr/interrupt.h>  // library for interrupts handling

#include <SPI.h> // lib to SPI sbernici
#include <SD.h> // lib to SD modul

/*----------------------- DEFINITION -----------------------------------*/


const byte led_indication = 5;
const byte cs_pin = 10;
const byte watter_level_mess_pin = A0;
const byte relay_humidity = 9;
const byte relay_heater = 8;

bool measure_flag = 1;

static int measure_count = 10;  //counts of analog samples
int watter_level = -100; 


void setup() {
  serial_initial();
  analogReference(DEFAULT);
  pins_initial();
  bootup_led_indication();

  digitalWrite(relay_heater, HIGH);
  digitalWrite(relay_humidity, HIGH);
}

void loop() {
  voltage_measure();
  Serial.println(watter_level);
  delay(2000);
}
