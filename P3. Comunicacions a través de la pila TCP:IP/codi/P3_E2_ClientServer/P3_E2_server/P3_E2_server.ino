#include "ESP8266WiFi.h"
#include <ArduinoJson.h>

char ssid[] = "Conectate aqui Marcos";
char password[] = "12345678";

WiFiServer server(80); //port del server

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP); //Acces mode selected
  WiFi.disconnect();
  WiFi.softAP(ssid,password);
  //WiFi.begin();
  delay(1000);
  
  server.begin();
  Serial.println("\nWiFi ON");
  Serial.print("Local IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Server port: ");
  Serial.println("80");
}

void loop() {
  String data = "";
  DynamicJsonDocument doc(1024);
  
  WiFiClient client = server.available();
  if (client == true) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      boolean finish = false;
      if(client.available()) {
        while(client.available()){
          char c = client.read();
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          client.write(c);
          if (c == '\n' && currentLineIsBlank) {
  
          // Here is where the POST data is. 
            while(client.available())
            {
              char c = client.read();
              data += c;
              client.write(c);
            }
            finish = true;
          }
          if (finish){
            finish = false;
            char c_data[data.length()];
            data.toCharArray(c_data,data.length()+1);
            
            deserializeJson(doc,c_data);
            const char *user_name = doc["UserName"];
            const char *message = doc["Message"];
            Serial.print(user_name);
            Serial.print(": ");
            Serial.println(message);
            //send_all(doc,client);
            //delay(20);
          }
        }
      }
      if(Serial.available()){
        send_all(client);
      }
      data = "";
    }
  }
}
void send_all(WiFiClient client){
  String message = Serial.readString();
  message = message.substring(0, message.length() - 1);
  Serial.print("Server: ");
  Serial.println(message);
  DynamicJsonDocument doc(1024);
  doc["UserName"] = "Server";
  doc["Message"] = message;
  if (client.connected()) {
    client.print("POST /api/Users HTTP/1.1");
    client.print("content-type: application/json");
    client.print("Host: ");
    client.print(WiFi.softAPIP()); //IP del server
    client.print("Connection: close");
    client.println();
    serializeJson(doc, client);
  }
}
