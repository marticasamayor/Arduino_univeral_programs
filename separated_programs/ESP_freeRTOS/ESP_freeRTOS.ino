#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FreeRTOS.h>

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int variableCompartida = 0;
SemaphoreHandle_t xMutex;

// void Tasca1Buzzer(void *pvParameters){
//   pinMode(5, OUTPUT);

//   while(true){
//     digitalWrite(5, !digitalRead(5));
//     delay(1000);
//   }
// }

// void Tasca2Pantalla(void *pvParameters){
//   display.begin(i2c_Address, true); // Address 0x3C default
//  //display.setContrast (0); // dim display
//   display.display();
//   delay(500);
//   display.clearDisplay();
//   while(true){
//     display.setTextSize(1);
//     display.setTextColor(SH110X_WHITE);
//     display.setCursor(0, 0);
//     display.println("Testejant FreeRTOS");

//     display.display();
//     delay(2000);
//     display.clearDisplay();

//     display.display();
//     delay(500);
//   }
// }

void Tasca3Increment(void *pvParameters){
  for(;;){
    if(xSemaphoreTake(xMutex, portMAX_DELAY)){
      variableCompartida++;
      // Serial.print("IncrementTask: Incrementat a ");
      // Serial.println(variableCompartida);
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // delay(500);
  }
}

void Tasca4Print(void *pvParameters){
  for(;;){
    // if(xSemaphoreTake(xMutex, portMAX_DELAY)){
      Serial.println("PrintTask: Valor actual = ");
      // Serial.println(variableCompartida);
      // vTaskDelay(100 / portTICK_PERIOD_MS);
      // xSemaphoreGive(xMutex);
    //}
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // delay(1000);
  }
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Comença");

  xMutex = xSemaphoreCreateMutex();
  if (xMutex == NULL) {
    Serial.println("Error creant el semàfor!");
    // while (1); // Atura el programa
  }


  // xTaskCreate(Tasca3Increment, "TascaIncrement", 1024, NULL, 1, NULL);

  xTaskCreate(Tasca4Print, "TascaPrint", 1024, NULL, 2, NULL);


}

void loop() {

}
