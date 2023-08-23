//####################################################
//#   temperatue_meassurement_heater
//#   Copyright (c) 2023 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 1 Hz
//               -   timer 0 -- sampling freq

// HW: Aruino nano, temperatrue sensor DS18B20, I2C LCD displey 16x2; 2 chanels relay

/*----------------------- DEPENDENCES ----------------------------------*/
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

#include <Wire.h> //This library allows you to communicate with I2C / TWI devices
#include <LiquidCrystal_I2C.h> // knihovna pro ovladani displaye

#include <OneWire.h>
#include <DallasTemperature.h>

/*------------------------- Set the adress (0x3F)and pins on the I2C chip used for LCD connections ----------------------*/
 
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

/*----------------------- DEFINITION -----------------------------------*/
#define ONE_WIRE_BUS_1 4
#define ONE_WIRE_BUS_2 5

OneWire oneWire_BUS1(ONE_WIRE_BUS_1);
OneWire oneWire_BUS2(ONE_WIRE_BUS_2);

DallasTemperature sensor_1(&oneWire_BUS1);
DallasTemperature sensor_2(&oneWire_BUS2);

const float start_temp_kotel = 30.0;
const float hysterezia = 2.0;
float hysterezia_temp = start_temp_kotel - hysterezia;
const float max_temperature = 73.0;
float hysterezia_max_temperature = max_temperature + hysterezia;

float temp_boiler = 0.0;
float temp_kotel = 0.0;

bool temp_boiler_error_flag = 0;
bool temp_kotel_error_flag = 0;

const byte led_measure_indication = 8;
const byte relay_1_boiler_pin = 6;
const byte relay_2_kotel_pin = 7;

bool led_measure_indication_flag = 0;

bool relay_1_boiler_flag = 0;
bool relay_2_kotel_flag = 0;

bool measure_flag = 0;

int timer0_counter =0;
static int timer0_10s_count = 9770; //1s = 977


void setup() {
  lcd_initial();
  pins_initial();
  init_relay_set();
  serial_initial();
  sensors_initial();
  bootup_led_indication();
  timer0_initial();
  wdt_enable(WDTO_2S);  //wdt reset_15MS; 30MS; 60ms; 120MS; 250MS; 500MS; 1S; 2S; 4S; 8S
  Serial.println("Inicialization_done");
  lcd_print_end_of_init();
}

void loop() {
  measure_flag_check ();
  wdt_reset (); // Reset watchdog counter
}
