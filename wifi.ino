#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = "9496be6a87454dda8cd36bb9cefb54c0";

// Your WiFi credentials.
char ssid[] = "AndroidAP";
char pass[] = "12345678";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial1

// or Software Serial on Uno, Nano...
//#include <SoftwareSerial.h>
//SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

BlynkTimer timer;

int besucherHeute = 0
int besucherAktuell = 0
int besucherGegangen = 0

// This function sends the values to be displayed to Virtual Pin 0,1,2.
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, besucherHeute);
  Blynk.virtualWrite(V1, besucherAktuell);
  Blynk.virtualWrite(V2, besucherGegangen);
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, wifi, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, wifi, ssid, pass, IPAddress(192,168,1,100), 8442);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

// Nur noch werte setzen für die besucher werte und fertig
