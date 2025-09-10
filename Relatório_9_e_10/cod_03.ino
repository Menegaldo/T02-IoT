#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --- Wi-Fi ---
const char* WIFI_SSID     = "RedeWiFi";
const char* WIFI_PASSWORD = "senhaRedeWiFi";

// --- MQTT ---
const char* MQTT_BROKER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;
const char* TOPIC_CMD   = "casa/Relatorio_redes/seuNome/lampada"; // ON / OFF

// --- LED da placa (ativo em LOW) ---
#ifndef LED_BUILTIN
#define LED_BUILTIN 2              // fallback
#endif
const uint8_t LED_PIN = LED_BUILTIN;
const uint8_t LED_ON  = LOW;       // **corrigido**: acende em LOW
const uint8_t LED_OFF = HIGH;      // **corrigido**: apaga em HIGH

WiFiClient   net;
PubSubClient mqtt(net);

// ID único para o cliente MQTT
String makeClientId() {
  char id[20];
  snprintf(id, sizeof(id), "ESP8266-%06X", ESP.getChipId());
  return String(id);
}

// Mensagens recebidas
void onMsg(char* topic, byte* payload, unsigned int len) {
  String msg; msg.reserve(len);
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  Serial.printf("[MQTT] %s => %s\n", topic, msg.c_str());

  if (strcmp(topic, TOPIC_CMD) == 0) {
    if (msg.equalsIgnoreCase("ON"))  { digitalWrite(LED_PIN, LED_ON);  Serial.println("[LED] ON");  }
    if (msg.equalsIgnoreCase("OFF")) { digitalWrite(LED_PIN, LED_OFF); Serial.println("[LED] OFF"); }
  }
}

// Wi-Fi
void connectWiFi() {
  Serial.printf("[WiFi] Conectando a '%s'...\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(400); Serial.print("."); }
  Serial.printf("\n[WiFi] OK — IP: %s\n", WiFi.localIP().toString().c_str());
}

// MQTT
void connectMQTT() {
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(onMsg);
  while (!mqtt.connected()) {
    String cid = makeClientId();
    Serial.printf("[MQTT] Conectando como %s...", cid.c_str());
    if (mqtt.connect(cid.c_str())) {
      Serial.println("OK");
      if (mqtt.subscribe(TOPIC_CMD))
        Serial.printf("[MQTT] Subscrito: %s\n", TOPIC_CMD);
      else
        Serial.printf("[MQTT] Falha subscribe (state=%d)\n", mqtt.state());
    } else {
      Serial.printf(" falhou (rc=%d). Tentando em 2s...\n", mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);         // inicia apagado

  // autoteste (pisca com a nova lógica)
  for (int i = 0; i < 2; i++) { 
    digitalWrite(LED_PIN, LED_ON);  delay(120);
    digitalWrite(LED_PIN, LED_OFF); delay(120);
  }

  connectWiFi();
  connectMQTT();
  Serial.println("[READY] Publique ON/OFF no tópico para controlar o LED.");
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();
}
