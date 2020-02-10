#include "ESP8266WiFi.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  Serial.println("Setup done");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Scan start");
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n==0){
    Serial.println("no networks found");
  }else{
    for (int i=0; i<n; i++){
      //print SSID and RSSI for each network found
      Serial.print(i+1);
      Serial.print(" ");
      Serial.print(WiFi.SSID(i));
      Serial.print("(");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  Serial.println("******************************");
  // Wait a bit before scanning again
  delay(5000);
}
