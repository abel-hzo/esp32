#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>
#include "responses.h"

const char* ssid = ".....";
const char* password = ".....";

WebServer server(80);
WebSocketsServer webSocket(81);

const int led = GPIO_NUM_5;

/***********************************************************/
/******FUNCION PARA CONECTARSE AL MODEM O ACCESS POINT******/
/***********************************************************/
void initConnection() {

  Serial.println("1) Iniciando conexión");

  //Establecer valores de un IP estatica
  IPAddress ip(192, 168, 0, 100);
  IPAddress gateway(192, 168, 0, 1);  //IP del router
  IPAddress subnet(255, 225, 225, 0);

  //Configuramos el ESP32 como estación
  WiFi.mode(WIFI_STA);
  //Establecemos al IP estatica
  WiFi.config(ip, gateway, subnet);
  //Iniciamos la conexión
  WiFi.begin(ssid, password);

  //Esperamos a que conecte mediante la funcion WiFi.status()
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Imprimimos los datos de la conexión
  Serial.println();
  Serial.println("Conectado a: ");
  Serial.println(ssid);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/***********************************************************/
/*******FUNCION PARA INICIAR EL SISTEMA DE ARCHIVOS*********/
/***********************************************************/
void initLittleFS() {

/*Codigo para iniciar el ESP32, con true en su parametro quiere decir que, 
    aceptara el formateo.*/
#if defined(ESP32)
  if (!LittleFS.begin(true)) {
    Serial.println("Error Formateando.");
  } else {
    Serial.println("El dispositivo se formateo (ESP32).");
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

  // LittleFS.end();
}


/***********************************************************/
/*************FUNCION PARA INICIAR EL SERVIDOR**************/
/***********************************************************/
void initServer() {

  /*La funcion readFile es la funcion que leera los archivos
   y enviara el codigo por 'server.send' */

  server.on("/", []() {
    server.send(200, "text/html", readFile("/index.html"));
  });

  server.on("/style.css", []() {
    server.send(200, "text/css", readFile("/style.css"));
  });

  server.on("/main.js", []() {
    server.send(200, "text/js", readFile("/main.js"));
  });

  server.on("/status", []() {
    String json = "";

    if (digitalRead(led) == HIGH) {
      json = "{\"accion\": \"Encendido\",\"estado\": true}";
    } else {
      json = "{\"accion\": \"Apagado\",\"estado\": false}";
    }

    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("HTTP server started");
}

/***********************************************************/
/***************FUNCION PARA INICIAR EL MDNS****************/
/***********************************************************/
void initMDNS() {

  // Inicializamos el mDNS
  if (!MDNS.begin("myesp")) {  // Set the hostname to "myesp.local"
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("MDNS Iniciado.");
}

/***********************************************************/
/*************FUNCION PARA INICIAR EL WEBSOCKET*************/
/***********************************************************/
void initWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket Iniciado.");
}

/*Funcion que recibe los datos del websocket de la pagina.*/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        // webSocket.sendTXT(num, "Connected al websocket papu!!");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] mensaje recibido %s\n", num, payload);
      String msg = String((char*)(payload));

      if (msg.equalsIgnoreCase("on")) {
        digitalWrite(led, HIGH);
        webSocket.broadcastTXT("{\"accion\": \"Encendido\",\"estado\": true}");
      } else if (msg.equalsIgnoreCase("off")) {
        digitalWrite(led, LOW);
        webSocket.broadcastTXT("{\"accion\": \"Apagado\",\"estado\": false}");
      }

      // send message to client
      // webSocket.sendTXT(num, "message here Abel HZO!!!");

      // send data to all connected clients, tambien puede ser llamado en el loop.
      // webSocket.broadcastTXT("message here");
      break;
  }
}

void setup() {

  Serial.begin(9600);

  pinMode(led, OUTPUT);

  initConnection();
  initLittleFS();
  initServer();
  initMDNS();
  initWebSocket();
}

void loop() {
  server.handleClient();
  webSocket.loop();
}
