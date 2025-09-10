#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "SeuSSID";
const char* WIFI_PASS = "SuaSenha";
const char* MQTT_HOST = "test.mosquitto.org";
const int   MQTT_PORT = 1883;

const char* TOPIC_CMD = "lab/lampada/cmd";  // receptor assina aqui

WiFiClient net;
PubSubClient mqtt(net);

String cid() {
  char id[22];
  snprintf(id, sizeof(id), "ESP8266-SND-%06X", ESP.getChipId());
  return String(id);
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(300);
}

void mqttConnect() {
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  while (!mqtt.connected()) {
    if (!mqtt.connect(cid().c_str())) { delay(1500); }
  }
}

void pubCmd(const char* msg) {
  mqtt.publish(TOPIC_CMD, msg, false);
  Serial.print("[PUB] "); Serial.println(msg);
}

void setup() {
  Serial.begin(115200);
  wifiConnect();
  mqttConnect();
  Serial.println("Digite: on / off (Serial Monitor, NL+CR).");
}

void loop() {
  if (!mqtt.connected()) mqttConnect();
  mqtt.loop();

  // Comando via Serial
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n'); s.trim(); s.toLowerCase();
    if (s == "on")  pubCmd("ON");
    if (s == "off") pubCmd("OFF");
  }
}
