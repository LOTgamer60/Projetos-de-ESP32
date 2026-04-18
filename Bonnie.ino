#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid = "SEU_WIFI";
const char* password = "SENHA";

IPAddress local_IP(192, 168, 23, 200);
IPAddress gateway(192, 168, 23, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

Servo olhoE; // esquerdo
Servo olhoD; // direito

#define LED_E 2
#define LED_D 4

bool acordado = false;

void setup() {
  Serial.begin(115200);

  pinMode(LED_E, OUTPUT);
  pinMode(LED_D, OUTPUT);

  digitalWrite(LED_E, LOW);
  digitalWrite(LED_D, LOW);

  olhoE.attach(13);
  olhoD.attach(12);

  // começa fechado
  olhoE.write(0);
  olhoD.write(0);

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

// 👀 ACORDAR (com leve diferença entre olhos)
void acordarAnimatronico() {

  // LEDs acendem primeiro
  digitalWrite(LED_E, HIGH);
  delay(80);
  digitalWrite(LED_D, HIGH);

  // abertura suave com diferença
  for (int pos = 0; pos <= 90; pos++) {
    olhoE.write(pos);
    olhoD.write(pos - 5); // leve atraso
    delay(20);
  }
}

// 😴 DORMIR
void dormirAnimatronico() {

  for (int pos = 90; pos >= 0; pos--) {
    olhoE.write(pos);
    olhoD.write(pos - 5);
    delay(20);
  }

  digitalWrite(LED_E, LOW);
  delay(50);
  digitalWrite(LED_D, LOW);
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');

    while(client.available()) client.read();

    if (request.indexOf("GET /acordar") != -1) {
      if (!acordado) {
        Serial.println(">>> Acordando...");
        acordarAnimatronico();
        acordado = true;
      }
    }

    else if (request.indexOf("GET /dormir") != -1) {
      if (acordado) {
        Serial.println(">>> Dormindo...");
        dormirAnimatronico();
        acordado = false;
      }
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
}
