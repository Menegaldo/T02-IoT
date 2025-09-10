#include <ESP8266WiFi.h>   // Biblioteca para ESP8266

// --- Configuração da rede Wi-Fi ---
const char* ssid     = "NomeDaRede";     // Nome da rede (SSID)
const char* password = "SenhaSecreta";  // Senha da rede

void setup() {
  Serial.begin(115200);           // Inicializa a serial para debug
  WiFi.begin(ssid, password);   // Inicia a conexão Wi-Fi

  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { // Aguarda até conectar
    delay(500);
    Serial.print(".");
  }

  // Conexão estabelecida
  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); // Mostra o IP do ESP
}

void loop() {
  // Código principal vai aqui
}
