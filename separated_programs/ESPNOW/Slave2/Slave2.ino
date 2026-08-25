// afegit lectura segons estructura


#include <FastLED.h>
#define NUM_LEDS 1
#define DATA_PIN 6
CRGB leds[NUM_LEDS];


#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
int state = 0;

// #define CRYPTO_KEY "PASSWORD12345678"//"PASSWORD1"  // La mateixa clau de xifratge que en el dispositiu broker


typedef struct {
  // bool estat;
  char topic[50];
  char payload[50];
} struct_message;


// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    Serial.print("AP CHANNEL "); Serial.println(WiFi.channel());
  }
}


void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
           recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);

  // Interpretar les dades com a estructura
  const struct_message* msg = (const struct_message*) data;
  Serial.print("Topic: "); Serial.println(msg->topic);
  Serial.print("Payload: "); Serial.println(msg->payload);
  Serial.println();

  // digitalWrite(19, (state) ? HIGH : LOW);
  // digitalWrite(21, (state) ? HIGH : LOW);

  if(state){
    leds[0] = CRGB::Red;
    FastLED.show();
  }else{
    leds[0] = CRGB::Black;
    FastLED.show();
  }
  state = !state;
}


void setup() {
  //pinMode(3, OUTPUT);
  //digitalWrite(3, HIGH);
  // pinMode(19, OUTPUT);
  // pinMode(21, OUTPUT);

  Serial.begin(115200);
  Serial.println("ESPNow/Basic/Slave Example");
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  //Set device in AP mode to begin with
  delay(1000);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());


  WiFi.mode(WIFI_AP);
  // WiFi.mode(WIFI_AP_STA);

  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();

  // esp_now_set_pmk((uint8_t *)CRYPTO_KEY);

  // esp_now_peer_info_t peerInfo = {};
  // uint8_t masterMac[] = { 0x60, 0x55, 0xf9, 0x69, 0xb3, 0x78 }; // <-- 60:55:f9:69:b3:78
  // memcpy(peerInfo.peer_addr, masterMac, 6);
  // peerInfo.channel = 0;
  // peerInfo.encrypt = true;

  // if (esp_now_add_peer(&peerInfo) != ESP_OK) {
  //   Serial.println("Failed to add peer");
  // }


  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

  delay(100);
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(100);
  leds[0] = CRGB::Black;
  FastLED.show();
}



void loop() {

}
