#include <LittleFS.h>

template<class T> inline Print& operator<<(Print& obj, T arg) {
  obj.print(arg);
  return obj;
}

void createDirectory(const char* path) {

  Serial << "Create directory: " << path << "\n";

  if(LittleFS.mkdir(path)) {
    Serial << "Directory created: " << path << "\n"; 
  } else {
    Serial << "Directory failed." << "\n";
  }

}

void removeDirectory(const char* path) {

  Serial << "Removing directory: " << path << "\n";

  if(LittleFS.rmdir(path)) {
    Serial << "Directory removed: " << path << "\n";
  } else {
    Serial << "Removed Directory Failed." << "\n";
  }

}

void listDirectories(const char* dirname, uint8_t levels) {

  Serial << "List Directories: " << dirname << "\n";

  File root = LittleFS.open(dirname);

  if(!root) {
    Serial << "Failed Opened Directory." << "\n";
    return;
  }

  if(!root.isDirectory()) {
    Serial << "Not a Directory." << "\n";
    return;
  }

  File file = root.openNextFile();

  while(file) {

    if(file.isDirectory()) {
      Serial << "Directory: " << file.name() << "\n";

      if(levels) {
        listDirectories(file.path(), levels - 1);
      }

    } else {
      Serial << "FILE: " << file.name() << "\n";
      Serial << "SIZE: " << file.size() << "\n";
    }

    file = root.openNextFile();
  }

}

//Replace all content in the file.
void writeFile(const char* path, const char* text) {

  Serial << "Write the file: " << path << "\n";

  File file = LittleFS.open(path, "w");

  if (!file) {
    Serial << "File doesn´t open." << "\n";
    return;
  }

  if (file.println(text)) {
    Serial.println("Writing on the file.");
  } else {
    Serial.println("Error on writing.");
  }

  file.close();
}

void appendText(const char* path, const char* text) {

    Serial << "Add text in the file: " << path << "\n";

    File file = LittleFS.open(path, "a");

    if (!file) {
      Serial << "File doesn´t open." << "\n";
      return;
    }

    if (file.println(text)) {
      Serial.println("Message appended");
    } else {
      Serial.println("Append failed");
    }

    file.close();
}

//Read the file.
void readFile(const char* path) {

  Serial << "Read the file: " << path << "\n";

  File file = LittleFS.open(path, "r");

  if (!file) {
    Serial << "File doesn´t open."
           << "\n";
    return;
  }

  Serial.print("Content: ");

  while (file.available()) {
    Serial.write(file.read());
  }

  file.close();
}

void deleteFile(const char* path) {

  Serial << "Deleting file: " << path << "\n";

  if(LittleFS.remove(path)) {
    Serial << "File deleted: " << path << "\n";
  } else {
    Serial << "Error deleted: " << path << "\n";
  }
}

void setup() {

  Serial.begin(9600);
  Serial.println("\n\nIniciando de LittleFS");

  if (!LittleFS.begin(true)) {   //Pasando true en el parametro, permiso para formatear.
    Serial.println("Error montando LittleFS");
    return;
  }

  /*if (LittleFS.format()) {   //Formatar el sistema de archivos
    Serial.println("LittleFS formateado exitosamente");
  } else {
    Serial.println("Error al formatear LittleFS");
  }*/

  // LittleFS.end();
 
}

void loop() {

  if(Serial.available()) {

    String command = Serial.readString();
    command.trim();
    Serial.println("Comando: " + command);

    if(command.startsWith("MK")) {   // MK /folder
      //Función para crear un directorio.
      String folder = command.substring(command.indexOf(" "), 30);
      folder.trim();
      Serial.println(folder);

      createDirectory(folder.c_str());
    } else if(command.startsWith("WR")) {  // WR /folder/archivo.txt El contenido que tendra el archivo es este.
      //Funcion para crear un nuevo archivo al sistema de archivos.
      //WR archivo.txt Hola como estas
      //Devuelve 2, es decir, el primer espacio que encuentra, despues de WR.
      Serial.println(command.indexOf(" "));
      //Devuelve 14, es decir, el segundo espacio que encuentra, despues de archivo.txt.
      Serial.println(command.indexOf(" ", 3));
      //Devuelve 19, es decir, el tercer espacio que encuentra, despues de Hola.
      Serial.println(command.indexOf(" ", 15)); 
      //indexOf: Devuelve el indice del primer caracter encontrado en la cadena 
      //apartir del indice indicado en adelante.

      //Que busque de el incide el espacio en blanco desde el inicio al que se 
      String file = command.substring(command.indexOf(" "), command.indexOf(" ", 3));
      file.trim();

      String text = command.substring(command.indexOf(" ", 3), command.indexOf(" ", 100));
      text.trim();

      Serial.println(file);
      Serial.println(text);

      writeFile(file.c_str(), text.c_str());
    } else if(command.startsWith("RF")) {  //RF /archivo.txt

      String file = command.substring(command.indexOf(" "), 30);
      file.trim();
      Serial.println(file);

      readFile(file.c_str());

    } else if(command.startsWith("AP")) {  //AP /archivo.txt Appened information in the existent file. 
      //Funcion para concatenar texto a un archivo existente
      String file = command.substring(command.indexOf(" "), command.indexOf(" ", 3));
      file.trim();

      String text = command.substring(command.indexOf(" ", 3), command.indexOf(" ", 100));
      text.trim();

      Serial.println(file);
      Serial.println(text);

      appendText(file.c_str(), text.c_str());
    } else if(command.startsWith("LS")) {  //LS
      listDirectories("/", 2);
    } else if(command.startsWith("RM")) {  // RM /archivo.txt
      //Función para borrar un archivo.
      String file = command.substring(command.indexOf(" "), 30);
      file.trim();
      Serial.println(file);

      deleteFile(file.c_str());

    } else if(command.startsWith("RD")) {  // RD /folder
      //Función para borrar un directorio.
      String folder = command.substring(command.indexOf(" "), 30);
      folder.trim();
      Serial.println(folder);

      removeDirectory(folder.c_str());

    } else {
      Serial.println("No comando.");
    }

  }

}
