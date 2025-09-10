#include <ESP8266WiFi.h>   // Biblioteca correta para ESP8266

const char* ssid     = "NomeDaRede";      // SSID do Wi-Fi
const char* password = "SenhaSecreta";    // Senha do Wi-Fi

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");   // imprime pontos enquanto aguarda conectar
  }

  Serial.println("\nConectado ao Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Seu código aqui...
}
