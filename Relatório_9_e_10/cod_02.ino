#include <ESP8266WiFi.h>       // Biblioteca de Wi-Fi para ESP8266
#include <ESP8266HTTPClient.h> // Biblioteca para requisições HTTP

// --- Configuração da rede Wi-Fi ---
const char* ssid     = "NomeDaRede";     // SSID (nome da rede)
const char* password = "SenhaSecreta";  // Senha da rede

void setup() {
  Serial.begin(115200);           // Inicia comunicação serial
  WiFi.begin(ssid, password);     // Conecta ao Wi-Fi
  Serial.print("Conectando");

  // Espera até conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Conectado
  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); // Mostra o IP do ESP
  

  // --- Requisição HTTP GET ---
  WiFiClient client; // Cliente Wi-Fi (necessário para begin)
  HTTPClient http;  

  // Novo formato -> precisa passar WiFiClient e URL
  http.begin(client, "http://httpbin.org/get?sensor=ESP8266"); 

  int codigo = http.GET();  // Executa GET

  if (codigo > 0) {  // Se obteve resposta
    Serial.printf("Código HTTP: %d\n", codigo);
    if (codigo == HTTP_CODE_OK) {
      String resposta = http.getString();  // Lê a resposta
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
  // Vazio (coloque aqui se quiser repetir periodicamente)
}
