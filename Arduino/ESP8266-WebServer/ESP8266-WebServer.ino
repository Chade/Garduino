// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <FS.h>

// Replace with your network credentials
const char* ssid     = "gigacube-5193";
const char* password = "MGJGT2R35RL51HQE";

// Set web server port number to 80
WiFiServer server(80);

// Web page file
File webFile;

// Variable to store the HTTP request
String header;

void setup() {
  // Initialize serial
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  // Print local IP address
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Start web server
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            if (header.indexOf("channel") >= 0) {
              Serial.print("Request received: ");
              if (header.indexOf("channel.xml") >= 0) {
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/xml");
                client.println("Connection: keep-alive");
                client.println();
                
                Serial.println("channel.xml");
                webFile = SPIFFS.open("/channel.xml", "r");
                if (webFile) {
                  while (webFile.available()) {
                    client.write(webFile.read());
                  }
  
                  webFile.close();
                }
                else {
                  Serial.println("No such file");
                }
              }
              else if (header.indexOf("channel=") >= 0) {
                Serial.println("Update channel informations");
              }
            }
            else {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: keep-alive");
              client.println();
              
              webFile = SPIFFS.open("/index.html", "r");
              if (webFile) {
                Serial.println("Deliver index.html");
                while (webFile.available()) {
                  client.write(webFile.read());
                }
  
                webFile.close();
              }
              else {
                  Serial.println("No such file");
              }
            }

            // The HTTP response ends with another blank line
            client.println();

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
    header = "";
    
    // Close the connection
    client.stop();
    
    Serial.println("Client disconnected.");
    Serial.println("");
  } // end if (client)
}

// Assemble xml file
void sendXMLFile(WiFiClient cl){
  // Prepare XML file
  cl.print("<?xml version = \"1.0\" ?>");
  cl.print("<inputs>");

  
  cl.print("</inputs>");
}
