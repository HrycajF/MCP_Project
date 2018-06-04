/*
    Ultrasonic sensor 1 pins (S1):
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin 3
        Echo: Echo (OUTPUT) - Pin 4
        GND: GND

    Ultrasonic sensor 2 pins (S2):
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin 6
        Echo: Echo (OUTPUT) - Pin 7
        GND: GND
*/

// Sensor 1
int trigPinS1 = 3;    // Trig
int echoPinS1 = 4;    // Echo
long durationS1;
long distanceS1;

// Sensor 2
int trigPinS2 = 6;    // Trig
int echoPinS2 = 7;    // Echo
long durationS2;
long distanceS2;

//
int triggerDistance = 50;
int triggeredS1 = 0;
int triggeredS2 = 0;
int inOverall = 0;
int outOverall = 0;
int currentIn = 0;
int goingIn = 0;
int goingOut = 0;
int passed = 1;

void setup() {
  // Serial port begin
  Serial.begin (9600);

  // Define inputs and outputs
  pinMode(trigPinS1, OUTPUT);
  pinMode(echoPinS1, INPUT);
  pinMode(trigPinS2, OUTPUT);
  pinMode(echoPinS2, INPUT);
}

void loop() {
  cleanHighPulse(trigPinS1);
  durationS1 = getDuration(echoPinS1);
  distanceS1 = calculateDistance(durationS1);

  cleanHighPulse(trigPinS2);
  durationS2 = getDuration(echoPinS2);
  distanceS2 = calculateDistance(durationS2);

  // Print distances
  // Serial.print(distanceS1);
  // Serial.print(" cm (S1);  ");
  // Serial.print(distanceS2);
  // Serial.print(" cm (S2)");
  Serial.print(" inOverall: ");
  Serial.print(inOverall);
  Serial.print(" outOverall: ");
  Serial.print(outOverall);
  Serial.print(" currentIn: ");
  Serial.print(currentIn);
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
  Serial.println();

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
              inOverall++;
              currentIn++;
          }

          if(goingOut == 1) {
              triggeredS1 = 0;
              triggeredS2 = 0;
              goingOut = 0;
              passed = 1;
              outOverall++;
              currentIn--;
          }
      }
  }


    // if distanceS1 is within triggerDistance set triggeredS1 to 1
    // then set cycle to 10


    // if distanceS2 is within triggerDistance set triggeredS2 to 1



  // Max. 50 measurements per second (delay: 20 millis)
  // Delay smaller than 20 millis equals a permanent measurement
  delay(50);
}

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
