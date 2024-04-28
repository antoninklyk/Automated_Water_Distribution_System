/* Le but de ce programme est de nous donner 
la mesure de l'humiditié par le sensor toutes les 1 s */
// Just with WiFi classes sufficient

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
//variable to store the HTTP Request
String http_request_header;

int current_time = millis();
int last_time = 0;
int time_to_wait = 5000;
String output26State = "off";
String output27State = "off";

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
  WiFiClient client = server.available();

  // Collect Data from Sensor
  sensor_analog = analogRead(sensor_pin); // read the data
  humidity = (((sensor_analog/4095.00) * 100)+40);
  Serial.print("Moisture = ");
  Serial.print(humidity);  /* Print Temperature on the serial window */
  Serial.println("%");
  String humidityString = String(humidity);

  delay(1000);
  
  if(client){ //if a new client connects (On our listener)
    current_time = millis();
    last_time = current_time;
    Serial.println("New Client");
    String current_line = ""; // make a String to hold incoming data from the client

    while(client.connected() && current_time - last_time <= time_to_wait){ // Loop tant que le client est connecté, et que c'est time to read
      current_time = millis();
      //Setup le listener de http request header sur le client
      if(client.available()){

        char c = client.read(); //Cette ligne lit un octet (caractère) de données à partir du client 
                                //et le stocke dans la variable c.
        Serial.write(c);
        http_request_header += c;

        if(c == '\n'){  //Cette condition vérifie si le caractère lu est un saut de ligne (\n). 
                        //Cela indique la fin d'une ligne dans la requête HTTP envoyée par le client.
          if(current_line.length() == 0){
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html>");
            client.println("<html lang=\"en\">");
            client.println("<head>");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<title>Humidity Data</title>");
            client.println("<style>");
            client.println(".encadre {");
            client.println("border: 2px solid #000;");
            client.println("padding: 10px;");
            client.println("width: 200px;");
            client.println("text-align: center;");
            client.println("}");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<div class=\"encadre\">");
            client.print("<p>Humidity: <span id=\"humidityValue\">");
            client.print(humidityString);
            client.println("</span></p>");
            client.println("</div>");
            client.println("</body>");
            client.println("<script>");
            client.println("var humidity = document.getElementById(\"humidityValue\").textContent;");
            client.println("</script>");
            client.println("</html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;

          }else { // if you got a newline, then clear currentLine
            current_line = "";
          }
        }else if(c != '\r'){
          current_line += c;
        }
      }
    }

    // Clear the header variable
    http_request_header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    
    
    //last_time = millis();

  }
  
}