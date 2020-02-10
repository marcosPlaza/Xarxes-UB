#include "ESP8266WiFi.h"

String ssid, password; 
int contador;
bool menu_printed;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA); //Station mode selected
  WiFi.disconnect();
  menu_printed = false;
  delay(2000);
  Serial.println("setup done");
}

void loop() {
  //Mostrem les opcions
  if (!menu_printed) {
    Serial.println("\nWhat do you want? (enter the number)\n");
    Serial.println("1. Scan networks");
    Serial.println("2. Connect to network");
    Serial.println("3. Get RSSI");
    Serial.println("4. Disconnect from network\n");
    menu_printed = true;
  }

  //Recollim la opció de l'usuari
  while (!Serial.available());
  String option = Serial.readString();

  option = option.substring(0, option.length() - 1);

  //Depenent de la opció escollida anem a una de les 4 funcions
  if (option.equals("1")) {
    Serial.println("Scan networks selected:\n");
    scan(); //Escanejem les xarxes disponibles
  } else if (option.equals("2")) {
    Serial.println("Connect to network selected:\n");

    if (WiFi.status() != WL_CONNECTED) {
      wifiNetworkSelection(); //Recollim l'ssid i la password de la xarxa

      ssid = ssid.substring(0, ssid.length() - 1);
      password = password.substring(0, password.length() - 1);

      //we need a char array
      char cssid[ssid.length()];
      char cpsw[password.length()];

      ssid.toCharArray(cssid, ssid.length() + 1);
      password.toCharArray(cpsw, password.length() + 1);

      connectToWiFi(cssid, cpsw); //Un cop recollits el nom i la password ens intentem connectar a la xarxa
    } else {
      //Si ja estem connectats mostrarem un missatge per pantalla
      Serial.println("You are already connected");
    }
  } else if (option.equals("3")) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Enter any key to quit");
      while (!Serial.available()) { //Mentres no entrem cap dada pel monitor serial no deixem de mostrar 
        //l'RSSI de la xarxa
        Serial.println(WiFi.RSSI());
        delay(1000);
      }
      option = Serial.readString();
    } else {
      //Si no estem connectats no tenim res a mostrar
      Serial.println("Not connected\n");
    }
  } else if (option.equals("4")) {
    Serial.println("Disconnect from network selected:\n");
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect(); //Ens desconnectem de la xarxa
      Serial.println("You have been disconnected");
    } else {
      Serial.println("You are already disconnected");
    }
  //En cas contrari avisem que no s'ha seleccionat la opció correcta
  } else {
    Serial.println("Not available option");
  }
  menu_printed = false;
}

//Funció per escanejar les xarxes disponibles
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

//Funció per a connectar-nos a la Wi-Fi que nosaltres escollim dins de les disponibles
void connectToWiFi(char cssid[], char cpsw[]) {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(cssid, cpsw); //Passem nom de la xarxa i contrassenya com arguments de la funció WiFi.begin
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting...");
      contador++;
      if (contador == 10) { //Si ens passem del temps (10 segons) sortirem sense haver-nos connectat
        break;
      }

    }
    Serial.println(WiFi.localIP()); //Mostrem la IP del dispositiu al qual ens hem connectat
    //IP UNSET en cas de que no ens haguem connectat a la xarxa
  }
}

//Funció per a recollir l'ssid i la password
void wifiNetworkSelection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("SSID? >");
    while (!Serial.available());

    ssid = Serial.readString();
    
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
