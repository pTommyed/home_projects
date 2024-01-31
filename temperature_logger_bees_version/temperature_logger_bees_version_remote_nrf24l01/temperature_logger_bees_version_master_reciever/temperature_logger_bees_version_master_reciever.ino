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

#include<Wire.h>
#include <RF24.h>
#include <printf.h>
#include <SPI.h> // lib to SPI sbernici

/*----------------------- DEFINITION -----------------------------------*/

RF24 Radio (7, 8);

const byte address_re[6] = "00002";
const int buf_size = 5;
int buf_re[buf_size];

const int log_size = 50;
const int data_size = 6;
int log_data [log_size] [data_size];


void setup() {
  serial_initial();
  log_data_init();
  nrf24l01_initial();
  serial_initial_output();
}

void loop() {
  remote_recieve_data();
}