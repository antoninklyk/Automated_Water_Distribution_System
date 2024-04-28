/* Le but de ce programme est de nous donner 
la mesure de l'humiditié par le sensor toutes les 1 s */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>


uint8_t sensor_analog = 0;
uint8_t humidity = 0;
const int sensor_pin = 35;  /* Soil moisture sensor O/P pin */

WiFiServer server(80);
HTTPClient httpclient;
const char* ssid = "antoniPhone";
const char* password = "123456789";

int last_time = 0;
int time_to_wait = 5000;

void setup(void){

  //Set the baudrate to 115200
  Serial.begin(115200);

  // WIFI SETUP //
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("WiFi Setup Done !\n\n\n");

  // SERVER SETUP //
  server.begin();

}

void loop(void){

  // Listen for incoming clients ???
  //WiFiClient client = server.available();

  // Collect Data from Sensor
  sensor_analog = analogRead(sensor_pin); // read the data
  humidity = (((sensor_analog/4095.00) * 100)+40);
  Serial.print("Moisture = ");
  Serial.print(humidity);  /* Print Temperature on the serial window */
  Serial.println("%");
  delay(1000);
  
  if((millis() - last_time)>=time_to_wait){
    // Verif Connexion Physique au WiFi
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi Connection Setup Correctly !");

      //Creation et configuration Client
      HTTPClient httpclient;
      String serverPath = String("???humidity=50%");
      httpclient.begin(serverPath);
      int httpResponseCode = httpclient.GET();

      // Analyse Adresse
      if(httpResponseCode>0){

        Serial.print("HTTP Response code: ");
        Serial.print(httpResponseCode);
        String payload = httpclient.getString();
        Serial.print(payload);
      }
      else{
        Serial.print("HTTP Error code: ");
        Serial.print(httpResponseCode);
      }
    } else {
      Serial.println("Erreur de connexion au réseau WiFi");
    }
    last_time = millis();
  }
  
}