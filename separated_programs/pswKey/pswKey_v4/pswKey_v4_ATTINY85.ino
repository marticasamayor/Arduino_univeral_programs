//Micro Digispark with  ATTINY85

#include "DigiKeyboard.h"

unsigned long time;
int num=0;
int temp;

void setup() {
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(1, LOW);
  digitalWrite(2, HIGH);
  
  pinMode(2,INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  bool anterior = digitalRead(0);
  //Serial.println(anterior);
  //Serial.println(" ");
  Serial.println(num);
  Serial.println(" ");
  delay(20);
  bool ara = digitalRead(0);
  if(ara == LOW && ara != anterior){
      num ++;
      temp = 50;
  }

  if(temp>0){
    temp--;
    if(temp==0){
      Serial.println(num);
      Serial.println(" ");
      switch (num){
      case 1:
        DigiKeyboard.print("unacontrasenyademerda1.0");
        DigiKeyboard.write('\t');
        break;
      case 2:
        DigiKeyboard.print("usuari");
        DigiKeyboard.write('\t');
        DigiKeyboard.print("unacontrasenyademerda1.0");
        //Keyboard.write('\t');
        delay(3000);
        DigiKeyboard.write('\t');
        DigiKeyboard.write('\t');
        break;
      case 3:
        DigiKeyboard.print("usuari");
        DigiKeyboard.write('\t');
        break;
      }
      num=0;
    }
  }
}//
