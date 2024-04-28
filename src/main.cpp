/* Le but de ce programme est de nous donner 
la mesure de l'humiditié par le sensor toutes les 1 s */

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <ESPmDNS.h>


uint8_t sensor_analog = 0;
uint8_t humidity = 0;
const int sensor_pin = 35;  /* Soil moisture sensor O/P pin */

const char *ssid = "";
const char *password = "";
//const char *serverUrl = "http://example.com/api"; // URL du serveur

WebServer server(80);
WiFiClient client;

void handleRoot() {
  Serial.print("Handle Root");
  server.send(200, "text/plain", "hello from esp32!");
  Serial.print("End!");
}

void handleNotFound() {
  Serial.print("Handle Not Found");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.print("End!");
}

void clientSendData(uint8_t dataToSend) //idéalement il faudrait une requete HTTP Post (Créer ressource) et des requetes HTTP PUT (Update)
{  
  // Verif Connexion Physique au WiFi
  if (WiFi.status() == WL_CONNECTED) {
  Serial.println("WiFi Connection Setup Correctly !\n\n\n");


    //Verif que le Client est bien connecté au Server Setup précédemment
    int responseCode = client.connect(WiFi.localIP(),80);

    if(responseCode){

      Serial.print("Successfully connected to Server !");
      
      // NO WRITE ON CLIENT BUT PRINT
      //Serial.println("Client is writing ...");
      //size_t isWriting = client.write(dataToSend);

      // Création de la requête HTTP POST
      String postData = "data=" + String(dataToSend);
      client.println("GET / http://" + String(WiFi.localIP()));
      client.println(":80 HTTP/1.1"); // Remplacez "/votre_ressource" par le chemin de votre ressource sur le serveur
      client.println("Host: " + String(WiFi.localIP()));
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Content-Length: " + String(postData.length()));
      client.println();
      client.println(postData);
      delay(10);

            Serial.print("Successfully connected to Server !");



      /* if(isWriting){
        
        // Send Header to Server
        server.sendHeader("Content-Type", "application/x-www-form-urlencoded");

        
        //Analyse Reponse du Server
        //server.sendContent(postData); //POST


        server.send(200, "text/plain", postData);
        Serial.print("Data correctly Sended to the Server\n\n\n");

      }else{
        
        Serial.print("Data NOT correctly writed in the Server");

      } */

    } else {

      Serial.print("Erreur lors de la connexion au Server : ");
      Serial.println(responseCode);

    }

  } else {

  Serial.println("Erreur de connexion au réseau WiFi");

  }
}

void serverReceiveData(String data) //HTTP GET
{
  if(server.hasArg(data)){
    String donnee =server.arg(data);
    Serial.print("donnée reçue : ");
    Serial.print(donnee);
    server.send(200, "text/plain", "Données reçues avec succès");
  }else{
    server.send(404, "text/plain", "data Doesn't exist!");
  }
}


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

  // MDNS SETUP //
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started !\n\n\n");
  }

  // SERVER SETUP//
  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP Server Started !\n\n\n");

  // WIFI CLIENT SETUP //
  client.connect(WiFi.localIP(), 80);
  Serial.print("Client connection");
  while(client.connected()!=1){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("HTTP Client Connected to the Server !\n\n\n");

}

void loop(void){
  sensor_analog = analogRead(sensor_pin); // read the data
  humidity = (((sensor_analog/4095.00) * 100)+40);
  Serial.print("Moisture = ");
  Serial.print(humidity);  /* Print Temperature on the serial window */
  Serial.println("%");

  server.handleClient();

  clientSendData(humidity);

  delay(1000);              /* Wait for 1000mS */


  serverReceiveData("humidity");

  delay(10000);              /* Wait for 1000mS */
}