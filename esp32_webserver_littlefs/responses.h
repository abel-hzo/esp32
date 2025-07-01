void encender(int led, WebServer &server) {
  digitalWrite(led, HIGH);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"accion\": \"Encendido\",\"estado\": true}");
}

void apagar(int led, WebServer &server) {
  digitalWrite(led, LOW);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"accion\": \"Apagado\",\"estado\": false}");
}

void estado(int led, WebServer &server) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if(digitalRead(led) == HIGH) {
    server.send(200, "application/json", "{\"accion\": \"Encendido\",\"estado\": true}");
  } else {
    server.send(200, "application/json", "{\"accion\": \"Apagado\",\"estado\": false}");
  }
}

String readFile(const char* path) {

  File file = LittleFS.open(path, "r");

  if(!file) {
    Serial.println("File doesn´t exists.");
    return "";
  }

  String content = "";

  while(file.available()) {
    char c = file.read();
    content += c;
  }

  file.close();

  return content;
} 