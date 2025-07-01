#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>

const char *ssid = ".....";
const char *password = ".....";

AsyncWebServer server(80);
WebSocketsServer webSocket(81);

const int led = GPIO_NUM_5;

/***********************************************************/
/******FUNCION PARA CONECTARSE AL MODEM O ACCESS POINT******/
/***********************************************************/
void initConnection() {

  Serial.println();
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

  //Espera 5 segundos y reinicia el ESP
  /*while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }*/

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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/style.css", "text/css");
  });

  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/main.js", "text/js");
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "";

    if (digitalRead(led) == HIGH) {
      json = "{\"accion\": \"Encendido\",\"estado\": true}";
    } else {
      json = "{\"accion\": \"Apagado\",\"estado\": false}";
    }

    request->send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Servidor iniciado.");
  Serial.println();
}

/***********************************************************/
/*************FUNCION PARA INICIAR EL WEBSOCKET*************/
/***********************************************************/
void initWebSocket() {

  Serial.println("4) Iniciando WebSocket.");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("WebSocket Iniciado.");
  Serial.println();
}

/*Funcion que recibe los datos del websocket de la pagina.*/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);

        // send message to client
        // webSocket.sendTXT(num, "Connected al websocket papu!!");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] mensaje recibido %s\n", num, payload);
      String msg = String((char *)(payload));

      if (msg.equalsIgnoreCase("on")) {
        webSocket.broadcastTXT("{\"accion\": \"Encendido\",\"estado\": true}");
        digitalWrite(led, HIGH);
      } else if (msg.equalsIgnoreCase("off")) {
        webSocket.broadcastTXT("{\"accion\": \"Apagado\",\"estado\": false}");
        digitalWrite(led, LOW);
      }

      // send message to client
      // webSocket.sendTXT(num, "message here Abel HZO!!!");

      // send data to all connected clients, tambien puede ser llamado en el loop.
      // webSocket.broadcastTXT("message here");
      break;
  }
}

/***********************************************************/
/***************FUNCION PARA INICIAR EL MDNS****************/
/***********************************************************/
void initMDNS() {

  Serial.println("5) Iniciando MDNS");

  // Inicializamos el mDNS
  if (!MDNS.begin("myesp")) {  // Set the hostname to "myesp.local"
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("MDNS Iniciado.");
  Serial.println();
}

void setup() {

  Serial.begin(9600);
  pinMode(led, OUTPUT);

  initConnection();
  initLittleFS();
  initServer();
  initWebSocket();
  initMDNS();

}

void loop() {
  webSocket.loop();
}
