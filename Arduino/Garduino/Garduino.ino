#include <SPI.h>
#include <SD.h>
//#include <RTClib.h>
#include <U8g2lib.h>
#include <TimeLib.h>
#include <LCDMenuLib2.h>
#include "src/FileConfig/FileConfig.h"


// *****************************************************************************
// Defines
// *****************************************************************************

#define BAUD_RATE                    9600

// Pin settings
#define FLOW_PIN                     2
#define SD_CS_PIN                    47
#define LCD_CS_PIN                   53
#define RELAY1_PIN                   4
#define RELAY2_PIN                   5
#define RELAY3_PIN                   6
#define RELAY4_PIN                   7
#define MOISTURE_PIN                 A0
#define RAIN_PIN                     A1
#define BRIGHTNESS_PIN               A2
#define MOVEMENT_PIN                 3
#define MOVEMENT_DECAY               60
// Encoder
#define ENCODER_A_PIN        18    // physical pin has to be 2 or 3 to use interrupts (on mega e.g. 20 or 21), use internal pullups
#define ENCODER_B_PIN        19    // physical pin has to be 2 or 3 to use interrupts (on mega e.g. 20 or 21), use internal pullups
#define ENCODER_BUTTON_PIN   49    // physical pin , use internal pullup
#define EXTERNAL_BUTTON_PIN  48    // physical pin , use internal pullup
// Button
#define CONTROL_BUTTON_DEBOUNCE      200   // ms
#define CONTROL_BUTTON_LONG_PRESS    800   // ms
#define CONTROL_BUTTON_SHORT_PRESS   120   // ms
// Settings for u8g lib and LCD
#define LCDML_DISP_WIDTH             128            // LCD width
#define LCDML_DISP_HEIGHT            64             // LCD height
// Font settings
#define LCDML_DISP_FONT              u8g_font_6x12  // u8glib font (more fonts under u8g.h line 1520 ...)
#define LCDML_DISP_FONT_W            6              // font width
#define LCDML_DISP_FONT_H            12             // font height
// Cursor settings
#define LCDML_DISP_CURSOR_CHAR       ">"            // cursor char
#define LCDML_DISP_CUR_SPACE_BEFORE  2              // cursor space between
#define LCDML_DISP_CUR_SPACE_BEHIND  4              // cursor space between
// Menu position and size
#define LCDML_DISP_BOX_X0            0              // start point (x0, y0)
#define LCDML_DISP_BOX_Y0            0              // start point (x0, y0)
#define LCDML_DISP_BOX_X1            128            // width x  (x0 + width)
#define LCDML_DISP_BOX_Y1            64             // hight y  (y0 + height)
#define LCDML_DISP_FRAME             1              // draw a box around the menu
// Scrollbar settings
#define LCDML_DISP_SCROLL_WIDTH      6              // scrollbar width (if this value is < 3, the scrollbar is disabled)
// Rows and cols
#define LCDML_DISP_COLS_MAX          ((LCDML_DISP_BOX_X1-LCDML_DISP_BOX_X0)/LCDML_DISP_FONT_W)
#define LCDML_DISP_ROWS_MAX          ((LCDML_DISP_BOX_Y1-LCDML_DISP_BOX_Y0-((LCDML_DISP_BOX_Y1-LCDML_DISP_BOX_Y0)/LCDML_DISP_FONT_H))/LCDML_DISP_FONT_H)
// When you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
// The program needs more ram with this changes
#define LCDML_DISP_ROWS              LCDML_DISP_ROWS_MAX  // max rows
#define LCDML_DISP_COLS              20                   // max cols
// Channel settings
#define NUM_CHANNEL                  8
#define FILE_NAME                    "config.txt"


// *****************************************************************************
// Sanity checks
// *****************************************************************************
# if(LCDML_DISP_ROWS > _LCDML_DISP_cfg_max_rows)
# error ERROR: Change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib2.h
# endif

# if(LCDML_DISP_BOX_X1 > LCDML_DISP_WIDTH)
# error ERROR: LCDML_DISP_BOX_X1 is to big
# endif

# if(LCDML_DISP_BOX_Y1 > LCDML_DISP_HEIGHT)
# error ERROR: LCDML_DISP_BOX_Y1 is to big
# endif


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
unsigned int decayCounter = MOVEMENT_DECAY;

bool error = false;


// *****************************************************************************
// Menu
// *****************************************************************************

//LCDML_addAdvanced (id, prev_layer   , num, condition           , name              , callback_function, param, menu function type   );
  LCDML_addAdvanced ( 0, LCDML_0      ,   1, NULL                , "Home"            , mFunc_home       , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 1, LCDML_0      ,   2, NULL                , "Settings"        , NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 2, LCDML_0_2    ,   1, NULL                , "Set Date"        , mFunc_set_date   , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 3, LCDML_0_2    ,   2, NULL                , "Set Time"        , mFunc_set_time   , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 4, LCDML_0_2    ,   3, NULL                , "Activate Channel", NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 5, LCDML_0_2_3  ,   1, NULL                , ""                , mDyn_channel_0   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 6, LCDML_0_2_3  ,   2, NULL                , ""                , mDyn_channel_1   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 7, LCDML_0_2_3  ,   3, NULL                , ""                , mDyn_channel_2   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 8, LCDML_0_2_3  ,   4, NULL                , ""                , mDyn_channel_3   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 9, LCDML_0_2_3  ,   5, NULL                , ""                , mDyn_channel_4   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (10, LCDML_0_2_3  ,   6, NULL                , ""                , mDyn_channel_5   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (11, LCDML_0_2_3  ,   7, NULL                , ""                , mDyn_channel_6   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (12, LCDML_0_2_3  ,   8, NULL                , ""                , mDyn_channel_7   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (13, LCDML_0_2_3  ,  10, NULL                , "<Back>"          , mFunc_back       , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (14, LCDML_0_2    ,   4, NULL                , "<Back>"          , mFunc_back       , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (15, LCDML_0      ,   3, COND_enabled_ch0    , "Channel0"        , NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (16, LCDML_0      ,   4, COND_enabled_ch1    , "Channel1"        , NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (17, LCDML_0      ,   5, COND_enabled_ch2    , "Channel2"        , NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (18, LCDML_0      ,   6, COND_enabled_ch3    , "Channel3"        , NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (19, LCDML_0      ,   7, COND_enabled_ch4    , "Channel4"        , NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (20, LCDML_0      ,   8, COND_enabled_ch5    , "Channel5"        , NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (21, LCDML_0      ,   9, COND_enabled_ch6    , "Channel6"        , NULL             , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (22, LCDML_0      ,  10, COND_enabled_ch7    , "Channel7"        , NULL             , 0    , _LCDML_TYPE_default  );

// Menu element count
// This value must be the same as the last menu element
#define LCDML_DISP_COUNT 22

// Create menu
LCDML_createMenu(LCDML_DISP_COUNT);


// *****************************************************************************
// Functions
// *****************************************************************************

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
  File configFile = SD.open(FILE_NAME, FILE_READ);
  if (!FileConfig::readConfig(configFile, channel, NUM_CHANNEL)) {
    Serial.println(F("Error opening file from SD-Card"));
    error = true;
  }
  configFile.close();
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

  // Update display
  LCDML.loop();
}
