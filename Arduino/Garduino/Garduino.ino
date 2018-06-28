#include <SD.h>
//#include <RTClib.h>
#include <U8g2lib.h>
#include <TimeLib.h>
#include <LCDMenuLib2.h>

#include <FileConfig.h>
#include <FileConfigHelper.h>

#include "src/Garduino.h"
#include "src/GarduinoLib/GarduinoLib.h"


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

// RealTimeClock DS1307
//RTC_DS1307 rtc;

// Channel storage
Channel channel[NUM_CHANNEL];

// Flow counter
volatile unsigned long flowCounter = 0;

// Decay counter
unsigned long delayCounter = 0;

bool error = false;

// Current channel
byte current = 0;

byte click_count = 0;

// *****************************************************************************
// Menu
// *****************************************************************************

//LCDML_addAdvanced (id, prev_layer   , num, condition           , name              , callback_function , param, menu function type   );
  LCDML_addAdvanced ( 0, LCDML_0      ,   1, NULL                , "Home"            , mFunc_home        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 1, LCDML_0      ,   2, NULL                , "Channel"         , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 2, LCDML_0_2    ,   1, NULL                , ""                , mDyn_ch_select    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 3, LCDML_0_2    ,   2, NULL                , ""                , mDyn_ch_enable    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 4, LCDML_0_2    ,   3, NULL                , ""                , mDyn_ch_start     , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 5, LCDML_0_2    ,   4, NULL                , ""                , mDyn_ch_duration  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 6, LCDML_0_2    ,   5, NULL                , ""                , mDyn_ch_flow      , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 7, LCDML_0_2    ,   6, NULL                , " Moisture"       , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 8, LCDML_0_2_6  ,   1, NULL                , ""                , mDyn_moist_enable , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 9, LCDML_0_2_6  ,   2, NULL                , ""                , mDyn_moist_invert , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (10, LCDML_0_2_6  ,   3, NULL                , ""                , mDyn_moist_low    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (11, LCDML_0_2_6  ,   4, NULL                , ""                , mDyn_moist_high   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (12, LCDML_0_2_6  ,   5, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (13, LCDML_0_2    ,   7, NULL                , " Rain"           , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (14, LCDML_0_2_7  ,   1, NULL                , ""                , mDyn_rain_enable  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (15, LCDML_0_2_7  ,   2, NULL                , ""                , mDyn_rain_invert  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (16, LCDML_0_2_7  ,   3, NULL                , ""                , mDyn_rain_low     , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (17, LCDML_0_2_7  ,   4, NULL                , ""                , mDyn_rain_high    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (18, LCDML_0_2_7  ,   5, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (19, LCDML_0_2    ,   8, NULL                , " Movement"       , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (20, LCDML_0_2_8  ,   1, NULL                , ""                , mDyn_move_enable  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (21, LCDML_0_2_8  ,   2, NULL                , ""                , mDyn_move_invert  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (22, LCDML_0_2_8  ,   3, NULL                , ""                , mDyn_move_wait    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (23, LCDML_0_2_8  ,   5, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (24, LCDML_0_2    ,   9, NULL                , " Brightness"     , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (25, LCDML_0_2_9  ,   1, NULL                , ""                , mDyn_bright_enable, 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (26, LCDML_0_2_9  ,   2, NULL                , ""                , mDyn_bright_invert, 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (27, LCDML_0_2_9  ,   3, NULL                , ""                , mDyn_bright_low   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (28, LCDML_0_2_9  ,   4, NULL                , ""                , mDyn_bright_high  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (29, LCDML_0_2_9  ,   5, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (30, LCDML_0_2    ,  10, NULL                , " <Back>"         , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (31, LCDML_0      ,   3, NULL                , "Settings"        , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (32, LCDML_0_3    ,   1, NULL                , "Set Clock"       , mFunc_set_clock   , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (33, LCDML_0_3    ,   2, NULL                , "Read from SD"    , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (34, LCDML_0_3    ,   3, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );

// Menu element count
// This value must be the same as the last menu element
#define LCDML_DISP_COUNT 33

// Create menu
LCDML_createMenu(LCDML_DISP_COUNT);

// *****************************************************************************
// Functions
// *****************************************************************************


bool parseConfig(){
  FileConfig configFile(SD.open(FILE_NAME, FILE_READ));
  
  for(uint8_t i = 0; i < NUM_CHANNEL; i++)
  {
    String value;
    String header(F("Channel"));
    header.concat(i);
    
    Serial.println(header);
    
    value = configFile.getValue(F("Enabled"), header);
    channel[i].enabled = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("TimeStart"), header);
    channel[i].time.start_time = toSeconds(value);
    Serial.println(value);
    
    value = configFile.getValue(F("TimeDuration"), header);
    channel[i].time.duration = toSeconds(value);
    Serial.println(value);
    
    value = configFile.getValue(F("FlowCount"), header);
    channel[i].flow.count = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("MovementEnabled"), header);
    channel[i].movement.enabled = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("MovementInvert"), header);
    channel[i].movement.inverted = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("MovementPin"), header);
    channel[i].movement.input = value.toInt();
    Serial.println(value);

//    value = configFile.getValue(F("MovementDelay"), header);
//    channel[i].movement.delay = value.toInt();
//    Serial.println(value);
    
    value = configFile.getValue(F("MoistureEnabled"), header);
    channel[i].moisture.enabled = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("MoistureInvert"), header);
    channel[i].moisture.inverted = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("MoisturePin"), header);
    channel[i].moisture.input = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("MoistureThresholdLow"), header);
    channel[i].moisture.threshold_low = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("MoistureThresholdHigh"), header);
    channel[i].moisture.threshold_high = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("RainEnabled"), header);
    channel[i].rain.enabled = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("RainInvert"), header);
    channel[i].rain.inverted = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("RainPin"), header);
    channel[i].rain.input = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("RainThresholdLow"), header);
    channel[i].rain.threshold_low = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("RainThresholdHigh"), header);
    channel[i].rain.threshold_high = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("BrightnessEnabled"), header);
    channel[i].brightness.enabled = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("BrightnessInvert"), header);
    channel[i].brightness.inverted = toBool(value);
    Serial.println(value);
    
    value = configFile.getValue(F("BrightnessPin"), header);
    channel[i].brightness.input = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("BrightnessThresholdLow"), header);
    channel[i].brightness.threshold_low = value.toInt();
    Serial.println(value);
    
    value = configFile.getValue(F("BrightnessThresholdHigh"), header);
    channel[i].brightness.threshold_high = value.toInt();
    Serial.println(value);
    /*
    Serial.println( configFile.getValue(F("Enabled")                , header + String(i)) );
    Serial.println( configFile.getValue(F("TimeStart")              , header + String(i)) );
    Serial.println( configFile.getValue(F("TimeDuration")           , header + String(i)) );
    Serial.println( configFile.getValue(F("FlowCount")              , header + String(i)) );
    Serial.println( configFile.getValue(F("MovementEnabled")        , header + String(i)) );
    Serial.println( configFile.getValue(F("MovementInvert")         , header + String(i)) );
    Serial.println( configFile.getValue(F("MovementPin")            , header + String(i)) );
    Serial.println( configFile.getValue(F("MoistureEnabled")        , header + String(i)) );
    Serial.println( configFile.getValue(F("MoistureInvert")         , header + String(i)) );
    Serial.println( configFile.getValue(F("MoisturePin")            , header + String(i)) );
    Serial.println( configFile.getValue(F("MoistureThresholdLow")   , header + String(i)) );
    Serial.println( configFile.getValue(F("MoistureThresholdHigh")  , header + String(i)) );
    Serial.println( configFile.getValue(F("RainEnabled")            , header + String(i)) );
    Serial.println( configFile.getValue(F("RainInvert")             , header + String(i)) );
    Serial.println( configFile.getValue(F("RainPin")                , header + String(i)) );
    Serial.println( configFile.getValue(F("RainThresholdLow")       , header + String(i)) );
    Serial.println( configFile.getValue(F("RainThresholdHigh")      , header + String(i)) );
    Serial.println( configFile.getValue(F("BrightnessEnabled")      , header + String(i)) );
    Serial.println( configFile.getValue(F("BrightnessInvert")       , header + String(i)) );
    Serial.println( configFile.getValue(F("BrightnessPin")          , header + String(i)) );
    Serial.println( configFile.getValue(F("BrightnessThresholdLow") , header + String(i)) );
    Serial.println( configFile.getValue(F("BrightnessThresholdHigh"), header + String(i)) );
    */
  }

  configFile.close();
  return true;
}

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



// *****************************************************************************
// Setup
// *****************************************************************************

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    delay(100); // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();

  // Initiialize RTC
  /*Serial.print(F("Initializing RTC..."));
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
  */

  // Initialize SD Card
  Serial.print(F("Initializing SD card..."));
  while (!SD.begin(SD_CS_PIN)) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(F("Done"));

  // Parse config file
  Serial.print(F("Parsing config file..."));
//  File configFile = SD.open(FILE_NAME, FILE_READ);
//  if (!GarduinoLib::readConfig(configFile, channel, NUM_CHANNEL)) {
//    Serial.println(F("Error opening file from SD-Card"));
//    error = true;
//  }
//  configFile.close();

  parseConfig();
  
  Serial.println(F("Done"));

  // Set up IOs
  Serial.print(F("Setting up I/Os..."));
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  channel[0].pin_out = RELAY1_PIN;
  channel[1].pin_out = RELAY2_PIN;
  channel[2].pin_out = RELAY3_PIN;
  channel[3].pin_out = RELAY4_PIN;
  //TODO Setup the other 4 pins as well

  //attachInterrupt(digitalPinToInterrupt(FLOW_PIN), flowCounterInterrupt, RISING);
  Serial.println(F("Done"));

  // Setup LCD
  Serial.print(F("Initializing LCD..."));
  u8g2.begin();

  // LCDMenuLib Setup
  LCDML_setup(LCDML_DISP_COUNT);

  // Enable Menu Rollover
  LCDML.MENU_enRollover();

  // Enable home menu
  LCDML.SCREEN_enable(mFunc_home, 10000); // set to 10 seconds
  Serial.println(F("Done"));
}


// *****************************************************************************
// Loop
// *****************************************************************************

void loop() {
  // Get current time
  //now = rtc.now();

  for (byte i = 0; i < NUM_CHANNEL; i++) {
    if (channel[i].enabled) {
      // Check timer
      channel[i].active = channel[i].time.active(now());

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

      Serial.print(F("Rain = "));
      Serial.println(analogRead(A0));
      Serial.print(F("Moisture = "));
      Serial.println(analogRead(A1));
      Serial.print(F("Brightness = "));
      Serial.println(analogRead(A2));

      digitalWrite(channel[i].pin_out, channel[i].active);
    }
  }

  // Update display
  LCDML.loop();
}
