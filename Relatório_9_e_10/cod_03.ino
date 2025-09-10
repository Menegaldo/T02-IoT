#include <ESP8266WiFi.h>     // Wi-Fi no ESP8266
#include <PubSubClient.h>    // Cliente MQTT

// --- Wi-Fi ---
const char* WIFI_SSID     = "RedeWiFi";
const char* WIFI_PASSWORD = "senhaRedeWiFi";

// --- MQTT ---
const char* MQTT_BROKER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;
const char* TOPIC_CMD   = "casa/Relatorio_redes/seuNome/lampada"; // mensagens: ON / OFF

// --- LED da placa ---
#ifndef LED_BUILTIN                 // No ESP8266 (NodeMCU/Wemos) é o GPIO2 (D4)
#define LED_BUILTIN 2
#endif

WiFiClient   net;                   // Socket TCP
PubSubClient mqtt(net);             // Cliente MQTT usando o socket

// ID único para o cliente MQTT (usa ChipID do ESP8266)
String makeClientId() {
  uint32_t chip = ESP.getChipId();
  char id[20];
  snprintf(id, sizeof(id), "ESP8266-%06X", chip);
  return String(id);
}

// Callback: trata mensagens recebidas
void onMsg(char* topic, byte* payload, unsigned int len) {
  String msg; msg.reserve(len);
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];

  Serial.printf("[MQTT] %s => %s\n", topic, msg.c_str());

  if (strcmp(topic, TOPIC_CMD) == 0) {
    if (msg.equalsIgnoreCase("ON"))  { digitalWrite(LED_BUILTIN, HIGH); Serial.println("[LED] ON"); }
    if (msg.equalsIgnoreCase("OFF")) { digitalWrite(LED_BUILTIN, LOW);  Serial.println("[LED] OFF"); }
  }
}

// Conexão Wi-Fi (bloqueia até conectar)
void connectWiFi() {
  Serial.printf("[WiFi] Conectando a '%s'...\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(400); Serial.print("."); }
  Serial.printf("\n[WiFi] OK — IP: %s\n", WiFi.localIP().toString().c_str());
}

// Conexão MQTT + subscribe ao tópico de comando
void connectMQTT() {
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(onMsg);

  while (!mqtt.connected()) {
    String cid = makeClientId();                // evita colisão no broker público
    Serial.printf("[MQTT] Conectando como %s...", cid.c_str());

    if (mqtt.connect(cid.c_str())) {
      Serial.println("OK");
      if (mqtt.subscribe(TOPIC_CMD))
        Serial.printf("[MQTT] Subscrito: %s\n", TOPIC_CMD);
      else
        Serial.printf("[MQTT] Falha no subscribe (state=%d)\n", mqtt.state());
    } else {
      Serial.printf(" falhou (rc=%d). Nova tentativa em 2s...\n", mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Pisca rápido (autoteste)
  for (int i = 0; i < 2; i++) { digitalWrite(LED_BUILTIN, HIGH); delay(120); digitalWrite(LED_BUILTIN, LOW); delay(120); }

  connectWiFi();
  connectMQTT();

  Serial.println("[READY] Publique ON/OFF no tópico para controlar o LED.");
}

void loop() {
  // Reestabelece MQTT se cair e processa tráfego MQTT
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();
}
