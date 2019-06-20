// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <FS.h>

// Replace with your network credentials
const char* ssid     = "Jungholz";
const char* password = "45299115630418744911";

// Set web server port number to 80
WiFiServer server(80);

// File handle
File webFile;

// Variable to store the HTTP request
String request, response;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  Serial.println();

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("[ERROR] An Error has occurred while mounting SPIFFS");
    return;
  }
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("[INFO] Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  // Print local IP address
  Serial.println("[INFO] WiFi connected.");
  Serial.print("[INFO] IP address: ");
  Serial.println(WiFi.localIP());
  
  // Start web server
  server.begin();
  Serial.println("[INFO] Server started");
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {
    Serial.println("[INFO] New client connected");
    
    String currentLine = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n') {
          // If the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            if (request.indexOf("channel") >= 0) {
              Serial.print("[INFO] Request received: ");
              if (request.indexOf("channel.xml") >= 0) {
                Serial.println("channel.xml");
                sendFile(client, "/channel.xml");
              }
              else if (request.indexOf("channel=") >= 0) {
                // Forward request
                Serial.println(request);
                Serial.flush();

                // Wait for response
                byte counter = 100;
                while (!Serial.available() && counter--) {
                  delay(100);
                }
              }
            }
            else {
              Serial.println("[INFO] Request received: index.html");
              //sendFile(client, "/index.html");
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: keep-alive");
              client.println();
              
              webFile = SPIFFS.open("/index.html", "r");
              if (webFile) {
                while (webFile.available()) {
                  client.write(webFile.read());
                }
                webFile.close();
              }
              else {
                  Serial.println("[ERROR] No such file index.html");
              }
            }

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      } // end if (client.available())
    } // end while (client.connected())
    
    // Clear the header variable
    request = "";
    
    // Close the connection
    client.stop();
    
    Serial.println("[INFO] Client disconnected.");
    Serial.println("");

    /*while (Serial.available()) {
      char c = client.read();
      response += c;
      if (c == '\n') {
        
      }
    }*/
  } // end if (client)
}

void sendFile(WiFiClient cl, String filename) {
  Serial.print("Send file ");
  Serial.println(filename);
  
  String filetype = filename.substring(filename.lastIndexOf('.')+1);
  Serial.print("File extension is ");
  Serial.println(filetype);
  

  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  cl.println("HTTP/1.1 200 OK");
  cl.println("Content-type:text/" + filetype);
  cl.println("Connection: keep-alive");
  cl.println();
  
  webFile = SPIFFS.open(filename, "r");
  if (webFile) {
    while (webFile.available()) {
      cl.write(webFile.read());
    }
    webFile.close();
  }
  else {
      Serial.print("[ERROR] No such file ");
      Serial.println(filename);
  }

  Serial.println("Finished");
}

// Assemble xml file
void sendXMLFile(WiFiClient cl){
  // Prepare XML file
  cl.print("<?xml version = \"1.0\" ?>");
  cl.print("<inputs>");

  
  cl.print("</inputs>");
}
