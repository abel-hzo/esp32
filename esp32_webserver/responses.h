void pagina(int led, WebServer& server) { 

  char buffer[2000];

  String ip = WiFi.localIP().toString();

  String estado = "<div id='button'>🔴 Apagado</div>";

  if(digitalRead(led) == HIGH)
    estado = "<div id='button' style='background-color: green'>🟢 Encendido</div>";

  snprintf(buffer, 2000, 
    "<!DOCTYPE html>\
    <html lang='en'>\
    <head>\
        <meta charset='UTF-8'>\
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
        <title>Led</title>\
        <style>\
            * {\
                margin: 0;\
                padding: 0;\
                box-sizing: border-box;\
            }\
\
            body {\
                background-color: rgb(16, 16, 16);\
            }\
\
            h1 {\
                margin-top: 20px;\
                text-align: center;\
                font-family: 'Courier New', Courier, monospace;\
                color: white;\
            }\
\
            #button {\
                background-color: red;\
                padding: 1rem;\
                font-family: 'Courier New', Courier, monospace;\
                font-size: 20px;\
                font-weight: bold;\
                margin: 20px auto 0 auto;\
                color: white;\
                width: 200px;\
                text-align: center;\
                border-radius: 20px;\
                box-shadow: 5px 5px 10px rgb(44, 44, 44);\
                cursor: pointer;\
            }\
\
        </style>\
    </head>\
    <body>\
        <h1>Encendido del Led</h1>\
\
        %s\
    </body>\
\
    <script>\
        let button = document.getElementById('button');\
\
        button.addEventListener('click', (e) => {\
            if(button.textContent == '🔴 Apagado') {\
                fetching('http://%s/on');\
            } else {\
                fetching('http://%s/off');\
            }\
        });\
\
        function fetching(url) {\
            fetch(url, {\
                method: 'GET'\
            })\
            .then(response => response.json())\
            .then(data => {\
                button.style.backgroundColor = data.color;\
                button.textContent = data.msg;\
            });\
        }\
\
    </script>\
    </html>", estado.c_str(), ip.c_str(), ip.c_str());
  server.send(200, "text/html", buffer);
}

void encender(int led, WebServer& server) {
  digitalWrite(led, HIGH);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"msg\": \"🟢 Encendido\", \"color\": \"green\"}");
}

void apagar(int led, WebServer& server) {
  digitalWrite(led, LOW);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"msg\": \"🔴 Apagado\", \"color\": \"red\"}");
}