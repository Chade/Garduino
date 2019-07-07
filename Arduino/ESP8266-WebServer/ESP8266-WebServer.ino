// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

#define TIMEOUT 1500

// Replace with your network credentials
const char* ssid     = "Jungholz-AP";
const char* password = "45299115630418744911";


// Set web server port number to 80
AsyncWebServer server(80);

void setup() {
  // Initialize serial
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin()){
    Serial.println("An Error occurred while mounting SPIFFS");
    return;
  }

  // Initialize wifi and connect to network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Print local IP address
  Serial.print("WiFi connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route for xml requests
  server.on("/channel.xml", HTTP_GET, [](AsyncWebServerRequest *request){
    String head = request->url();
    int params = request->params();

    if (params > 0) {
       head += '?';
    }
    
    for (int i = 0; i < params; i++){
      AsyncWebParameter* p = request->getParam(i);
      head += p->name();
      head += '=';
      head += p->value();

      if ((params - i) > 1) {
        head += '&';
      }
    }

    Serial.println(head);
    Serial.flush();

    unsigned long startTime = millis();
    unsigned long duration = 0;
    String response = "";
    String currentLine = "";
    do {
      if (Serial.available()) {
        char c = Serial.read();

        // Check for newline
        if (c == '\n') {
          // If current line is empty, response is ready
          if (currentLine.length() == 0 && response.length() > 0) {
              break;
          }
          else if (currentLine.length() > 0) {
            response += currentLine;
            response += '\n';
            currentLine = "";
          }
        }
        else if (c != '\r') {
          currentLine += c;
        }
      }
      duration = millis() - startTime;
    } while (duration < TIMEOUT);

    if (duration < TIMEOUT) {
      request->send(200, "text/xml", response); 
    }
    else {
      Serial.println("Timout while assembling response");
      request->send(504, "text/plain", "Timout while assembling response"); 
    }
    
  });

  // Start web server
  server.begin();
  
  Serial.println("Webserver started\n");
  Serial.flush();
}

void loop(){

}
