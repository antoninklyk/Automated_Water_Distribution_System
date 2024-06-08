/* Le but de ce programme est de nous donner 
la mesure de l'humiditié par le sensor toutes les 1 s */
// Just with WiFi classes sufficient

#include <Arduino.h>
#include "Network.h"

//Measure
uint8_t sensor_analog = 0;
uint8_t humidity = 0;
const int sensor_pin = 35;  /* Soil moisture sensor pin */
const int pump_pin = 26;


void setup(void)
{
  //Set the baudrate to 115200
  Serial.begin(115200);

  //Irrigation setup
  pinMode(pump_pin, OUTPUT);

  // Setup all network parameters
  network_setup();

}

void loop(void)
{

  // Collect Data from Sensor
  sensor_analog = analogRead(sensor_pin); // read the data
  humidity = (((sensor_analog/4095.00) * 100)+40);
  Serial.print("Moisture = ");
  Serial.print(humidity);  /* Print Temperature on the serial window */
  Serial.println("%");
  String humidityString = String(humidity);

  delay(1000);

  //Irrigation
  if(humidity <= 50){
    digitalWrite(pump_pin, HIGH);
    delay(1000);
    digitalWrite(pump_pin, HIGH);
    delay(1000);
    digitalWrite(pump_pin, HIGH);
    delay(1000);
    digitalWrite(pump_pin, HIGH);
    delay(1000);
  }else{
    digitalWrite(pump_pin, LOW);
  }

  client_send_data(humidityString);
}