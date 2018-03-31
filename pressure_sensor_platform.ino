#include <Wire.h>

// Knock-off bmp280 breakouts are at different address.
#define BMP280_ADDRESS                (0x76)

#define DEBUG   0

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

Adafruit_BMP280 bmp; // I2C

// Mux control pins
#define PIN_S0 8
#define PIN_S1 9
#define PIN_S2 10
#define PIN_S3 11
#define PIN_S4 12
#define PIN_S5 13

char pbuf[128];

/*
 * PIN_S0   MUX 2 pin E (HIGH=disable MUX)
 * PIN_S1   MUX 1 pin E (HIGH=disable MUX)
 * PIN_S2   shared between MUXs, pin S0
 * PIN_S3   shared between MUXs, pin S1
 * PIN_S4   shared between MUXs, pin S2
 * PIN_S5   shared between MUXs, pin S3
 */
int controlPin[] = {PIN_S0, PIN_S1, PIN_S2, PIN_S3, PIN_S4, PIN_S5};

  int muxChannel[32][6]={
    {1,0,0,0,0,0}, //channel 0
    {1,0,1,0,0,0}, //channel 1
    {1,0,0,1,0,0}, //channel 2
    {1,0,1,1,0,0}, //channel 3
    {1,0,0,0,1,0}, //channel 4
    {1,0,1,0,1,0}, //channel 5
    {1,0,0,1,1,0}, //channel 6
    {1,0,1,1,1,0}, //channel 7
    {1,0,0,0,0,1}, //channel 8
    {1,0,1,0,0,1}, //channel 9
    {1,0,0,1,0,1}, //channel 10
    {1,0,1,1,0,1}, //channel 11
    {1,0,0,0,1,1}, //channel 12
    {1,0,1,0,1,1}, //channel 13
    {1,0,0,1,1,1}, //channel 14
    {1,0,1,1,1,1}, //channel 15
    {0,1,0,0,0,0}, //channel 16
    {0,1,1,0,0,0}, //channel 17
    {0,1,0,1,0,0}, //channel 18
    {0,1,1,1,0,0}, //channel 19
    {0,1,0,0,1,0}, //channel 20
    {0,1,1,0,1,0}, //channel 21
    {0,1,0,1,1,0}, //channel 22
    {0,1,1,1,1,0}, //channel 23
    {0,1,0,0,0,1}, //channel 24
    {0,1,1,0,0,1}, //channel 25
    {0,1,0,1,0,1}, //channel 26
    {0,1,1,1,0,1}, //channel 27
    {0,1,0,0,1,1}, //channel 28
    {0,1,1,0,1,1}, //channel 29
    {0,1,0,1,1,1}, //channel 30
    {0,1,1,1,1,1}  //channel 31
  };


void select_channel(int channel)
{
  for (int i=0; i<6; i++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
}


void setup_channel_pins()
{
  for (int i=0; i<6; i++) {
    pinMode(controlPin[i], OUTPUT);
  }
}



void empty_lines()
{
  for (int i=0; i<3; i++)
    Serial.println();
}



void setup() {
  bmp280_calib_data cal;
  Serial.begin(9600);

  empty_lines();
  Serial.println(F("BMP280 sensor platform by Antti Harri 2018"));
  empty_lines();

  Serial.println(F("Calibration data BEGINS"));
  setup_channel_pins();
  
  for (int i=0; i<32; i++) {
    select_channel(i);
    if (bmp.begin()) {  
      cal = bmp.getCoefficients();

      // T1-T3
      snprintf(pbuf, sizeof(pbuf), "C=%i;T1=%u;T2=%i;T3=%i;", i, cal.dig_T1, cal.dig_T2, cal.dig_T3);
      Serial.print(pbuf);

      // P1-P9
      snprintf(pbuf, sizeof(pbuf), "P1=%u;P2=%i;P3=%i;P4=%i;P5=%i;P6=%i;P7=%i;P8=%i;P9=%i;",
        i,
        cal.dig_P1,
        cal.dig_P2,
        cal.dig_P3,
        cal.dig_P4,
        cal.dig_P5,
        cal.dig_P6,
        cal.dig_P7,
        cal.dig_P8,
        cal.dig_P9
      );
      Serial.print(pbuf);
      
      // H1-H6
      snprintf(pbuf, sizeof(pbuf), "H1=%u;H2=%i;H3=%u;H4=%i;H5=%i;H6=%i;",
        i,
        cal.dig_H1,
        cal.dig_H2,
        cal.dig_H3,
        cal.dig_H4,
        cal.dig_H5,
        cal.dig_H6
      );
      Serial.println(pbuf);
    }
  }
  Serial.println(F("Calibration data ENDED"));
  empty_lines();

  delay(2000);
}

void loop() {
  for (int i=0; i<32; i++) {
    select_channel(i);
    //bmp.begin();
    snprintf(pbuf, sizeof(pbuf), "C=%i;T=%i;P=%i", i, bmp.readRawTemperature(), bmp.readRawPressure());
    Serial.println(pbuf);
  }
}
