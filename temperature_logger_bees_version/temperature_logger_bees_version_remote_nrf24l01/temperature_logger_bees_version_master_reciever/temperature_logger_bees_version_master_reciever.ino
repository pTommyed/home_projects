//####################################################
//#   temperature_logger_bees_version_remote_nrf24l01_reciever
//#   Copyright (c) 2024 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 8 s
//               -   watchdog used
//               -   remote version
//               -   reciever module

// HW: Aruino nano, nrf24l01

/*----------------------- DEPENDENCES ----------------------------------*/
#include <avr/sleep.h>      // library for sleep
#include <avr/power.h>      // library for power control
#include <avr/wdt.h>        // library for default watchdog functions
#include <avr/interrupt.h>  // library for interrupts handling

#include<Wire.h>
#include <RF24.h>
#include <printf.h>

#include <SPI.h> // lib to SPI sbernici
#include <SD.h> // lib to SD modul


/*----------------------- DEFINITION -----------------------------------*/

RF24 Radio (7, 8);

const byte address_tr[6] = "00002";
const byte address_re[6] = "00001";
const int buf_size = 5;
int buf_tr[buf_size];
int buf_re[buf_size];
const int index_device = 0; //index of device for remote comunication master

const byte led_indication = 2;
const byte cs_pin = 10;
const byte voltage_battery_pin = A0;

bool measure_flag = 1;

static int measure_count = 10;  //counts of battery voltage samples
float voltage_divided = 0.0;  //battery voltage divivded
float voltage_battery = 0.0;  //battery voltage
static float res_1 = 330000.0; // resistance of res_1 resistor
static float res_2 = 390000.0; // resistance of res_2 resistor
float res = res_1 + res_2; 

const int log_size = 50;
const int data_size = 6;
int log_data [log_size] [data_size];

File soubor; // variable for SD card
String file_name = "DATA.txt";



void setup() {
  serial_initial();
  log_data_init();
  nrf24l01_initial();
  analogReference(DEFAULT);
  pins_initial();
  serial_initial_output();
  sd_card_initial();
  sleep_mode_8s_init();
  bootup_led_indication();
}

void loop() {
  if (measure_flag == 1) {
    measure_flag = 0;
    voltage_measure();
    remote_transmit_data();
    serial_output();
    write_to_sd();
    run_sleep_8s();
  }
}