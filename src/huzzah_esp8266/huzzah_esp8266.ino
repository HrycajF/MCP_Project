// REMOVE VARS inOverall, outOverall
// REMOVE Serial + prints


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

          if(currentIn >= 0) {
            Blynk.virtualWrite(V0, currentIn);
          } else {
            currentIn = 0;
          }
      }
  }

