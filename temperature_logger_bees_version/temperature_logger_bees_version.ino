//####################################################
//#   temperature_logger_bees_version
//#   Copyright (c) 2023 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 10 s
//               -   timer 0 used

// HW: Aruino nano, temperatrue sensor DS18B20, micros SD card module

/*----------------------- DEPENDENCES ----------------------------------*/
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // library for default watchdog functions

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
const byte tpl110_reset_pin = 8;
const byte cs_pin = 10;
const byte voltage_battery_pin = A0;

bool measure_flag = 1;

int timer0_counter = 0;
static int timer0_10s_count = 9770; //1s = 977

float temperature[3] = {100.0,100.0,100.0}; // temperature of water in °C
bool temp_control[3] = {0,0,0}; // indicitation of plug in sensors

static int measure_count = 10;  //counts of battery voltage samples
float voltage_divided = 0.0;  //battery voltage divivded
float voltage_battery = 0.0;  //battery voltage
static float res_1 = 330000.0; // resistance of res_1 resistor
static float res_2 = 390000.0; // resistance of res_2 resistor
float res = res_1 + res_2; 

File soubor; // variable for SD card
String file_name = "DATA.txt";



void setup() {
  serial_initial();
  analogReference(DEFAULT);
  pins_initial();
  sensors_initial();
  serial_initial_output();
  sd_card_initial();
  timer0_initial();
  wdt_enable(WDTO_2S);  //wdt reset_15MS; 30MS; 60ms; 120MS; 250MS; 500MS; 1S; 2S; 4S; 8S
  bootup_led_indication();
}

void loop() {
  if (measure_flag == 1) {
    measure_flag = 0;
    measure_temperatures();
    voltage_measure();
    write_to_sd();
    serial_output();
    tpl_110_reset();
  }
  wdt_reset (); // Reset watchdog counter
}
