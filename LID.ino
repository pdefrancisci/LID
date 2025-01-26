// #define DHTPIN 3 // By default its connected to pin D3, it can be changed, define it before the #include of the library
#include "Arduino_SensorKit.h"
#include <ArduinoJson.h>

//uncomment line below if using DHT20
#define Environment Environment_I2C

#include "AirQuality.h"
#include "Arduino.h"
AirQuality airqualitysensor;
int current_quality =-1;
JsonDocument doc;
float pressure;
void setup() {
  //uncomment line below if using DHT20
  Wire.begin();
  Serial.begin(9600);
  Environment.begin();
  Pressure.begin();
  Oled.begin();
  Oled.setFlipMode(true); // Sets the rotation of the screen
  Serial.println("Initializing Air Quality Sensor...");
  // airqualitysensor.init(A0);
  Serial.println("Sensor Initialized");
}

void loop() {
  float f_temp = (Environment.readTemperature() * 1.8) + 32;
  doc["temp"] = f_temp;
  
  doc["humidity"] = Environment.readHumidity();
  
  doc["pressure"] = Pressure.readPressure();
  
  doc["aqi"] = analogRead(A0);

  Oled.setFont(u8x8_font_chroma48medium8_r); 
  Oled.setCursor(0, 0);
  Oled.print("Temperature = \n");
  Oled.print(f_temp); 
  Oled.print("\nHumidity = \n");
  Oled.print(Environment.readHumidity());
  Oled.print("\nPressure = \n");
  Oled.print(Pressure.readPressure());
  Oled.print("\nAQI = ");
  Oled.print(analogRead(A0));
  Oled.refreshDisplay();
  
  serializeJson(doc, Serial);

  delay(2000);
}

ISR(TIMER2_OVF_vect)
{
    if(airqualitysensor.counter==122)//set 2 seconds as a detected duty
    {
 
        airqualitysensor.last_vol=airqualitysensor.first_vol;
        airqualitysensor.first_vol=analogRead(A0);
        airqualitysensor.counter=0;
        airqualitysensor.timer_index=1;
        PORTB=PORTB^0x20;
    }
    else
    {
        airqualitysensor.counter++;
    }
}
