#include <WiFi.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "responses.h"

const char* ssid = ".....";
const char* password = ".....";

WebServer server(80);

const int led = GPIO_NUM_5;

/***********************************************************/
/******FUNCION PARA CONECTARSE AL MODEM O ACCESS POINT******/
/***********************************************************/
void initConnection() {
  
  Serial.println();
  Serial.println("1) Iniciando conexión");

  //Establecer valores de un IP estatica
  IPAddress ip(192, 168, 0, 100);
  IPAddress gateway(192, 168, 0, 1); //IP del router
  IPAddress subnet(255, 225, 225, 0);

  //Configuramos el ESP32 como estación
  WiFi.mode(WIFI_STA);
  //Establecemos al IP estatica
  WiFi.config(ip, gateway, subnet);
  //Iniciamos la conexión
  WiFi.begin(ssid, password);

  //Esperamos a que conecte mediante la funcion WiFi.status()
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Imprimimos los datos de la conexión
  Serial.println("Conectado a: ");
  Serial.println(ssid);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

}

/***********************************************************/
/*******FUNCION PARA INICIAR EL SISTEMA DE ARCHIVOS*********/
/***********************************************************/
void initLittleFS() {
  Serial.println("2) Iniciando sistema de archivos.");
  /*Codigo para iniciar el ESP32, con true en su parametro quiere decir que, 
    aceptara el formateo.*/
  #if defined(ESP32)
    if (!LittleFS.begin(true)) {
      Serial.println("Error Formateando.");
    } else {
      Serial.println("Sistema de archivos iniciado (ESP32).");
    }
  #endif

  /*Codigo para formatear el ESP8266, no necesita inicializarse con begin.*/
  /*Este codigo es meramente ilustrativo, no necesitara usarse ya que este codigo
      es exclusivo del ESP32.*/
  #if defined(ESP8266)
    if (!LittleFS.begin()) {
      Serial.println("Error Montando LittleFS.");
    }

    if (!LittleFS.format()) {
      Serial.println("Error Formateando.");
    } else {
      Serial.println("El dispositivo se formateo (ESP8266).");
    }
  #endif

  Serial.println();
  // LittleFS.end();
}

/***********************************************************/
/*************FUNCION PARA INICIAR EL SERVIDOR**************/
/***********************************************************/
void initServer() {

  Serial.println("3) Iniciando Servidor");

  server.on("/", []() {
    server.send(200, "text/html", readFile("/index.html"));
  });

  server.on("/style.css", []() {
    server.send(200, "text/css", readFile("/style.css"));
  });

  server.on("/main.js", []() {
    server.send(200, "text/js", readFile("/main.js"));
  });

  //server.on("/on", encender);
  server.on("/on", []() { encender(led, server); });
  //server.on("/off", apagar);
  server.on("/off", []() { apagar(led, server); });
  server.on("/status", []() { estado(led, server); });

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
  Serial.println("4) Iniciando MDNS");
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

  Serial.begin(9600);

  pinMode(led, OUTPUT);

  initConnection();
  initLittleFS();
  initServer();
  initMDNS();

}

void loop() {
  //Función que escucha las peticiones de los clientes.
  server.handleClient();
}
