// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <FS.h>

// Replace with your network credentials
//const char* ssid     = "gigacube-5193";
//const char* password = "MGJGT2R35RL51HQE";
const char* ssid     = "Jungholz";
const char* password = "45299115630418744911";


// Set web server port number to 80
WiFiServer server(80);

// Web page file
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
  Serial.println("[INFO] Server started\n");
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        request += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            if (request.indexOf("channel.xml") >= 0) {
              // Forward request to arduino mega
              Serial.println(request.substring(0, request.indexOf('\n')));
              Serial.flush();

              // Wait for response from arduino mega
              byte count = 100;
              while (!Serial.available() && count--) {
                delay(10);
              }

              // Read  response from arduino mega
              byte count = 100;
              while (Serial.available()) {
                Serial.read();
                delay(10);
              }

              // Send response to website
              sendFile(client, "/channel.xml");
            }
            else if (request.indexOf("channel=") >= 0) {
              // Forward request to arduino mega
              Serial.println(request.substring(0, request.indexOf('\n')));
              Serial.flush();

              // Wait for response from arduino mega
              while (!Serial.available() && count--) {
                delay(10);
              }

              // Read  response from arduino mega
              byte count = 100;
              while (Serial.available()) {
                Serial.read();
                delay(10);
              }

              // Send response to website
              client.println("HTTP/1.1 200 OK");
              client.println();
            }
            else {
              sendFile(client, "/index.html");
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
  } // end if (client)
}

void sendFile(WiFiClient cl, String filename) {
  String filetype = filename.substring(filename.lastIndexOf('.')+1);

  File file = SPIFFS.open(filename, "r");
  if (file) {
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    cl.println("HTTP/1.1 200 OK");
    cl.println("Content-type:text/" + filetype);
    cl.println("Connection: keep-alive");
    cl.println();
  
    while (file.available()) {
      cl.write(file.read());
    }
    file.close();

    cl.println();
  }
  else {
      cl.println("HTTP/1.1 400 Bad Request");
      cl.println("Content-type:text/plain");
      cl.println("Connection: close");
      cl.println();
      cl.println("Could not find file " + filename);
      
  }
}

// Assemble xml file
void sendXMLFile(WiFiClient cl){
  // Prepare XML file
  cl.print("<?xml version = \"1.0\" ?>");
  cl.print("<inputs>");

  
  cl.print("</inputs>");
}
