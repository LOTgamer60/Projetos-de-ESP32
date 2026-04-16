#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid = "SEU_WIFI";
const char* password = "SENHA";

IPAddress local_IP(192, 168, 23, 200);
IPAddress gateway(192, 168, 23, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

Servo servo;

#define LED 2

bool acordado = false;

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  servo.attach(13);
  servo.write(0); // olhos fechados

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

// abrir olhos + acender LED
void acordarAnimatronico() {
  digitalWrite(LED, HIGH); // LED liga primeiro

  for (int pos = 0; pos <= 90; pos++) {
    servo.write(pos);
    delay(20);
  }
}

//  fechar olhos + apagar LED
void dormirAnimatronico() {
  for (int pos = 90; pos >= 0; pos--) {
    servo.write(pos);
    delay(20);
  }

  digitalWrite(LED, LOW); // LED apaga no final
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');

    while(client.available()) client.read();

    //  ACORDAR
    if (request.indexOf("GET /acordar") != -1) {
      if (!acordado) {
        Serial.println(">>> Acordando...");
        acordarAnimatronico();
        acordado = true;
      }
    }

    //  DORMIR
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
