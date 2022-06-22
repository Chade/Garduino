#include <Arduino.h>

#include <SPI.h>
#include <SD.h>

#include <EEPROM.h>

#include <Wire.h>

#include <TimeLib.h>
#include <Timezone.h>
#include <DS3232RTC.h>

#include <AM232X.h>

#include <FileConfig.h>
#include <FileConfigHelper.h>

#include <U8g2lib.h>
#include <LCDMenuLib2.h>

#include "Garduino.h"
#include "SunCalender.h"


// *****************************************************************************
// Prototypes
// ******************************************************************************

void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();


// *****************************************************************************
// Objects
// *****************************************************************************

// Display
U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ LCD_CS_PIN, /* reset=*/ U8X8_PIN_NONE);

// Menu
LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, LCDML_DISP_ROWS, LCDML_DISP_COLS, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);


// *****************************************************************************
// Objects
// *****************************************************************************

// Log file
File logFile;

AM232X am2321;

DS3232RTC RTC;

TimeChangeRule *tcr;
TimeChangeRule timeSommer = {"CEST", Last, Sun, Mar, 2, UTC2CEST};    // Daylight saving time = UTC +2 hours
TimeChangeRule timeWinter = {"CET",  Last, Sun, Oct, 3,  UTC2CET};    // Standard time = UTC +1 hours
Timezone timezone(timeSommer, timeWinter);

// Channel storage
Channel channel[NUM_CHANNEL];

// Flow counter
volatile unsigned long flowCounter = 0;

// Decay counter
unsigned long delayCounter = 0;

// Counter for menu clicks
byte click_count = 0;

// Unsaved changes
bool dirty_eeprom = false;
bool dirty_sdcard = false;

// SD initialized
volatile bool sdReady = false;

// Sensor readings
float temperature_intern = 0.0;
float humidity_intern = 0.0;

// *****************************************************************************
// Functions
// *****************************************************************************

void markDirty(bool eeprom, bool sdcard) {
  dirty_eeprom = eeprom;
  dirty_sdcard = sdcard;
}

bool parseConfig(const byte& idx) {
  FileConfig configFile(SD.open(FILE_NAME, FILE_READ));

  if(configFile) {
    String value;
    String header(F("Channel"));
    header.concat(idx);

    value = configFile.getValue(F("Enabled"), header);
    channel[idx].enabled = toBool(value);

    value = configFile.getValue(F("OutputPin"), header);
    channel[idx].output = value.toInt();

    value = configFile.getValue(F("InputPin"), header);
    channel[idx].input = value.toInt();

    value = configFile.getValue(F("SignalPin"), header);
    channel[idx].signal = value.toInt();

    value = configFile.getValue(F("AuxPin"), header);
    channel[idx].aux = toBool(value);

    value = configFile.getValue(F("TimeStart"), header);
    channel[idx].time.start_time = toSeconds(value);

    value = configFile.getValue(F("TimeDuration"), header);
    channel[idx].time.duration = toSeconds(value);

    value = configFile.getValue(F("TimeRepeat"), header);
    channel[idx].time.repeat = toSeconds(value);

    value = configFile.getValue(F("TimeAdjustSetpoint"), header);
    channel[idx].time.setAdjustSetpoint(value);

    value = configFile.getValue(F("TimeAdjustOffset"), header);
    channel[idx].time.adjustOffset = toSignedSeconds(value);

    value = configFile.getValue(F("FlowCount"), header);
    channel[idx].flow.count = value.toInt();

    value = configFile.getValue(F("MovementEnabled"), header);
    channel[idx].movement.enabled = toBool(value);

    value = configFile.getValue(F("MovementInvert"), header);
    channel[idx].movement.inverted = toBool(value);

    value = configFile.getValue(F("MovementPin"), header);
    channel[idx].movement.input = value.toInt();

    value = configFile.getValue(F("MovementDelay"), header);
    channel[idx].movement.delay = value.toInt();

    value = configFile.getValue(F("MoistureEnabled"), header);
    channel[idx].moisture.enabled = toBool(value);

    value = configFile.getValue(F("MoistureInvert"), header);
    channel[idx].moisture.inverted = toBool(value);

    value = configFile.getValue(F("MoisturePin"), header);
    channel[idx].moisture.input = value.toInt();

    value = configFile.getValue(F("MoistureThresholdLow"), header);
    channel[idx].moisture.threshold_low = value.toInt();

    value = configFile.getValue(F("MoistureThresholdHigh"), header);
    channel[idx].moisture.threshold_high = value.toInt();

    value = configFile.getValue(F("RainEnabled"), header);
    channel[idx].rain.enabled = toBool(value);

    value = configFile.getValue(F("RainInvert"), header);
    channel[idx].rain.inverted = toBool(value);

    value = configFile.getValue(F("RainPin"), header);
    channel[idx].rain.input = value.toInt();

    value = configFile.getValue(F("RainThresholdLow"), header);
    channel[idx].rain.threshold_low = value.toInt();

    value = configFile.getValue(F("RainThresholdHigh"), header);
    channel[idx].rain.threshold_high = value.toInt();

    value = configFile.getValue(F("BrightnessEnabled"), header);
    channel[idx].brightness.enabled = toBool(value);

    value = configFile.getValue(F("BrightnessInvert"), header);
    channel[idx].brightness.inverted = toBool(value);

    value = configFile.getValue(F("BrightnessPin"), header);
    channel[idx].brightness.input = value.toInt();

    value = configFile.getValue(F("BrightnessThresholdLow"), header);
    channel[idx].brightness.threshold_low = value.toInt();

    value = configFile.getValue(F("BrightnessThresholdHigh"), header);
    channel[idx].brightness.threshold_high = value.toInt();

    configFile.close();
    
    return true;
  }

  return false;
}

bool parseConfig() {
  for(byte i = 0; i < NUM_CHANNEL; i++)
  {
    Serial.print('.');
    parseConfig(i);
  }

  return true;
}

void channelToXML(Stream& stream) {
  stream.println(F("<?xml version = \"1.0\" ?>"));
  stream.println(F("<channels>"));

  for (byte i = 0; i < NUM_CHANNEL; i++) {
    channel[i].printXML(stream, String(i), false);
  }

  stream.println(F("</channels>"));
  stream.println('\n');
}

void setupIOs() {
  // Set up IOs
  Serial.print(F("[MEGA2560] Setup I/Os."));
  for (byte i = 0; i < NUM_CHANNEL; i++) {
    Serial.print('.');
    if (channel[i].output != 0) {
      pinMode(channel[i].output, OUTPUT);
      digitalWrite(channel[i].output, HIGH);
    }
    if (channel[i].input != 0) {
      pinMode(channel[i].input, INPUT);
    }
    if (channel[i].signal != 0) {
      pinMode(channel[i].signal, OUTPUT);
    }
    if (channel[i].movement.input != 0) {
      pinMode(channel[i].movement.input, INPUT);
    }
    if (channel[i].moisture.input != 0) {
      pinMode(channel[i].moisture.input, INPUT);
    }
    if (channel[i].rain.input != 0) {
      pinMode(channel[i].rain.input, INPUT);
    }
    if (channel[i].brightness.input != 0) {
      pinMode(channel[i].brightness.input, INPUT);
    }
  }
  
  pinMode(AUX_PIN, OUTPUT);
  digitalWrite(AUX_PIN, HIGH);

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(SD_CD_PIN, INPUT);
  pinMode(FLOW_PIN, INPUT);
  
  Serial.println(F("Done"));

  // Attach interrupts
  Serial.print(F("[MEGA2560] Attach interrupts....."));
  attachInterrupt(digitalPinToInterrupt(SD_CD_PIN), sdDetectInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), flowCounterInterrupt, RISING);
  Serial.println(F("Done"));
}

void readEnvSensor() {
  unsigned long current_time = millis();
  static unsigned long last_time = 0;

  if (current_time - last_time > 10000) {
    Serial.print(F("[MEGA2560] Read temperature/humidity..."));
    if (am2321.read() == AM232X_OK) {
      temperature_intern = am2321.getTemperature();
      humidity_intern  = am2321.getHumidity();
      Serial.print(F(" ( "));
      Serial.print(temperature_intern, 1);
      Serial.print(F("°C | "));
      Serial.print(humidity_intern, 1);
      Serial.print(F("% )"));

      if (temperature_intern > 30.0) {
        Serial.println(F(": Fan ON"));
        digitalWrite(FAN_PIN, LOW);
      }
      else if (temperature_intern < 25.0) {
        Serial.println(F(": Fan OFF"));
        digitalWrite(FAN_PIN, HIGH);
      }
    }
    else {
      Serial.println(F("Failed"));
    }
    last_time = current_time;
  }
}

void printRequest(const String& key, const String& value, byte channelIdx, Stream& streamOut = Serial) {
  streamOut.print("[ESP8266 ] [Channel");
  streamOut.print(channelIdx);
  streamOut.print("] ");
  streamOut.print(key);
  streamOut.print(" = ");
  streamOut.println(value);
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

void handleRequest(Stream& streamIn, Stream& streamOut = Serial) {
  String request;
  int index;
  if (readRequest(streamIn, request)) {
    // =========================================================================
    // Handle "channel.xml" requests
    // =========================================================================
    if ((index  = request.indexOf(F("channel.xml"))) >= 0) {
      streamOut.println("[ESP8266 ] Responding to xml request " + request);
      
      // It's a channel.xml request
      // Now we check, if the request contains specific data
      if ((index = request.indexOf('?', index+11)) >= 0) {
        // We now extract the channel index and key value pairs
        // Read channel number
        int channelIdxStart = request.indexOf(F("channel="), index+1) + 8;
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
          key.trim();
          
          String value = request.substring(sepIdx+1, endIdx);
          value.trim();

          if (key == F("enabled")) {
            channel[channelIdx].enable(toBool(value));
            printRequest("Enable", value, channelIdx, streamOut);
          }
          else if (key == F("skip")) {
            channel[channelIdx].doSkip(toBool(value));
            printRequest("Skip", value, channelIdx, streamOut);
          }
          else if (key == F("state")) {
            bool requestActive = toBool(value);
            printRequest("Activate", value, channelIdx, streamOut);
            if(!channel[channelIdx].active && requestActive) {
              printRequest("State", "Activated", channelIdx, streamOut);
              channel[channelIdx].enabled = false;
              channel[channelIdx].active = true;
              channel[channelIdx].skip = false;
            }
            else if(channel[channelIdx].active && !requestActive) {
              printRequest("State", "Deactivated", channelIdx, streamOut);
              channel[channelIdx].active = false;
              if (channel[channelIdx].enabled) {
                channel[channelIdx].skip = true;
              }
              else {
                channel[channelIdx].enabled = true;
              }
            }
          }
          else if (key == F("time")) {
            channel[channelIdx].time.setStartTime(toTime(value));
            printRequest("StartTime", value, channelIdx, streamOut);
          }
          else if (key == F("duration")) {
            channel[channelIdx].time.setDuration(toSeconds(value) * 60);
            printRequest("Duration", value, channelIdx, streamOut);
          }
          else if (key == F("repeat")) {
            channel[channelIdx].time.setRepeat(toSeconds(value) * 60);
            printRequest("Repeat", value, channelIdx, streamOut);
          }
          else if (key == F("setpoint")) {
            channel[channelIdx].time.setAdjustSetpoint(value);
            printRequest("AdjustSetpoint", value, channelIdx, streamOut);
          }
          else if (key == F("offset")) {
            channel[channelIdx].time.setAdjustOffset(toSignedSeconds(value) * 60);
            printRequest("AdjustOffset", value, channelIdx, streamOut);
          }
          startIdx = endIdx;
        } while (endIdx < request.length());

        // Send response xml for modified channel
        channel[channelIdx].printXML(streamIn, String(channelIdx));
        //channel[channelIdx].printXML(streamOut, String(channelIdx));
      }
      else {
        // Send entire xml
        channelToXML(streamIn);
        channelToXML(streamOut);
      }
    }
    // =========================================================================
    // Handle "config" requests
    // =========================================================================
    else if ((index = request.indexOf(F("config.xml"))) >= 0) {
      streamOut.println("[ESP8266 ] Responding to xml request " + request);
      if ((index = request.indexOf('?', index+10)) >= 0) {
        streamOut.println("[ESP8266 ] Responding to xml request " + request);
        
        int actionIdxStart = request.indexOf(F("action="), index+1) + 7;
        int actionIdxEnd   = request.length();

        String value = request.substring(actionIdxStart, actionIdxEnd);
        value.trim();
        
        if (value == F("save")) {
          streamOut.println("[ESP8266 ] Request saving config");

          for (byte i = 0; i < NUM_CHANNEL; i++) {
            EEPROM.put(i * sizeof(Channel), channel[i]);
          }
          
          streamIn.println(F("<?xml version = \"1.0\" ?>"));
          streamIn.print(F("<status>"));
          streamIn.print(F("OK"));
          streamIn.println(F("</status>"));
          streamIn.println('\n');
        }
        else if (value == F("load")) {
          streamOut.println("[ESP8266 ] Request loading config");

          for (byte i = 0; i < NUM_CHANNEL; i++) {
            EEPROM.get(i * sizeof(Channel), channel[i]);
          }
          
          streamIn.println(F("<?xml version = \"1.0\" ?>"));
          streamIn.print(F("<status>"));
          streamIn.print(F("OK"));
          streamIn.println(F("</status>"));
          streamIn.println('\n');
        }
        else {
          streamOut.println("[ESP8266 ] Request unknown action " + value);
        }
      }
    }
    // =========================================================================
    // Relay output of ESP8266
    // =========================================================================
    else {
      // Request probybly contains only ESP8266 outputs
      // Relay output from ESP8266
      streamOut.println("[ESP8266 ] " + request);
    }
  }
}


// *****************************************************************************
// Callbacks
// *****************************************************************************

void flowCounterInterrupt() {
  flowCounter += 1;
}

void sdDetectInterrupt() {
  if (sdReady) {
    sdReady = false;
    SD.end();
  }
}


// *****************************************************************************
// Setup
// *****************************************************************************

void setup() {
  // Initialize Serial
  Serial.begin(BAUD_RATE);
  Serial3.begin(BAUD_RATE);
  while (!Serial || !Serial3) {
    delay(100); // wait for serial ports to connect. Needed for native USB port only
  }

  Serial.println();  

  // Initialize LCD
  Serial.print(F("[MEGA2560] Initializing LCD......"));
  initMenu();
  Serial.println(F("Done"));

  // Initiialize RTC
  Serial.print(F("[MEGA2560] Initializing RTC......"));
  RTC.begin();
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) {
     Serial.println(F("Failed"));
  }
  else {
    Serial.println(F("Done"));
  }

  // Write boot notice to log
  Serial.print(F("[MEGA2560] Write notice to log..."));
  sdReady = SD.begin(SD_CS_PIN);
  if(sdReady) {
    logFile = SD.open(LOGFILE, (O_READ | O_WRITE | O_CREAT | O_APPEND));
    if (logFile) {
      char string_buf[40];
      time_t localtime_unix = timezone.toLocal(now());
      sprintf (string_buf, "[%d.%02d.%02d|%02d:%02d:%02d]Booted", year(localtime_unix), month(localtime_unix), day(localtime_unix), hour(localtime_unix), minute(localtime_unix), second(localtime_unix));
      logFile.println(string_buf);
      logFile.close();
      Serial.println(F("Done"));
    }
    else {
      Serial.println(F("Error opening file"));
    }
#ifndef READ_FROM_EEPROM
    LCDML.OTHER_jumpToFunc(mFunc_readSD);
#endif
  }
  else {
    Serial.println(F("Error starting SD"));
  }

  // Parse config file
#ifdef READ_FROM_EEPROM
  LCDML.OTHER_jumpToFunc(mFunc_readEEPROM);
#endif
}


// *****************************************************************************
// Loop
// *****************************************************************************

void loop() {
  static unsigned long button_time = 0;
  static byte button_prev = LOW;
  
  for (byte i = 0; i < NUM_CHANNEL; i++) {
    if (channel[i].enabled) {
      // ====================================================================================================
      // Adjust start time if adjustment based on sunrise/sunset is enabled
      // ====================================================================================================
      if(channel[i].time.isAdjustEnabled() ) {
        time_t sunrise_localtime_unix, sunset_localtime_unix;
        time_t localtime_unix = timezone.toLocal(now(), &tcr);
        double timezone_offset = tcr->offset / SECS_PER_MIN;

        // Check, if next start is scheduled for today or tomorrow
        if (elapsedSecsToday(localtime_unix) > channel[i].time.getNextStartTime(localtime_unix)) {
          localtime_unix += SECS_PER_DAY;
        }

        // Get sunrise and sunset times
        sunCalender(sunrise_localtime_unix, sunset_localtime_unix, localtime_unix, LATITUDE * M_DEG2RAD, LONGITUDE * M_DEG2RAD, timezone_offset);

        if(channel[i].time.adjustSetpoint == Timer::ESunrise) {
          channel[i].time.start_time = channel[i].time.adjustOffset + elapsedSecsToday(sunrise_localtime_unix);
        }
        else if(channel[i].time.adjustSetpoint == Timer::ESunset) {
          channel[i].time.start_time = channel[i].time.adjustOffset + elapsedSecsToday(sunset_localtime_unix);
        }
        else if(channel[i].time.adjustSetpoint == Timer::ENoon) {
          channel[i].time.start_time = channel[i].time.adjustOffset + (elapsedSecsToday(sunrise_localtime_unix) + elapsedSecsToday(sunset_localtime_unix)) / 2.0;
        }
      }
      
      // ====================================================================================================
      // Check timer and sensors to de-/activate channel
      // ====================================================================================================
      channel[i].active = channel[i].time.active(timezone.toLocal(now()));

      if (channel[i].active != channel[i].was_active) {
        if (channel[i].was_active) {
          // Write statistics to sd card
          if(SD.begin(SD_CS_PIN)) {
            logFile = SD.open(LOGFILE, (O_READ | O_WRITE | O_CREAT | O_APPEND));
            char string_buf[40];
            if (logFile) {
              float liter = (flowCounter - channel[i].flow.start_count)/FLOW_CONV;
              time_t localtime_unix = timezone.toLocal(now());
              sprintf (string_buf, "[%d.%02d.%02d|%02d:%02d:%02d](Ch%02d)", year(localtime_unix), month(localtime_unix), day(localtime_unix), hour(channel[i].time.start_time), minute(channel[i].time.start_time), second(channel[i].time.start_time), i);
              logFile.print(string_buf);
              if (channel[i].skip == true) {
                logFile.println(F("Skipped"));
              }
              else {
                logFile.println(liter);
              }
              logFile.close();
            }
          }
          // Reset skip flag on timer deactivation
          channel[i].skip = false;
        }
        else {
          // Reset flow counter on timer activation
          channel[i].flow.start_count = flowCounter;
        }
        channel[i].was_active = channel[i].active;
      }

      // Check flow counter to deactivate channel
      if (channel[i].active && channel[i].flow.count > 0) {
        channel[i].active &= channel[i].flow.active(flowCounter);
      }

      // Check movement sensor to deactivate channel
      if (channel[i].active && channel[i].movement.enabled) {
        if (channel[i].movement.active()) {
          // Reset decay counter
          delayCounter = 0;
          channel[i].active = false;
        }
        else if (delayCounter < channel[i].movement.delay) {
          delayCounter++;
          channel[i].active = false;
        }
      }

      // Check moisture sensor to deactivate channel
      if (channel[i].active && channel[i].moisture.enabled) {
        channel[i].active &= channel[i].moisture.active();
      }

      // Check rain sensor to deactivate channel
      if (channel[i].active && channel[i].rain.enabled) {
        channel[i].active &= channel[i].rain.active();
      }

      // Check brightness sensor to deactivate channel
      if (channel[i].active && channel[i].brightness.enabled) {
        channel[i].active &= channel[i].brightness.active();
      }
    }

    // ====================================================================================================
    // Update buttons
    // ====================================================================================================
    if (channel[i].input != 0) {
      if (digitalRead(channel[i].input) == HIGH && bitRead(button_prev, i) == LOW) { // Rising edge = button pressed
        bitWrite(button_prev, i, HIGH);
        button_time = millis();
      }
      else if (digitalRead(channel[i].input) == HIGH && bitRead(button_prev, i) == HIGH && button_time != 0) { // High = keeping button pressed
        if ((millis() - button_time) >= CONTROL_BUTTON_LONG_PRESS) {
          // Toggle enabled
          button_time = 0;
          channel[i].enabled = !channel[i].enabled;
        }
      }
      else if (digitalRead(channel[i].input) == LOW && bitRead(button_prev, i) == HIGH) { // Falling edge = button released
        bitWrite(button_prev, i, LOW);
        unsigned long pressed =  millis() - button_time;
        if (pressed >= CONTROL_BUTTON_SHORT_PRESS && pressed < CONTROL_BUTTON_LONG_PRESS) {
          if (channel[i].enabled) {
            // Toggle skip
            channel[i].skip = !channel[i].skip;
          }
          else{
            // Toggle active
            channel[i].active = !channel[i].active;
          }
        }
      }
    }
  }

  // ====================================================================================================
  // Handle request from webserver
  // ====================================================================================================
  handleRequest(Serial3);

  // ====================================================================================================
  // Set outputs and signals
  // ====================================================================================================
  bool auxiliaryPin = false;
  for (byte i = 0; i < NUM_CHANNEL; i++) {
    // Update signals
    if (channel[i].signal != 0) {
      if (channel[i].enabled) {
        if (channel[i].skip) {
          digitalWrite(channel[i].signal, (millis()%3000 < 200) );
        }
        else if (channel[i].time.active(timezone.toLocal(now()))) {
          if (!channel[i].active) {  // Execute prohibited by some sensor
            digitalWrite(channel[i].signal, (millis()%1000 < 50) );
          }
          else {
            digitalWrite(channel[i].signal, (millis()%1000 < 500) );
          }
        }
        else if (channel[i].time.preactive(timezone.toLocal(now()), 60)) {
          digitalWrite(channel[i].signal, (millis()%200 < 50));
        }
        else {
          digitalWrite(channel[i].signal, HIGH);
        }
      }
      else if (channel[i].active) {
        digitalWrite(channel[i].signal, (millis()%1000 < 500) );
      }
      else {
        digitalWrite(channel[i].signal, LOW);
      }
    }

    // Set digital output
    digitalWrite(channel[i].output, !(channel[i].active && !channel[i].skip));

    // Assemble state for auxiliary pin over all channels
    auxiliaryPin |= channel[i].active && !channel[i].skip && channel[i].aux;
  }

  // If at least one channel needs the auxiliary pin, it will be set
  digitalWrite(AUX_PIN, !auxiliaryPin);  // Needs to be inverted due to relay logic

  // ====================================================================================================
  // Read environment sensor
  // ====================================================================================================
  readEnvSensor();

  // ====================================================================================================
  // Update display
  // ====================================================================================================
  updateMenu();
}
