String readFile(const char* path) {

  File file = LittleFS.open(path, "r");

  if (!file) {
    Serial.println("File doesn´t exists.");
    return "";
  }

  String content = "";

  while (file.available()) {
    char c = file.read();
    content += c;
  }

  file.close();

  return content;
}