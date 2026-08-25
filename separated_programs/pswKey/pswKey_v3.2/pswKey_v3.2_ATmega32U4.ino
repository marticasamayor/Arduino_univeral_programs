// push button to write user and password on windows
// code writed arduino Pro Micro with ATmega32U4
// by Martí Casamayor (github.com/CasamaMaker/Arduino_univeral_programs)

#include "Keyboard.h"

unsigned long time;
int num=0;
int temp;
float temps;


/////////////// Contrasenya actual /////////////////The password is splited in two parts by '@' character
//ejemple: password1Part1@password1Part2

String usuari =             "user1";
String contrasenyaPart1  =  "password1Part1";
String contrasenyaPart2  =  "password1Part2";
bool contrasenyaPartida  =  true;

////////////// Contrasenya anterior ////////////////and there's a second psw (with same user)
String contrasenyaPart1_old  = "password2Part1";
String contrasenyaPart2_old  = "password2Part1";
bool contrasenyaPartida_old  = true;



/////////////////// Programa  /////////////////////
void setup() {
  Keyboard.begin();
  pinMode(9,INPUT_PULLUP);    //9//promicro: 2 / Beetle: A2-20  /  picoclick_c3t: 3
  Serial.begin(115200);
  escriuContrasenyaAmbSimbol();  
}

void escriuContrasenyaAmbSimbol(){
  Keyboard.print(contrasenyaPart1);
  Keyboard.press(KEY_RIGHT_ALT);
  Keyboard.write('2');
  Keyboard.release(KEY_RIGHT_ALT);
  Keyboard.print(contrasenyaPart2);
  //Keyboard.write('\t');
}

void escriuContrasenyaAmbSimbol_old(){
  Keyboard.print(contrasenyaPart1_old);
  Keyboard.press(KEY_RIGHT_ALT);
  Keyboard.write('2');
  Keyboard.release(KEY_RIGHT_ALT);
  Keyboard.print(contrasenyaPart2_old);
  //Keyboard.write('\t');
}

void loop() {
  bool anterior = digitalRead(2);
  delay(20);
  bool ara = digitalRead(2);
  if(ara == LOW && ara != anterior){    //flag de pujada
      num ++;
      if(num == 1){
        temps = millis();
      }
      Serial.print(".");      
  }

  if((millis()> temps + 1000)&&(num>0)){
    Serial.println(num);
    switch (num){
      case 2:
        if(contrasenyaPartida){
          escriuContrasenyaAmbSimbol();
        }else{
          Keyboard.print(contrasenyaPart1);
        }
        break;
      case 1:
        Keyboard.print(usuari);
        Keyboard.write('\t');
        if(contrasenyaPartida){
          escriuContrasenyaAmbSimbol();
        }else{
          Keyboard.print(contrasenyaPart1);
        }
        //Serial.println(contrasenyaPart2);
        delay(3000);
        Keyboard.write('\t');
        Keyboard.write('\t');
        break;
      case 3:
        Keyboard.print(usuari);
        break;
      case 4:
        if(contrasenyaPartida_old){
          escriuContrasenyaAmbSimbol_old();
        }else{
          Keyboard.print(contrasenyaPart1_old);
        }
        break;
      }
    num=0;
  }
}
