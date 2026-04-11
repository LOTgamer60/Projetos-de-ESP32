#include <WiFi.h>

const char* ssid = "Everton";
const char* password = "tdhwn06723";

// CONFIGURAÇÃO DE IP FIXO
IPAddress local_IP(192, 168, 23, 200);
IPAddress gateway(192, 168, 23, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

#define LED 2

bool ledState = false;
bool alertMode = false; // novo modo alerta

unsigned long previousMillis = 0;
const long interval = 500; // tempo do pisca (ms)
bool ledBlinkState = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Erro ao configurar IP fixo");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.print("IP fixo: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Servidor iniciado!");
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');

    while(client.available()) client.read();

    // 🚨 SE ESTIVER EM ALERTA
    if (alertMode) {

      // Só aceita PARAR
      if (request.indexOf("GET /led/parar") != -1) {
        alertMode = false;
        ledState = false; // pode mudar se quiser manter ligado depois
        Serial.println(">>> ALERTA PARADO");
      } else {
        Serial.println(">>> Ignorando comando (em modo alerta)");
      }

    } 
    else {
      // 🔴 LIGAR
      if (request.indexOf("GET /led/on") != -1) {
        ledState = true;
        Serial.println("LED LIGADO");
      }

      // ⚫ DESLIGAR
      else if (request.indexOf("GET /led/off") != -1) {
        ledState = false;
        Serial.println("LED DESLIGADO");
      }

      // 🚨 ATIVAR ALERTA
      else if (request.indexOf("GET /led/alerta") != -1) {
        alertMode = true;
        Serial.println("MODO ALERTA ATIVADO");
      }
    }

    // resposta
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.print("OK");

    client.stop();
  }

  // 🚨 PISCANDO
  if (alertMode) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledBlinkState = !ledBlinkState;
      digitalWrite(LED, ledBlinkState);
    }
  } 
  else {
    digitalWrite(LED, ledState);
  }
}