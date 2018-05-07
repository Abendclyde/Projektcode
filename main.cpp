/*  "Sonnennachführung"
    Code von Niklas Puls
*/
// Libraries:
#include <Arduino.h>
#include <Stepper.h>

// Deklarierungen:
int SPU = 360;
int steps = 1;
Stepper Motorx(SPU, 0,2,1,3);
Stepper Motory(SPU, 4,6,5,7);
int dir = 0;
double valueMid, valueNext = 0;

// Functions und Voids:
double light(int sensor){
  double summe = 0;
  for(int i; i <= 5000; i++){
    summe += analogRead(sensor);
  }
  return (summe/5000);
}

void directs(int dir) {
  switch (dir) {
    case 0:
      Motorx.step(-steps);
      break;
    case 1:
      Motory.step(steps);
      break;
    case 2:
      Motorx.step(steps);
      break;
    case 3:
      Motory.step(-steps);
      break;
  }
}

void setup() {
  Motorx.setSpeed(1);
  Motory.setSpeed(1);
  valueMid = light(A0);
}


void loop() {
  directs(dir);
  valueNext = light(A0);
  if(valueNext > valueMid){
    valueMid = valueNext;
  } else {
    directs((dir + 2) % 4);
    dir = dir + 1;
  }
}
