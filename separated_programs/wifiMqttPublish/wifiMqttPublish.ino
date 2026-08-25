/*
 * Código para ESP32 que se conecta a WiFi y envía un número aleatorio
 * a través de MQTT al tema diposit/ultrasons
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>

// Configuración WiFi - Reemplaza con tus credenciales
const char* ssid = "Mas Guantera";
const char* password = "Moia08180";

// Configuración del servidor MQTT
const char* mqtt_server = "cangoita.duckdns.org";
const int mqtt_port = 444;
const char* mqtt_user = "mqtt-user";      // Deja vacío si no se requiere
const char* mqtt_password = "CanGoitaMolaMolt"; // Deja vacío si no se requiere
const char* mqtt_topic = "diposit/ultrasons";
const char* clientID = "diposit";

// Intervalo de envío en milisegundos (5 segundos)
const long interval = 5000;
unsigned long previousMillis = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Bucle hasta que estemos reconectados
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    
    // Intentar conectar
    bool connected;
    if (mqtt_user && mqtt_password && strlen(mqtt_user) > 0) {
      connected = client.connect(clientID, mqtt_user, mqtt_password);
    } else {
      connected = client.connect(clientID);
    }
    
    if (connected) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inicializar la semilla para números aleatorios
  randomSeed(analogRead(0));
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  
  // Comprobar si es hora de enviar un nuevo mensaje
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Generar un número aleatorio entre 0 y 100
    int randomNumber = random(101);
    
    // Convertir el número a cadena
    char msg[10];
    itoa(randomNumber, msg, 10);
    
    Serial.print("Enviando mensaje: ");
    Serial.println(msg);
    
    // Publicar mensaje MQTT
    client.publish(mqtt_topic, msg);
  }
}