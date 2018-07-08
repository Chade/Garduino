#ifndef _GARDUINO_H_
#define _GARDUINO_H_

#include <Arduino.h>
#include <FileConfigHelper.h>


// *****************************************************************************
// Communication
// *****************************************************************************

#define BAUD_RATE                    115200

// *****************************************************************************
// Pins
// *****************************************************************************

#define FLOW_PIN                     2
#define SD_CD_PIN                    3
#define SD_CS_PIN                    53
#define LCD_CS_PIN                   49
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
#define ENCODER_A_PIN                19    // physical pin has to be 2 or 3 to use interrupts (on mega e.g. 20 or 21), use internal pullups
#define ENCODER_B_PIN                18    // physical pin has to be 2 or 3 to use interrupts (on mega e.g. 20 or 21), use internal pullups
#define ENCODER_BUTTON_PIN           17    // physical pin , use internal pullup
#define EXTERNAL_BUTTON_PIN          24    // physical pin , use internal pullup


// *****************************************************************************
// Delays
// *****************************************************************************

#define CONTROL_BUTTON_DEBOUNCE      200   // ms
#define CONTROL_BUTTON_LONG_PRESS    800   // ms
#define CONTROL_BUTTON_SHORT_PRESS   120   // ms


// *****************************************************************************
// Display settings
// *****************************************************************************

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
#define LCDML_DISP_FRAME_OFFSET      LCDML_DISP_BOX_X0 + LCDML_DISP_FONT_W + LCDML_DISP_CUR_SPACE_BEHIND
// Scrollbar settings
#define LCDML_DISP_SCROLL_WIDTH      6              // scrollbar width (if this value is < 3, the scrollbar is disabled)
// Rows and cols
#define LCDML_DISP_COLS_MAX          ((LCDML_DISP_BOX_X1-LCDML_DISP_BOX_X0)/LCDML_DISP_FONT_W)
#define LCDML_DISP_ROWS_MAX          ((LCDML_DISP_BOX_Y1-LCDML_DISP_BOX_Y0-((LCDML_DISP_BOX_Y1-LCDML_DISP_BOX_Y0)/LCDML_DISP_FONT_H))/LCDML_DISP_FONT_H)
// When you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
// The program needs more ram with this changes
#define LCDML_DISP_ROWS              LCDML_DISP_ROWS_MAX  // max rows
#define LCDML_DISP_COLS              20                   // max cols


// *****************************************************************************
// General
// *****************************************************************************

#define NUM_CHANNEL                  4
#define FILE_NAME                    "config.txt"
#define BACKUP_NAME                  "backup.txt"


// *****************************************************************************
// Classes
// *****************************************************************************

class Switch {
public:
  bool enabled;
  bool inverted;
  byte input;

public:
  bool isEnabled(){
    return enabled;
  }
  
  void enable(const bool& value){
    enabled = value;
  }
  
  bool isInverted(){
    return inverted;
  }
  
  void invert(const bool& value){
    inverted = value;
  }
  
  byte getPin(){
    return input;
  }
  
  void setPin(const byte& pin){
    input = pin;
  }

  void print(Stream &stream, const String& name = "") {
    stream.print(name);
    stream.print(F("Enabled = "));
    stream.println(fromBool(isEnabled()));
    stream.print(name);
    stream.print(F("Invert = "));
    stream.println(fromBool(isInverted()));
    stream.print(name);
    stream.print(F("Pin = "));
    stream.println(getPin());
  }
};

class Timer {
public:
  time_t start_time;
  time_t duration;

public:  
  void print(Stream &stream, const String &name) {
    stream.print(name);
    stream.print(F("Start = "));
    stream.println(fromTime(start_time));
    stream.print(name);
    stream.print(F("Duration = "));
    stream.println(fromTime(duration));
  }
  
  bool active(const time_t& now) {
    return (elapsedSecsToday(now) >= start_time) && ((elapsedSecsToday(now) < start_time + duration) || (duration == 0));
  }

  void setStartTime(const uint8_t& hour, const uint8_t& minute, const uint8_t& second){
    start_time = hour * SECS_PER_HOUR + minute * SECS_PER_MIN + second;
  }
  
  void setStartTime(const time_t& time){
    start_time = time;
  }
  
  time_t getStartTime(){
    return start_time;
  }
};

class Counter {
public:
  uint32_t start_count;
  uint32_t count;

  void print(Stream &stream, const String &name) {
    stream.print(name);
    stream.print(F("Count = "));
    stream.println(count);
  }
  
  bool active(const volatile unsigned long& current){
    return (current - start_count) < count;
  }
};

class DigitalSwitch : public Switch {
public:
  uint32_t delay;
  
public:
  void print(Stream &stream, const String &name) {
    Switch::print(stream, name);
  }
  
  bool active(){
    return getValue() ^ isInverted();
  }
  
  bool getValue(){
    return digitalRead(getPin());
  }
};

class AnalogSwitch : public Switch {
public:
  uint16_t threshold_low = 0;
  uint16_t threshold_high = 1024;

 public:
  void print(Stream &stream, const String &name) {
    Switch::print(stream, name);
    stream.print(name);
    stream.print(F("ThresholdLow = "));
    stream.println(threshold_low);
    stream.print(name);
    stream.print(F("ThresholdHigh = "));
    stream.println(threshold_high);
  }

  bool active(){
    int value = getValue();

    if(value > threshold_low && value < threshold_high) {
      return !isInverted();
    }
    return isInverted();
  }
  
  int getValue(){
    return analogRead(getPin());
  }
};

class Channel {
public:
  bool enabled;
  bool active;
  bool was_active;
  bool skip;
  byte output;

  Timer time;
  Counter flow;
  DigitalSwitch movement;
  AnalogSwitch moisture;
  AnalogSwitch rain;
  AnalogSwitch brightness;

public:
  bool enable(const bool& value){
    enabled = value;
  }
  
  bool isEnabled(){
    return enabled;
  }
  
  void print(Stream &stream, const String &name) {
    stream.println("################################################################################");
    stream.println();
        
    stream.print('[');
    stream.print(name);
    stream.println(']');

    stream.print(F("Enabled = "));
    stream.println(fromBool(isEnabled()));
    stream.print(F("OutputPin = "));
    stream.println(output);
    stream.println();

    time.print(stream, "Time");
    stream.println();

    flow.print(stream, "Flow");
    stream.println();

	movement.print(stream, "Movement");
    stream.println();
	
    moisture.print(stream, "Moisture");
    stream.println();

    rain.print(stream, "Rain");
    stream.println();

    brightness.print(stream, "Brightness");
    stream.println();
  }
};

#endif
