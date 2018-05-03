#include <SD.h>
#include <RTClib.h>
#include <FileConfig.h>

#define BAUD_RATE    9600

#define FLOW_PIN        2
#define SD_CS_PIN      10
#define SD_MOSI_PIN    11
#define SD_MISO_PIN    12
#define SD_SCK_PIN     13
#define SDA_PIN        A4
#define SCL_PIN        A5
#define RELAY1_PIN      4
#define RELAY2_PIN      5
#define RELAY3_PIN      6
#define RELAY4_PIN      7
#define MOISTURE_PIN   A0
#define RAIN_PIN       A1
#define BRIGHTNESS_PIN A2
#define MOVEMENT_PIN    3
#define MOVEMENT_DECAY 60
#define NUM_CHANNEL     4

#define FILE_NAME "config.txt"

// RealTimeClock DS1307
RTC_DS1307 rtc;

// Channel storage
Channel channel[NUM_CHANNEL];

// Flow counter
volatile unsigned long flowCounter = 0;

// Decay counter
unsigned int decayCounter = MOVEMENT_DECAY;

bool error = false;

// Last time movement was detected
DateTime now;


void errorBlink(const byte& strobe = 5) {
  byte count = 0;
  bool ledState = false;
  pinMode(LED_BUILTIN, OUTPUT);

  while(true) {
    if (count <= strobe) {
      count++;
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
      delay(50);
    }
    else{
      count = 0;
      delay(500);
    }
  }
}

void flowCounterInterrupt() {
  flowCounter += 1;
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    delay(100); // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();

// Initiialize RTC:
  Serial.print(F("Initializing RTC..."));
  while (!rtc.begin()) {
    Serial.print('.');
    delay(100);
  }
  Serial.println(F("Done"));

  // Adjust RTC
  Serial.print(F("Adjusting RTC..."));
  if (rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.print(FileConfig::fromTime(rtc.now().hour(), rtc.now().minute()));
    Serial.println(F("...Done"));
  }
  else {
    Serial.println(F("Failed"));
  }

  // Initialize SD Card:
  Serial.print(F("Initializing SD card..."));
  while (!SD.begin(SD_CS_PIN)) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(F("Done"));

  // Parse config file
  Serial.print(F("Parsing config file..."));
  File configFile = SD.open(FILE_NAME, FILE_READ);
  if (!FileConfig::readConfig(configFile, channel, NUM_CHANNEL)) {
    Serial.println(F("Error opening file from SD-Card"));
    error = true;
  }
  configFile.close();
  Serial.println(F("Done"));

  Serial.print(F("Setting up I/Os..."));
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  channel[0].pin_out = RELAY1_PIN;
  channel[1].pin_out = RELAY2_PIN;
  channel[2].pin_out = RELAY3_PIN;
  channel[3].pin_out = RELAY4_PIN;

  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), flowCounterInterrupt, RISING);

  Serial.println(F("Done"));
}

void loop() {
  // Get current time
  now = rtc.now();

  for (byte i = 0; i < NUM_CHANNEL; i++) {
    if (channel[i].enabled) {
      // Check timer
      channel[i].active = channel[i].time.active(now);

      if (channel[i].active != channel[i].was_active) {
        if (!channel[i].was_active) {
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
          decayCounter = 0;
          channel[i].active = false;
        }
        else if (decayCounter < MOVEMENT_DECAY) {
          decayCounter++;
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

      // Check moisture sensor to deactivate channel
      if (channel[i].active && channel[i].brightness.enabled) {
        channel[i].active &= channel[i].brightness.active();
      }

      Serial.print(F("Rain = "));
      Serial.println(analogRead(A0));
      Serial.print(F("Moisture = "));
      Serial.println(analogRead(A1));
      Serial.print(F("Brightness = "));
      Serial.println(analogRead(A2));

      digitalWrite(channel[i].pin_out, channel[i].active);
    }
  }

  // Wait one second
  delay(1000);
}
