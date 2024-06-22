/* Le but de ce programme est de nous donner 
la mesure de l'humiditié par le sensor toutes les 1 s */
// Just with WiFi classes sufficient

#include <Arduino.h>
#include "Network.h"

//Measure
uint8_t soil = 0;
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
  //TO DO etalonnage correct capteur
  // Collect Data from Sensor
  soil = analogRead(sensor_pin); // read the data
  Serial.println(String("Soil val: ") + soil);
  humidity = map(soil, 0, 200, 0, 100); //((sensor_analog/1900.00) * 100);
  String humidityString = String(humidity);
  Serial.println(String("Value after mapped: ") + humidity);

  //Network Send Data
  http_send_data(humidityString);

  //TO DO Grafana with influxdb to visualize datas on a dashboard

  //Irrigation
  if(humidity >= 60){
    
    Serial.println("Enough Water for plants");
    digitalWrite(pump_pin, LOW);
  
  }else if(humidity <= 5){

    Serial.println("Water Level Is Low");

    digitalWrite(pump_pin, HIGH);
    
    /*for(int i=0; i<20; i++){
      digitalWrite(pump_pin, HIGH);
      delay(50);
      digitalWrite(pump_pin, LOW);
      delay(50);
    }*/

  }else{

    Serial.println("Water Level Is OK");
    
  }

    delay(2000);

}