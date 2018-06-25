#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

/*
    Ultrasonic sensor 1 pins (S1):
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin 12
        Echo: Echo (OUTPUT) - Pin 14
        GND: GND

    Ultrasonic sensor 2 pins (S2):
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin 15
        Echo: Echo (OUTPUT) - Pin 13
        GND: GND

    WiFi:
        VCC: +3.3V

*/
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define SERIAL_BAUD 9600
#define ESP8266_BAUD 115200
#define RX 5
#define TX 6

#define BLYNK_GREEN   "#23C48E"
#define BLYNK_YELLOW  "#ED9D00"
#define BLYNK_RED     "#D3435C"


// Blynk App Auth Token
char auth[] = "863e91c92bad441e94b1577b2e31d088";

// WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SKYNET";
char pass[] = "";

// Software Serial
SoftwareSerial EspSerial(RX, TX);

// Sensor 1
int trigPinS1 = 12;    // Trig
int echoPinS1 = 14;    // Echo
long durationS1;
long distanceS1;

// Sensor 2
int trigPinS2 = 15;    // Trig
int echoPinS2 = 13;    // Echo
long durationS2;
long distanceS2;

// Shift register
int latchPin = 2;
int clockPin = 5;
int dataPin = 16;
byte leds = 0;

// Global vars
int triggerDistance = 25;
int triggeredS1 = 0;
int triggeredS2 = 0;
int currentIn = 5;
int initialIn = 5;
int goingIn = 0;
int goingOut = 0;
int passed = 1;
String message = "Raum frei";
int warning = 5;
int roomCapacity = 25;


// --- SETUP --------------------------------------------------------
void setup() {
  // Debug console
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  Serial.println("Started");

  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, ssid, pass, "192.168.1.101", 8080);
  Blynk.setProperty(V1, "max", roomCapacity);
  Blynk.virtualWrite(V0, initialIn);
  Blynk.virtualWrite(V1, initialIn);

  // Define inputs and outputs
  pinMode(trigPinS1, OUTPUT);
  pinMode(echoPinS1, INPUT);
  pinMode(trigPinS2, OUTPUT);
  pinMode(echoPinS2, INPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}
// --- END OF SETUP -------------------------------------------------


// --- LOOP ---------------------------------------------------------
void loop() {

  Blynk.run();

  cleanHighPulse(trigPinS1);
  durationS1 = getDuration(echoPinS1);
  distanceS1 = calculateDistance(durationS1);

  cleanHighPulse(trigPinS2);
  durationS2 = getDuration(echoPinS2);
  distanceS2 = calculateDistance(durationS2);

  // Print distances for debugging
  // Serial.print(distanceS1);
  // Serial.print(" cm (S1);  ");
  // Serial.print(distanceS2);
  // Serial.print(" cm (S2)");
  // Serial.print(" currentIn: ");
  // Serial.print(currentIn);
  // Serial.print(" triggeredS1: ");
  // Serial.print(triggeredS1);
  // Serial.print(" triggeredS2: ");
  // Serial.print(triggeredS2);
  // Serial.print(" goingIn: ");
  // Serial.print(goingIn);
  // Serial.print(" goingOut: ");
  // Serial.print(goingOut);
  // Serial.print(" passed: ");
  // Serial.print(passed);
  // Serial.println();


  // Check if someone goes in or out and increase or decrease counter
  //      S1    S2
  //      |      |

  // check if S1 or S2 or both are set to 1
  // if all are set to 0: do nothing
  if(triggeredS1 == 0 && triggeredS2 == 0 && passed == 1) {
      if(distanceS1 <= triggerDistance && distanceS2 >= triggerDistance) {
          triggeredS1 = 1;
          goingIn = 1;
          passed = 0;
      }

      if(distanceS1 >= triggerDistance && distanceS2 <= triggerDistance) {
          triggeredS2 = 1;
          goingOut = 1;
          passed = 0;
      }
      // For now ignore if both are below triggerDistance
  }

  if(goingIn == 1 && passed == 0) {
      if(distanceS2 <= triggerDistance) {
          triggeredS2 = 1;
      }
  }

  if(goingOut == 1 && passed == 0) {
      if(distanceS1 <= triggerDistance) {
          triggeredS1 = 1;
      }
  }

  if(triggeredS1 == 1 && triggeredS2 == 1 && passed == 0) {
      if(distanceS1 >= triggerDistance && distanceS2 >= triggerDistance) {
          if(goingIn == 1) {
              triggeredS1 = 0;
              triggeredS2 = 0;
              goingIn = 0;
              passed = 1;
              currentIn++;
          }

          if(goingOut == 1) {
              triggeredS1 = 0;
              triggeredS2 = 0;
              goingOut = 0;
              passed = 1;
              currentIn--;
          }

          checkCapacity();

        //   if(currentIn >= 0) {
        //     Blynk.virtualWrite(V0, currentIn);
        //     Blynk.virtualWrite(V1, currentIn);
        //     Blynk.virtualWrite(V2,message);
        //   } else {
        //     currentIn = 0;
        //   }
      }
  }


    // if distanceS1 is within triggerDistance set triggeredS1 to 1
    // then set cycle to 10


    // if distanceS2 is within triggerDistance set triggeredS2 to 1


  // Max. 50 measurements per second (delay: 20 millis)
  // Delay smaller than 20 millis equals a permanent measurement
  delay(50); //DEFAULT: 50
}

// --- END OF LOOP --------------------------------------------------

// The sensor is triggered by a HIGH pulse of 10 or more microseconds.
// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
void cleanHighPulse(int trigPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
}

// Read the signal from the sensor: a HIGH pulse whose
// duration is the time (in microseconds) from the sending
// of the ping to the reception of its echo off of an object.
long getDuration(int echoPin) {
    return pulseIn(echoPin, HIGH);
}

// Convert the time into a distance
long calculateDistance(long duration) {
    return (duration/2) / 29.1;
}

void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, MSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

void updateLeds(int value){
  leds = 0;
  updateShiftRegister();

  if(value >= 8){
    value = 8;
  }
  else if(value < 0){
    value = 0;
  }

  for (int i = 0; i < value; i++)
  {
    bitSet(leds, i);
    updateShiftRegister();
  }
  //delay(500);
}

//to prevent wrong values
void checkCapacity(){
  if(currentIn > roomCapacity){
    Blynk.setProperty(V0, "color", BLYNK_RED);
    Blynk.setProperty(V1, "color", BLYNK_RED);
    message = "Raum überfüllt!";
    updateLeds(8);
    sendCurrentIn();
  }
  else if(currentIn > (roomCapacity-warning)){
    Blynk.setProperty(V0, "color", BLYNK_YELLOW);
    Blynk.setProperty(V1, "color", BLYNK_YELLOW);
    message = "Raum fast voll!";
    int value = (currentIn*1.0/roomCapacity*1.0)*8;
    updateLeds(value);
    sendCurrentIn();
  }
  else if(currentIn < 0){
    currentIn = 0;
    updateLeds(0);
  }
  else{
    Blynk.setProperty(V0, "color", BLYNK_GREEN);
    Blynk.setProperty(V1, "color", BLYNK_GREEN);
    message = "Raum noch nicht voll";
    int value = (currentIn*1.0/roomCapacity*1.0)*8;
    updateLeds(value);
    sendCurrentIn();
  }
}

void sendCurrentIn() {
    Blynk.virtualWrite(V0, currentIn);
    Blynk.virtualWrite(V1, currentIn);
    Blynk.virtualWrite(V2, message);
}
