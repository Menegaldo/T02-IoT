#include <WiFi.h>
#include <HTTPClient.h>

// ---- Wi-Fi (troque pelos seus) ----
const char* ssid     = "WLL-Inatel";
const char* password = "inatelsemfio";

void setup() {
  Serial.begin(115200);
  delay(200);

  // 1) Conectar ao Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConectado! IP: %s\n", WiFi.localIP().toString().c_str());

  // 2) Fazer uma requisição HTTP simples (GET) ao httpbin
  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // segue 301/302 se houver
  http.setTimeout(7000);                                  // (ms) tempo limite de conexão

  const char* url = "http://httpbin.org/get?sensor=ESP32&demo=1";
  Serial.printf("Requisitando: %s\n", url);

  http.begin(url);                 // inicia conexão com o servidor
  int code = http.GET();           // envia GET
  Serial.printf("Código HTTP: %d\n", code);

  if (code > 0) {
    String body = http.getString(); // corpo da resposta
    Serial.println("Resposta:");
    Serial.println(body);           // JSON com detalhes da requisição
  } else {
    // Em caso de falha, mostra o erro textual da biblioteca
    Serial.printf("Falha na requisição: %s\n", http.errorToString(code).c_str());
  }

  http.end(); // encerra a conexão
}

void loop() {
  // vazio — a demonstração roda uma vez no setup()
}
