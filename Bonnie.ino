#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid = "SEU WIFI";
const char* password = "SUA SENHA";

IPAddress local_IP(192, 168, 23, 200);
IPAddress gateway(192, 168, 23, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

Servo servo;

// Estados
bool acordado = false;

void setup() {
  Serial.begin(115200);

  servo.attach(13);
  servo.write(0); // começa com olhos fechados

  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.println(WiFi.localIP());

  server.begin();
}

// Função para abrir olhos suavemente
void abrirOlhos() {
  for (int pos = 0; pos <= 90; pos++) {
    servo.write(pos);
    delay(20); // controla a velocidade
  }
}

// Função para fechar olhos
void fecharOlhos() {
  for (int pos = 90; pos >= 0; pos--) {
    servo.write(pos);
    delay(20);
  }
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');

    while(client.available()) client.read();

    // 👀 ACORDAR
    if (request.indexOf("GET /acordar") != -1) {
      if (!acordado) {
        Serial.println(">>> Acordando...");
        abrirOlhos();
        acordado = true;
      }
    }

    // 😴 DORMIR
    else if (request.indexOf("GET /dormir") != -1) {
      if (acordado) {
        Serial.println(">>> Dormindo...");
        fecharOlhos();
        acordado = false;
      }
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
}
