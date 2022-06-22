#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define TIMEOUT 1500

// Replace with your network credentials
const char* ssid     = "Jungholz";
const char* password = "20230562558816153495";

// Set web server port number to 80
ESP8266WebServer server(80);


// Route for root / web page
void handleRoot(){
  Serial.print("Serving index.html...");
  if (SPIFFS.exists("/index.html")) {
    File file = SPIFFS.open("/index.html", "r");
    if (file.available()) {
      size_t size = server.streamFile(file, "text/html");
      Serial.println(size);
    }
    else {
      server.send(404, "text/plain", "Site not available");
      Serial.println("Could not read index.html");
    }
    file.close(); 
  }
  else {
    server.send(404, "text/plain", "Site could not be found");
    Serial.println("Could not find index.html");
  }
}

// Route for xml requests
void handleXML(){
  String head = server.uri();
  int params = server.args();

  if (params > 0) {
     head += '?';
  }
  
  for (int i = 0; i < params; i++){
    head += server.argName(i);
    head += '=';
    head += server.arg(i);

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
    server.send(200, "text/xml", response); 
  }
  else {
    Serial.println("Timout while assembling response");
    server.send(504, "text/plain", "Timout while assembling response"); 
  }
}

void setup() {
  // Initialize serial
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin()){
    Serial.println("An Error occurred while mounting SPIFFS");
  }

  // Initialize wifi and connect to network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address
  Serial.println("");
  Serial.print("WiFi connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/channel.xml", HTTP_GET, handleXML);
  server.on("/config.xml",  HTTP_GET, handleXML);

  // Start web server
  server.begin();
  
  Serial.println("Webserver started\n");
  Serial.flush();
}

void loop(){
  server.handleClient();
  MDNS.update();
}
