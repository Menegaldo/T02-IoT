#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "SeuSSID";
const char* WIFI_PASS = "SuaSenha";
const char* MQTT_HOST = "test.mosquitto.org";
const int   MQTT_PORT = 1883;

const char* TOPIC_CMD = "lab/lampada/cmd";

#ifndef LED_BUILTIN
#define LED_BUILTIN 2           // fallback (D4/GPIO2)
#endif
const uint8_t LED_PIN = LED_BUILTIN;
const uint8_t LED_ON  = LOW;    // acende
const uint8_t LED_OFF = HIGH;   // apaga

WiFiClient net;
PubSubClient mqtt(net);

String cid() {
  char id[22];
  snprintf(id, sizeof(id), "ESP8266-RCV-%06X", ESP.getChipId());
  return String(id);
}

void setLed(bool on) { digitalWrite(LED_PIN, on ? LED_ON : LED_OFF); }

void onMsg(char* topic, byte* payload, unsigned int len) {
  String msg; msg.reserve(len);
  for (unsigned int i=0;i<len;i++) msg += (char)payload[i];
  msg.trim(); msg.toUpperCase();

  if (strcmp(topic, TOPIC_CMD)==0) {
    if (msg == "ON")  setLed(true);
    if (msg == "OFF") setLed(false);
    Serial.printf("[RCV] %s => %s\n", topic, msg.c_str());
  }
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(300);
}

void mqttConnect() {
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMsg);
  while (!mqtt.connected()) {
    if (mqtt.connect(cid().c_str())) {
      mqtt.subscribe(TOPIC_CMD);
    } else {
      delay(1500);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  setLed(false);                 // começa apagado
  wifiConnect();
  mqttConnect();
}

void loop() {
  if (!mqtt.connected()) mqttConnect();
  mqtt.loop();
}
