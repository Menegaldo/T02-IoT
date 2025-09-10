#include <ESP8266WiFi.h>       // Biblioteca Wi-Fi
#include <ESP8266HTTPClient.h> // Biblioteca HTTP

// --- Configuração Wi-Fi ---
const char* ssid     = "NomeDaRede";     
const char* password = "SenhaSecreta";  

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // --- Requisição HTTP GET ---
  WiFiClient client;  // Novo objeto obrigatório
  HTTPClient http;

  // Novo formato: precisa passar o client + URL
  http.begin(client, "http://httpbin.org/get?sensor=ESP8266");

  int codigo = http.GET();  // Executa GET

  if (codigo > 0) {  // Resposta recebida
    Serial.printf("Código HTTP: %d\n", codigo);
    if (codigo == HTTP_CODE_OK) {
      String resposta = http.getString();
      Serial.println("Resposta:");
      Serial.println(resposta);
    }
  } else {
    // Caso não consiga conectar
    Serial.printf("Falha na requisição: %s\n", http.errorToString(codigo).c_str());
  }

  http.end(); // Encerra conexão
}

void loop() {
}
