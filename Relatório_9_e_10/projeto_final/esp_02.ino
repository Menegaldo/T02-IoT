#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "SeuSSID";
const char* WIFI_PASS = "SuaSenha";
const char* MQTT_HOST = "test.mosquitto.org";
const int   MQTT_PORT = 1883;

const char* TOPIC_CMD = "lab/lampada/cmd";

// ---- Lâmpada no D0 (GPIO16) ----
const uint8_t LAMP_PIN = D0;   // = GPIO16
const uint8_t LAMP_ON  = LOW;  // muitos relés são ativo-LOW
const uint8_t LAMP_OFF = HIGH; // se seu relé for ativo-HIGH, inverta estes dois

WiFiClient net;
PubSubClient mqtt(net);

String cid() {
  char id[22];
  snprintf(id, sizeof(id), "ESP8266-RCV-%06X", ESP.getChipId());
  return String(id);
}

void setLamp(bool on) { digitalWrite(LAMP_PIN, on ? LAMP_ON : LAMP_OFF); }

void onMsg(char* topic, byte* payload, unsigned int len) {
  String msg; msg.reserve(len);
  for (unsigned int i=0;i<len;i++) msg += (char)payload[i];
  msg.trim(); msg.toUpperCase();

  if (strcmp(topic, TOPIC_CMD)==0) {
    if (msg == "ON")  setLamp(true);
    if (msg == "OFF") setLamp(false);
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
    if (mqtt.connect(cid().c_str())) mqtt.subscribe(TOPIC_CMD);
    else delay(1500);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LAMP_PIN, OUTPUT);
  setLamp(false);                 // inicia desligada
  wifiConnect();
  mqttConnect();
}

void loop() {
  if (!mqtt.connected()) mqttConnect();
  mqtt.loop();
}
