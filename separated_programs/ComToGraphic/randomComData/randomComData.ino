/*
  Arduino Pro Micro - Generador de dades fictícies
  Envia 3 valors separats per espais via Serial
  Compatible amb els dos plotters Python
*/

#include <Arduino.h>

float t = 0.0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Espera que el port sèrie estigui llest (important en Pro Micro)
  }
}

void loop() {
  // Variable 1: sinusoide
  float v1 = 50.0 * sin(t);

  // Variable 2: rampa (0 a 100)
  float v2 = fmod(t * 10.0, 100.0);

  // Variable 3: soroll aleatori
  float v3 = random(0, 100);

  // Enviar dades separades per espais
  Serial.print(v1, 2);
  Serial.print(" ");
  Serial.print(v2, 2);
  Serial.print(" ");
  Serial.println(v3);

  // Increment del temps
  t += 0.1;

  // Període d’enviament (~20 Hz)
  delay(50);
}
