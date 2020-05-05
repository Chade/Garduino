#include <Arduino.h>

#include <SPI.h>
#include <SD.h>

#include <EEPROM.h>

#include <Wire.h>

#include <TimeLib.h>
#include <DS3232RTC.h>

#include <AM232X.h>

#include <FileConfig.h>
#include <FileConfigHelper.h>

#include <U8g2lib.h>
#include <LCDMenuLib2.h>

#include "Garduino.h"


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

// Channel storage
Channel channel[NUM_CHANNEL];

// Flow counter
volatile unsigned long flowCounter = 0;

// Decay counter
unsigned long delayCounter = 0;

// Counter for menu clicks
byte click_count = 0;

// SD initialized
volatile bool sdReady = false;

// Sensor readings
float temperature_intern = 0.0;
float humidity_intern = 0.0;


// *****************************************************************************
// Functions
// *****************************************************************************

bool parseConfig(const byte& idx){
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

    value = configFile.getValue(F("TimeStart"), header);
    channel[idx].time.start_time = toSeconds(value);

    value = configFile.getValue(F("TimeDuration"), header);
    channel[idx].time.duration = toSeconds(value);

    value = configFile.getValue(F("TimeRepeat"), header);
    channel[idx].time.repeat = toSeconds(value);

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
  
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, HIGH);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(SD_CD_PIN, INPUT);
  pinMode(FLOW_PIN, INPUT);
  
  Serial.println(F("Done"));
}

void readSensor() {
  unsigned long current_time = millis();
  static unsigned long last_time = current_time;

  if (current_time - last_time > 10000) {
    Serial.print(F("[MEGA2560] Read temperature/humidity..."));
    if (am2321.read() == AM232X_OK) {
      temperature_intern = am2321.getTemperature();
      humidity_intern  = am2321.getHumidity();
      Serial.print(F(" ( "));
      Serial.print(temperature_intern, 1);
      Serial.print(F("°C | "));
      Serial.print(humidity_intern, 1);
      Serial.println(F("% )"));

      if (temperature_intern > 30.0) {
        digitalWrite(FAN_PIN, LOW);
      }
      else if (temperature_intern < 25.0) {
        digitalWrite(FAN_PIN, HIGH);
      }
    }
    else {
      Serial.println(F("Failed"));
    }
    last_time = current_time;
  }
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
  if (readRequest(streamIn, request)) {
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

          if (key == F("enabled")) {
            channel[channelIdx].enable(toBool(value));
          }
          else if (key == F("skip")) {
            channel[channelIdx].doSkip(toBool(value));
          }
          else if (key == F("time")) {
            channel[channelIdx].time.setStartTime(toTime(value));
          }
          else if (key == F("duration")) {
            channel[channelIdx].time.setDuration(toSeconds(value));
          }
          else if (key == F("repeat")) {
            channel[channelIdx].time.setRepeat(toSeconds(value));
          }

          startIdx = endIdx;
        } while (endIdx < request.length());

        // Send response xml
        channel[channelIdx].printXML(streamIn, String(channelIdx));
      }
      else {
        channelToXML(streamIn);
      }
    }
    else {
      streamOut.println("[ESP8266]  " + request);
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
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) {
     Serial.println("Failed");
  }
  else {
    Serial.println(F("Done"));
  }

  // Write boot notice to log
  Serial.print(F("[MEGA2560] Write notice to log..."));
  if(SD.begin(SD_CS_PIN)) {
    logFile = SD.open(LOGFILE, (O_READ | O_WRITE | O_CREAT | O_APPEND));
    if (logFile) {
      char string_buf[40];
      sprintf (string_buf, "[%d.%02d.%02d|%02d:%02d:%02d]Booted", year(now()), month(now()), day(now()), hour(now()), minute(now()), second(now()));
      logFile.println(string_buf);
      logFile.close();
      Serial.println(F("Done"));
    }
    else {
      Serial.println(F("Error opening file"));
    }
  }
  else {
    Serial.println(F("Error starting SD"));
  }

  // Attach interrupts
  Serial.print(F("[MEGA2560] Attach interrupts....."));
  attachInterrupt(digitalPinToInterrupt(SD_CD_PIN), sdDetectInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), flowCounterInterrupt, RISING);
  Serial.println(F("Done"));

  // Parse config file
  //LCDML.OTHER_jumpToFunc(mFunc_readSD);
  LCDML.OTHER_jumpToFunc(mFunc_readEEPROM);
}


// *****************************************************************************
// Loop
// *****************************************************************************

void loop() {
  static unsigned long button_time = 0;
  static byte button_prev = LOW;
  
  for (byte i = 0; i < NUM_CHANNEL; i++) {
    if (channel[i].enabled) {
      // Check timer
      channel[i].active = channel[i].time.active(now());

      if (channel[i].active != channel[i].was_active) {
        if (channel[i].was_active) {
          // Write statistics to sd card
          if(SD.begin(SD_CS_PIN)) {
            logFile = SD.open(LOGFILE, (O_READ | O_WRITE | O_CREAT | O_APPEND));
            char string_buf[40];
            if (logFile) {
              float liter = (flowCounter - channel[i].flow.start_count)/FLOW_CONV;
              sprintf (string_buf, "[%d.%02d.%02d|%02d:%02d:%02d](Ch%02d)", year(now()), month(now()), day(now()), hour(channel[i].time.start_time), minute(channel[i].time.start_time), second(channel[i].time.start_time), i);
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
        channel[i].active = channel[i].flow.active(flowCounter);
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

    // Update buttons
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
            // Toggle skip
            channel[i].active = !channel[i].active;
          }
        }
      }
    }

    // Update signals
    if (channel[i].signal != 0) {
      if (channel[i].enabled) {
        if (channel[i].skip) {
          digitalWrite(channel[i].signal, (millis()%3000 < 200) );
        }
        else if (channel[i].time.active(now())) {
          if (!channel[i].active) {  // Executen prohibited by some sensor
            digitalWrite(channel[i].signal, (millis()%1000 < 50) );
          }
          else {
            digitalWrite(channel[i].signal, (millis()%1000 < 500) );
          }
        }
        else if (channel[i].time.preactive(now(), 60)) {
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
  }

  // Activate pump
  if ((channel[0].active && !channel[0].skip) ||
      (channel[1].active && !channel[1].skip) ||
      (channel[2].active && !channel[2].skip) ||
      (channel[3].active && !channel[3].skip)) {
    digitalWrite(PUMP_PIN, LOW);
  }
  else {
    digitalWrite(PUMP_PIN, HIGH);
  }

  // Read temperature sensor
  readSensor();

  // Handle request from webserver
  handleRequest(Serial3);

  // Update display
  updateMenu();
}
