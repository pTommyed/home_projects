//####################################################
//#   temperature_logger_bees_version
//#   Copyright (c) 2023 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 1 Hz
//               -   timer 0 -- sampling freq

// HW: Aruino nano, temperatrue sensor DS18B20, 2 micros SD card module

/*----------------------- DEPENDENCES ----------------------------------*/
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

#include <OneWire.h>
#include <DallasTemperature.h>

#include <SPI.h> // lib to SPI sbernici
#include <SD.h> // lib to SD modul


/*----------------------- DEFINITION -----------------------------------*/
#define ONE_WIRE_BUS_1 4
#define ONE_WIRE_BUS_2 5
#define ONE_WIRE_BUS_3 6

OneWire oneWire_BUS1(ONE_WIRE_BUS_1);
OneWire oneWire_BUS2(ONE_WIRE_BUS_2);
OneWire oneWire_BUS3(ONE_WIRE_BUS_3);

DallasTemperature sensor_1(&oneWire_BUS1);
DallasTemperature sensor_2(&oneWire_BUS2);
DallasTemperature sensor_3(&oneWire_BUS3);

const byte led_indication = 7;
const byte cs_pin = 10;

bool measure_flag = 0;

float temperature[3] = {100.0,100.0,100.0}; // temperature of water in °C
bool temp_control[3] = {0,0,0}; // indicitation of plug in sensors

File soubor; // variable for SD card
String file_name = "DATA.txt";



void setup() {
  serial_initial();
  pins_initial();
  sensors_initial();
  serial_initial_output();
  sd_card_initial();
  bootup_led_indication();
}

void loop() {
  
}
