#include <Arduino.h>

#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <DS1307RTC.h>

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
    //Serial.println(value);

    value = configFile.getValue(F("OutputPin"), header);
    channel[idx].output = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("TimeStart"), header);
    channel[idx].time.start_time = toSeconds(value);
    //Serial.println(value);

    value = configFile.getValue(F("TimeDuration"), header);
    channel[idx].time.duration = toSeconds(value);
    //Serial.println(value);

    value = configFile.getValue(F("FlowCount"), header);
    channel[idx].flow.count = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("MovementEnabled"), header);
    channel[idx].movement.enabled = toBool(value);
    //Serial.println(value);

    value = configFile.getValue(F("MovementInvert"), header);
    channel[idx].movement.inverted = toBool(value);
    //Serial.println(value);

    value = configFile.getValue(F("MovementPin"), header);
    channel[idx].movement.input = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("MovementDelay"), header);
    channel[idx].movement.delay = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("MoistureEnabled"), header);
    channel[idx].moisture.enabled = toBool(value);
    //Serial.println(value);

    value = configFile.getValue(F("MoistureInvert"), header);
    channel[idx].moisture.inverted = toBool(value);
    //Serial.println(value);

    value = configFile.getValue(F("MoisturePin"), header);
    channel[idx].moisture.input = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("MoistureThresholdLow"), header);
    channel[idx].moisture.threshold_low = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("MoistureThresholdHigh"), header);
    channel[idx].moisture.threshold_high = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("RainEnabled"), header);
    channel[idx].rain.enabled = toBool(value);
    //Serial.println(value);

    value = configFile.getValue(F("RainInvert"), header);
    channel[idx].rain.inverted = toBool(value);
    //Serial.println(value);

    value = configFile.getValue(F("RainPin"), header);
    channel[idx].rain.input = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("RainThresholdLow"), header);
    channel[idx].rain.threshold_low = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("RainThresholdHigh"), header);
    channel[idx].rain.threshold_high = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("BrightnessEnabled"), header);
    channel[idx].brightness.enabled = toBool(value);
    //Serial.println(value);

    value = configFile.getValue(F("BrightnessInvert"), header);
    channel[idx].brightness.inverted = toBool(value);
    //Serial.println(value);

    value = configFile.getValue(F("BrightnessPin"), header);
    channel[idx].brightness.input = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("BrightnessThresholdLow"), header);
    channel[idx].brightness.threshold_low = value.toInt();
    //Serial.println(value);

    value = configFile.getValue(F("BrightnessThresholdHigh"), header);
    channel[idx].brightness.threshold_high = value.toInt();
    //Serial.println(value);

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
  while (!Serial) {
    delay(100); // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();

  // Initialize LCD
  Serial.print(F("Initializing LCD..."));
  initMenu();
  Serial.println(F("Done"));

  // Initiialize RTC
  Serial.print(F("Initializing RTC..."));
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) {
     Serial.println("Failed");
  }
  else {
    Serial.println(F("Done"));
  }

  // Attach sd card detection interrupt
  attachInterrupt(digitalPinToInterrupt(SD_CD_PIN), sdDetectInterrupt, FALLING);

  // Parse config file
  if(!LCDML.OTHER_jumpToFunc(mFunc_readSD)) {
    Serial.println(F("Parsing config file...Failed"));
  }
}


// *****************************************************************************
// Loop
// *****************************************************************************

void loop() {
  for (byte i = 0; i < NUM_CHANNEL; i++) {
    if (channel[i].enabled) {
      // Check timer
      channel[i].active = channel[i].time.active(now());

      if (channel[i].active != channel[i].was_active) {
        if (channel[i].was_active) {
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

      // Check moisture sensor to deactivate channel
      if (channel[i].active && channel[i].brightness.enabled) {
        channel[i].active &= channel[i].brightness.active();
      }

      digitalWrite(channel[i].output, channel[i].active && !channel[i].skip);
    }
  }

  // Update display
  updateMenu();
}
