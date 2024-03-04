//####################################################
//#   temperature_logger_bees_version
//#   Copyright (c) 2024 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 8 s
//               -   watchdog used

// HW: Aruino nano, temperatrue sensor DHT22, capacitive moiscure sensor, micros SD card module, ssr relay modul 4ch

/*----------------------- DEPENDENCES ----------------------------------*/
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

#include <Wire.h> //This library allows you to communicate with I2C / TWI devices
#include <LiquidCrystal_I2C.h> // knihovna pro ovladani displaye

#include <SPI.h> // lib to SPI sbernici
#include <SD.h> // lib to SD modul

#include "DHT.h" // humidity sensore

#include <OneWire.h>

/*------------------------- Set the adress (0x3F)and pins on the I2C chip used for LCD connections ----------------------*/
 
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

/*----------------------- DEFINITION -----------------------------------*/

#define DHTPIN 4     // humidity sensor - pin
#define DHTTYPE DHT22   // DHT 22 
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor (humidity sensor)

const byte led_indication = 5;
const byte cs_pin = 10;
const byte watter_level_mess_pin = A0;
const byte relay_humidity = 9;
const byte relay_heater = 8;

const float air_temp_set = 35.0;
const float hysterezia = 0.5;
const float air_temp_max = air_temp_set + hysterezia;
const float air_temp_min = air_temp_set - hysterezia;

const float humidity_max = 80.0;
const float humidity_min = 60.0;

const int watter_level_full = 100; // 0-100 - full watter level
const int watter_level_none = 400; // 400-515 - none watter

float air_temp = 0.0; // in °C
float humidity = 1000.0; // in %
int watter_level = 2000;

bool heater_on_flag = 0;
bool humidity_on_flag = 0;

bool air_temp_error_flag = 0;
bool humidity_error_flag = 0;

bool measure_flag = 1;
bool led_measure_indication_flag  = 0;

static int measure_count = 10;  //counts of analog samples

int timer0_counter =0;
static int timer0_4s_count = 3908; //1s = 977

File soubor; // variable for SD card
String file_name = "DATA.txt";



/*----------------------setup------------------------------------*/

void setup() {
  lcd_initial();
  serial_initial();
  analogReference(DEFAULT);
  pins_initial();
  init_outputs_set();
  sensors_initial();
  sd_card_initial();
  lcd_print_end_of_init();
  bootup_led_indication();
  timer0_initial();
  wdt_enable(WDTO_8S);  //wdt reset_15MS; 30MS; 60ms; 120MS; 250MS; 500MS; 1S; 2S; 4S; 8S
}

/*----------------------main_loop------------------------------------*/

void loop() {
  if (measure_flag == 1) {
    measure_flag = 0;
    
    watter_level_measure();
    reading_data_DHT22();

    control_heater_output();
    control_humidity_output();

    write_to_sd();
    serial_output();
    lcd_print();

    led_measure_indicator_set();
  }
  wdt_reset (); // Reset watchdog counter
}
