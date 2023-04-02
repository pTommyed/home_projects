//####################################################
//#   water_pressure_sensor_module
//#   Copyright (c) 2022 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 1 Hz
//               -   timer 0 -- sampling freq

// HW: Aruino nano, SD Card Module, RTC module DS3231

/*----------------------- DEPENDENCES ----------------------------------*/
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

#include <SPI.h> // lib for SPI 
#include <SD.h> // lib for SD modul

#include <Wire.h>
#define DS3231_I2C_ADDRESS 0x68

/*----------------------- DEFINITION -----------------------------------*/
File Soubor; // variable for SD card
String file_name = "";

String date = "";
byte Sec, Min, Hour, DayWeek, DayMonth, Month, Year;

byte decToBcd(byte val){
    return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
    return( (val/16*10) + (val%16) );
}

const byte pressure_pin = A0;

const byte cs_pin = 10; // SPI CS pin number
const byte led_on_indication = 4;
const byte led_measure_indication = 5;

bool led_measure_indication_flag = 0;

bool measure_flag = 0;
static int measure_count = 10;
float voltage = 0.0; 
float pressure = 0.0;  // pressure [MPa]
static float vcc = 4.609; // source voltage [V]

int timer0_counter =0;
static int timer0_10s_count = 9770; //1s = 977


void setup() {
  analogReference(DEFAULT);
  pins_initial();
  Wire.begin();
  serial_initial();
  file_name_creation();
  sd_card_initial();
  bootup_led_indication();
  timer0_initial();
  wdt_enable(WDTO_2S);  //wdt reset_15MS; 30MS; 60ms; 120MS; 250MS; 500MS; 1S; 2S; 4S; 8S
}

void loop() {
measure_flag_check ();
wdt_reset (); // Reset watchdog counter
}
