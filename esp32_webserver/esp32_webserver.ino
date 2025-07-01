#if defined(ESP32)

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "responses.h"

WebServer server(80);

#endif

const char* ssid = ".....";
const char* password = ".....";

const int led = GPIO_NUM_5;

/***********************************************************/
/******FUNCION PARA CONECTARSE AL MODEM O ACCESS POINT******/
/***********************************************************/
void initConnection() {

  Serial.println();
  Serial.println("1) Iniciando conexión");

  // No funciona al conectar con Hotspot Mobile, solo con router.
  IPAddress ip(192, 168, 0, 10);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conectado a la red Wi-Fi: ");
  Serial.println(WiFi.SSID()); 
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(); 

}

/***********************************************************/
/*************FUNCION PARA INICIAR EL SERVIDOR**************/
/***********************************************************/
void initServer() {

  Serial.println("2) Iniciando Servidor");

  //server.on("/", mensaje);
  server.on("/", []() { pagina(led, server); });
  //server.on("/on", encender);
  server.on("/on", []() { encender(led ,server); });
  server.on("/off", []() { apagar(led, server); });

  //En caso de colocar una url no registrada.
  server.onNotFound([]() { 
    server.send(404, "text/html", "<h1>Lo siento, lo que tu buscas no existe!!!</h1>");
  });

  server.begin();
  Serial.println("Servidor iniciado.");
  Serial.println();
}

/***********************************************************/
/***************FUNCION PARA INICIAR EL MDNS****************/
/***********************************************************/
void initMDNS() {
  Serial.println("3) Iniciando MDNS");
  // No funciona al conectar con Hotspot Mobile
  if(!MDNS.begin("myesp")) {
    Serial.println("Error al configurar MDNS");
    while (1) {
        delay(1000);
    }
  }  

  MDNS.addService("http", "tcp", 80);

  Serial.println("MDNS Iniciado");
  Serial.println();
}

void setup() {
  pinMode(led, OUTPUT);

  Serial.begin(9600);

  initConnection();
  initServer();
  initMDNS();
}

void loop() {
  server.handleClient();
}
