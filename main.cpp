/*
    "Sonnennachführung"
    Code von Niklas Puls
*/

// Libraries:
#include <Arduino.h>
#include <Stepper.h>
#include <EEPROM.h>

// Deklarierungen:
const int SPU = 360;
const int steps = 5;
int velo = 1;
Stepper Motorx(SPU, 8,10,9,11);
Stepper Motory(SPU, 3,5,4,6);
int dir = 0;
double valueMid, valueNext = 0;
const int SW_pin = 13;
const int Y_pin = A1;
const int X_pin = A2;
bool repeat1, repeat2, repeat3 = true;
bool power = false;
bool giveValues = false;
bool change = false;
bool changeVelo = true;
char nextAction = 0;
char turnOff = 'N';
int y, x, z = 0;

// Functions und Voids:
double light(int sensor){
  double average = 0;
  for(int i; i <= 5000; i++){
    average += analogRead(sensor);
  }
  return (average/5000);
}

void getSpeed(){
  int oldVelo = 0;
  while(changeVelo){
    y = analogRead(Y_pin);
    if(y > 470 && y < 570){
      change = true;
      }
    if(y < 470 || y > 570){
      if(y < 470){
        if(change){
          if(velo < 10){
          velo++;
          EEPROM.write(0, velo);
          } else {
            Serial.println("Reached Maximum!");
          }
          if(velo != oldVelo){
            Serial.print("> Velocity: ");
            Serial.println(velo);
          }
          oldVelo = velo;
          change = false;
        }
      } else {
        if(change){
          if(velo > 1){
          velo--;
          EEPROM.write(0, velo);
          } else {
            Serial.println("Reached Minimum!");
          }
          if(velo != oldVelo){
            Serial.print("> Velocity: ");
            Serial.println(velo);
          }
          oldVelo = velo;
          change = false;
        }
      }
    }
    z = digitalRead(SW_pin);
    if(z < 1){
      changeVelo = false;
    }
  }
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

void menu(){
  Serial.println("Controls:");
  Serial.println("> 0: Lock / Unlock");
  Serial.println("> 1: Manual");
  Serial.println("> 2: Automatic");
  Serial.println("> 3: Show / Hide Values");
  Serial.println("> 4: Change Speed");
  Serial.println("> 5: Exit");
  Serial.println("-----");
  Serial.println("Current Settings:");
  Serial.print("> Locked: ");
  if(power){
    Serial.println("False");
  } else {
    Serial.println("True");
  }
  Serial.print("> Show Values: ");
  if(giveValues){
    Serial.println("True");
  } else {
    Serial.println("False");
  }
  Serial.print("> Motor Speed: ");
  Serial.println(velo);
  Serial.println("");
  Serial.print(">> ");
}

void countdown(){
  for(int val = 5; val > 0; val--){
    Serial.println(val);
    delay(1000);
  }
}

void clearScreen(){
  for(int i = 100; i > 0; i--){
    Serial.println("");
  }
}

void setup() {
  Serial.begin(9600);
  Motorx.setSpeed(velo);
  Motory.setSpeed(velo);
  valueMid = light(A0);
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.println("Started!");
  menu();
  velo = EEPROM.read(0);
}



void loop() {
  if(Serial.available() > 0){
    nextAction = Serial.read();
    switch (nextAction) {
      case '0':
        power = !power;
        if(power){
          Serial.println("Unlocked!");
          delay(1000);
          clearScreen();
          menu();
        } else {
          Serial.println("Locked!");
          delay(1000);
          clearScreen();
          menu();
        }
        break;
      case '1':
        if(!repeat1){
          repeat1 = true;
        }
        if(power){
          Serial.println("Manual Control");
          delay(1000);
          clearScreen();
          while(repeat1){
            if(giveValues){
              Serial.print("X: ");
              Serial.println(x);
              Serial.print("Y: ");
              Serial.println(y);
              Serial.print("Switch: ");
              Serial.println(z);
              Serial.println("");
            }
            y = analogRead(Y_pin);
            if(y < 470 || y > 570){
              if(y < 470){
                Motorx.step(-steps);
              } else if(y > 570) {
                Motorx.step(steps);
              }
            }
            x = analogRead(X_pin);
            if(x < 450 || x > 550){
              if(x < 450){
                Motory.step(-steps);
              } else if(x > 550){
                Motory.step(steps);
              }
            }
            z = digitalRead(SW_pin);
            if(z < 1){
              repeat1 = false;
              nextAction = 0;
              clearScreen();
              menu();
            }
          }
        } else {
          Serial.println("Locked!");
          delay(1000);
          clearScreen();
          menu();
        }
        break;
      case '2':
        while(repeat2){
        if(power){
          directs(dir);
          valueNext = light(A0);
          if(valueNext > valueMid){
            valueMid = valueNext;
          } else {
            directs((dir + 2) % 4);
            dir = (dir + 1) % 4;
          }
        } else {
          Serial.println("Locked!");
          delay(1000);
          clearScreen();
          menu();
        }
        z = digitalRead(SW_pin);
        if(z < 1){
          repeat2 = false;
        }
        }
        repeat2 = true;
        break;
      case '3':
        if(power){
        giveValues = !giveValues;
        if(giveValues){
          Serial.println("Values shown!");
          delay(1000);
          clearScreen();
          menu();
        } else {
          Serial.println("Values hidden!");
          delay(1000);
          clearScreen();
          menu();
        }
        } else {
          Serial.println("Locked!");
          delay(1000);
          clearScreen();
          menu();
        }
        break;
      case '4':
        if(power){
          Serial.println("Change Speed");
          delay(1000);
          clearScreen();
          getSpeed();
          Serial.print("> Velocity: ");
          Serial.println(velo);
          Serial.print("Velocity changed to ");
          Serial.print(velo);
          Serial.println("!");
          changeVelo = true;
          delay(1000);
          clearScreen();
          menu();
        } else {
          Serial.println("Locked!");
          delay(1000);
          clearScreen();
          menu();
        }
        break;
      case '5':
        Serial.println("Are you sure?");
        Serial.println("Enter Y or N");
        repeat3 = true;
        while(repeat3){
        turnOff = Serial.read();
        switch (turnOff){
          case 'Y':
            Serial.println("Stopped!");
            delay(1000);
            clearScreen();
            for(int i = 1; i > 0; i++){}
            break;
          case 'N':
            Serial.println("Canceled");
            repeat3 = false;
            delay(1000);
            clearScreen();
            menu();
            break;
        }
        }
        break;
  }
  }
}
