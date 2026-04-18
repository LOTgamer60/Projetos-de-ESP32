#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid = "SEU_WIFI";
const char* password = "SENHA";

IPAddress local_IP(192, 168, 23, 200);
IPAddress gateway(192, 168, 23, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

Servo olho1;
Servo olho2;

#define LED 2

bool acordado = false;

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  olho1.attach(13);
  olho2.attach(12);

  // começa fechado
  olho1.write(0);
  olho2.write(0);

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

// 👀 abrir olhos (com leve diferença)
void abrirOlhos() {
  digitalWrite(LED, HIGH);

  for (int pos = 0; pos <= 90; pos++) {
    olho1.write(pos);
    olho2.write(pos - 5); // leve atraso no segundo olho
    delay(20);
  }
}

// 😴 fechar olhos
void fecharOlhos() {
  for (int pos = 90; pos >= 0; pos--) {
    olho1.write(pos);
    olho2.write(pos - 5);
    delay(20);
  }

  digitalWrite(LED, LOW);
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');

    while(client.available()) client.read();

    if (request.indexOf("GET /acordar") != -1) {
      if (!acordado) {
        Serial.println("Acordando...");
        abrirOlhos();
        acordado = true;
      }
    }

    else if (request.indexOf("GET /dormir") != -1) {
      if (acordado) {
        Serial.println("Dormindo...");
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
