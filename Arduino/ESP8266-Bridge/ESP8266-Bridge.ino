
struct Channel {
  String enabled;
  String state;
  String paused;
  String startTime;
  String duration;
  String repeat;
} channels[4];

void parseToXml(Stream& stream, const Channel* ch, const byte& numCh) {
  stream.println("<?xml version = \"1.0\" ?>");
  stream.println("<channels>");
  for (byte i = 0; i < numCh; i++) {
    stream.println("<channel id=\"" + String(i) + "\">");
    stream.println("<enabled>"  + ch[i].enabled   + "</enabled>");
    stream.println("<state>"    + ch[i].state     + "</state>");
    stream.println("<time>"     + ch[i].startTime + "</time>");
    stream.println("<duration>" + ch[i].duration  + "</duration>");
    stream.println("<repeat>"   + ch[i].repeat    + "</repeat>");
    stream.println("</channel>");
  }
  stream.println("</channels>");
  stream.println('\n');
}

void parseToXml(Stream& stream, const Channel& ch, const byte& idx) {
  stream.println("<?xml version = \"1.0\" ?>");
  stream.println("<channels>");
  stream.println("<channel id=\"" + String(idx) + "\">");
  stream.println("<enabled>"  + ch.enabled   + "</enabled>");
  stream.println("<state>"    + ch.state     + "</state>");
  stream.println("<time>"     + ch.startTime + "</time>");
  stream.println("<duration>" + ch.duration  + "</duration>");
  stream.println("<repeat>"   + ch.repeat    + "</repeat>");
  stream.println("</channel>");
  stream.println("</channels>");
  stream.println('\n');
}


bool readRequest (Stream& stream, String& request) {
  static String buffer = "";
  
  while (stream.available()) {
    char c = stream.read();
    if (c == '\n') {
      request = buffer;
      buffer = "";
      return true;
    }
    else {
      buffer += c;
    }
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);

  Serial.print("[MEGA2560] Setup...");

  channels[0].enabled = "true";
  channels[0].state = "idle";
  channels[0].paused = "false";
  channels[0].startTime = "20:00";
  channels[0].duration = "15";
  channels[0].repeat = "120";
  
  channels[1].enabled = "true";
  channels[1].state = "active";
  channels[1].paused = "false";
  channels[1].startTime = "20:15";
  channels[1].duration = "20";
  channels[1].repeat = "180";
  
  channels[2].enabled = "false";
  channels[2].state = "idle";
  channels[2].paused = "false";
  channels[2].startTime = "20:30";
  channels[2].duration = "25";
  channels[2].repeat = "240";
  
  channels[3].enabled = "true";
  channels[3].state = "paused";
  channels[3].paused = "true";
  channels[3].startTime = "20:45";
  channels[3].duration = "30";
  channels[3].repeat = "300";

  Serial.println("Done");
}

void loop() {
  String request;
  if (readRequest(Serial3, request)) {
    Serial.println("[ESP8266]  " + request);
    int index = request.indexOf("channel.xml");
    if (index >= 0) {
      index = request.indexOf('?', index+11);
      if (index >= 0) {
        // Read channel number
        int channelIdxStart = request.indexOf("channel=", index+1) + 8;
        byte channelIdx = request.substring(channelIdxStart).toInt();

        int startIdx = index;
        int sepIdx, endIdx;

        do {
          sepIdx = request.indexOf('=', startIdx+1);
          endIdx = request.indexOf('&', sepIdx+1);
          if (endIdx == 0) {
            endIdx = request.length();
          }

          String key   = request.substring(startIdx+1, sepIdx);
          String value = request.substring(sepIdx+1, endIdx);
          Serial.print(key);
          Serial.print(" = ");
          Serial.println(value);

          // ToDo: Query content and parse to data object

          startIdx = endIdx;
        } while (endIdx < request.length());

        parseToXml(Serial3, channels[channelIdx], channelIdx);
      }
      else {
        Serial.print("[MEGA2560] Deliver channel.xml...");
        parseToXml(Serial3, channels, 4);
        Serial.println("Done");
      }
    }
  }
}
