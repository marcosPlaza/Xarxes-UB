#include "ESP8266WiFi.h"

String ssid, password;
int contador;
bool menu_printed;

WiFiClient client;

const int channelID = 912098;
String writeAPIKey = "R69RUMB0MWBP68X9";
const char* server = "api.thingspeak.com";
const int postingInterval = 20*1000;

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
    Serial.println("3. Connect to ThingSpeak");
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
      Serial.println("Enter any key to quit");
      while (!Serial.available()) {
        thingspeak();
      }
      option = Serial.readString();
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

void thingspeak(){
  if(client.connect(server,80)){
    long rssi = WiFi.RSSI();

    String body = "field1=";
    body += String(rssi);

    Serial.print("RSSI: ");
    Serial.println(rssi);

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266 (nothans)/1.0");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: "+writeAPIKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.println(body);
  }
  client.stop();
  delay(postingInterval);
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
      }else{
        Serial.print(".");
      }
      contador++;
      if (contador == 10) {
        break;
      }

    }
    Serial.println("");
    Serial.println(WiFi.localIP());
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
