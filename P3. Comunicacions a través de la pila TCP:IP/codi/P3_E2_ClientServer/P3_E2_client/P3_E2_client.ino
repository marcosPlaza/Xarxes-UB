#include "ESP8266WiFi.h"
#include <ArduinoJson.h>

String ssid, password;
int contador;
bool menu_printed;

WiFiClient client;

const char* server = "192.168.4.1";
bool server_connected = false;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA); //Station mode selected
  WiFi.disconnect();
  menu_printed = false;
  delay(2000);
  Serial.println("setup done");
}

void loop() {
  if (!menu_printed) {
    Serial.println("\nWhat do you want? (enter the number)\n");
    Serial.println("1. Scan networks");
    Serial.println("2. Connect to network");
    Serial.println("3. Chat");
    Serial.println("4. Disconnect from network\n");
    menu_printed = true;
  }

  while (!Serial.available());
  String option = Serial.readString();

  option = option.substring(0, option.length() - 1);

  if (option.equals("1")) {
    Serial.println("Scan networks selected:\n");
    scan();
  } else if (option.equals("2")) {
    Serial.println("Connect to network selected:\n");

    if (WiFi.status() != WL_CONNECTED) {
      wifiNetworkSelection();

      ssid = ssid.substring(0, ssid.length() - 1);
      password = password.substring(0, password.length() - 1);

      //we need a char array
      char cssid[ssid.length()];
      char cpsw[password.length()];

      ssid.toCharArray(cssid, ssid.length() + 1);
      password.toCharArray(cpsw, password.length() + 1);

      connectToWiFi(cssid, cpsw);
    } else {
      Serial.println("You are already connected");
    }
  } else if (option.equals("3")) {
    if (WiFi.status() == WL_CONNECTED) {
      chat();
    } else {
      Serial.println("Not connected\n");
    }
  } else if (option.equals("4")) {
    Serial.println("Disconnect from network selected:\n");
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect();
      Serial.println("You have been disconnected");
    } else {
      Serial.println("You are already disconnected");
    }
  } else {
    Serial.println("Not available option");
  }
  menu_printed = false;
}

void chat(){
  server_connected = client.connect(server,80);
  if(server_connected){
    Serial.print("Introdueix nom d'usuari: ");
    while(!Serial.available());
    String userName = Serial.readString();
    Serial.println(userName);
    
    userName = userName.substring(0, userName.length() - 1);
    
    Serial.println("You can now write to the chat");
    for(;;){
      if(Serial.available()){
        String message = Serial.readString();
        if(message.equals("\n")){
          break;
        }
        
        message = message.substring(0, message.length() - 1);

        send_message(userName,message);
        
        delay(20);
      }
      if(client.connected() && client.available())
      {
        receive_message();
      }
    }
  }
  client.stop();
}

void send_message(String userName, String message){
  DynamicJsonDocument doc(1024);
  doc["UserName"] = userName;
  doc["Message"] = message;
  if (server_connected) {
    client.print("POST /api/Users HTTP/1.1");
    client.print("content-type: application/json");
    client.print("Host: ");
    client.print(server); //IP del server
    client.print("Connection: close");
    client.println();
    serializeJson(doc, client);
  }
}

void receive_message(){
  String data = "";
  DynamicJsonDocument doc(1024);
  
  boolean currentLineIsBlank = true;
  boolean finish = false;
  while(client.available()) {
    char c = client.read();
    // if you've gotten to the end of the line (received a newline
    // character) and the line is blank, the http request has ended,
    // so you can send a reply
    if (c == '\n' && currentLineIsBlank) {

    // Here is where the POST data is. 
      while(client.available())
      {
        char c = client.read();
        data += c;
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
    }
  }
}

void scan() {
  Serial.println("Scan start");

  int n = WiFi.scanNetworks();
  if (n == 0)
    Serial.println("no networks found");
  else {
    for (int i = 0; i < n; i++) {
      //print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(" ");
      Serial.print(WiFi.SSID(i));
      Serial.print("(");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
}

void connectToWiFi(char cssid[], char cpsw[]) {
  // put your setup code here, to run once:

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(cssid, cpsw);
    bool printed = false;
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      if (!printed){
        Serial.print("Connecting.");
        printed = true;
      }else{
        Serial.print(".");
      }
      contador++;
      if (contador == 10) {
        break;
      }

    }
    Serial.println("");
    if(WiFi.status() == WL_CONNECTED){
      Serial.println(WiFi.localIP());
    }
    else{
      Serial.println("Connection failed");
    }
  }
}

void wifiNetworkSelection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("SSID? >");
    while (!Serial.available());

    ssid = Serial.readString();
    //Serial.readStringUntil("\n");
    Serial.print("SSID selected > ");
    Serial.println(ssid);
    delay(1000);

    Serial.print("Password? > ");
    while (!Serial.available());

    password = Serial.readString();
    Serial.println(password);
    delay(1000);
  }
}
