#ifndef _GARDUINO_H_
#define _GARDUINO_H_

#include <Arduino.h>
#include <FileConfigHelper.h>


// *****************************************************************************
// Communication
// *****************************************************************************

#define BAUD_RATE                    115200
//#define READ_FROM_EEPROM

// *****************************************************************************
// Pins
// *****************************************************************************

// I2C
#define SDA                          20
#define SCL                          21
// SPI
#define MISO                         50
#define MOSI                         51
#define SCK                          52
// SD-Card
#define SD_CD_PIN                    3
#define SD_CS_PIN                    53
// LCD
#define LCD_CS_PIN                   49
// Sensor
#define MOISTURE_PIN                 A0
#define RAIN_PIN                     A1
#define BRIGHTNESS_PIN               A2
#define MOVEMENT_PIN                 48
#define FLOW_PIN                     2
#define AUX_PIN                      10
#define FAN_PIN                      11
// Encoder
#define ENCODER_A_PIN                19    // physical pin has to be 2 or 3 to use interrupts (on mega e.g. 20 or 21), use internal pullups
#define ENCODER_B_PIN                18    // physical pin has to be 2 or 3 to use interrupts (on mega e.g. 20 or 21), use internal pullups
#define ENCODER_BUTTON_PIN           17    // physical pin , use internal pullup
#define EXTERNAL_BUTTON_PIN          24    // physical pin , use internal pullup
// Buzzer
#define BUZZER_PIN                   25
// Buttons
#define BUTTON1_PIN                  26
#define BUTTON1_LED_PIN              30
#define BUTTON2_PIN                  27
#define BUTTON2_LED_PIN              31
#define BUTTON3_PIN                  28
#define BUTTON3_LED_PIN              32
#define BUTTON4_PIN                  29
#define BUTTON4_LED_PIN              33

// *****************************************************************************
// Delays
// *****************************************************************************

#define CONTROL_BUTTON_DEBOUNCE      200   // ms
#define CONTROL_BUTTON_LONG_PRESS    800   // ms
#define CONTROL_BUTTON_SHORT_PRESS   120   // ms

// *****************************************************************************
// Constants
// *****************************************************************************

#define FLOW_CONV                    600.0 // ticks per liter
#define UTC2CET                         60 // Offset from winter time to UTC
#define UTC2CEST                       120 // Offset from summer time to UTC
#define LATITUDE                  49.07703 // position for sunrise/sunset calculation
#define LONGITUDE                  9.19951 // position for sunrise/sunset calculation

// *****************************************************************************
// Display settings
// *****************************************************************************

#define LCDML_DISP_WIDTH             128                  // LCD width
#define LCDML_DISP_HEIGHT            64                   // LCD height
// Font settings
#define LCDML_DISP_FONT              u8g2_font_6x12_tf    // u8glib font (more fonts under u8g.h line 1520 ...)
#define LCDML_DISP_FONT_W            6                    // font width
#define LCDML_DISP_FONT_H            12                   // font height
// Cursor settings
#define LCDML_DISP_CURSOR_CHAR       ">"                  // cursor char
#define LCDML_DISP_CUR_SPACE_BEFORE  2                    // cursor space between
#define LCDML_DISP_CUR_SPACE_BEHIND  4                    // cursor space between
// Menu position and size
#define LCDML_DISP_BOX_X0            0                    // start point (x0, y0)
#define LCDML_DISP_BOX_Y0            0                    // start point (x0, y0)
#define LCDML_DISP_BOX_X1            128                  // width x  (x0 + width)
#define LCDML_DISP_BOX_Y1            64                   // hight y  (y0 + height)
#define LCDML_DISP_FRAME             1                    // draw a box around the menu
#define LCDML_DISP_FRAME_OFFSET      LCDML_DISP_BOX_X0 + LCDML_DISP_FONT_W + LCDML_DISP_CUR_SPACE_BEHIND
// Scrollbar settings
#define LCDML_DISP_SCROLL_WIDTH      6                    // scrollbar width (if this value is < 3, the scrollbar is disabled)
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

#define NUM_CHANNEL                  6
#define FILE_NAME                    "config.txt"
#define BACKUP_NAME                  "backup.txt"
#define LOGFILE                      "watering.log"


// *****************************************************************************
// Classes
// *****************************************************************************

class Switch {
public:
  bool enabled = false;
  bool inverted = false;
  byte input = 0;

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
  time_t start_time = 0;
  time_t duration = 0;
  time_t repeat = 0;

  uint8_t adjustSetpoint = 0;
  signed_time_t adjustOffset = 0;

  enum setpoint_enum {ENone = 0, ESunrise = 1, ENoon = 2, ESunset = 3};
  const char * const setpoint_names[4] = {"none", "sunrise", "noon", "sunset"};

public:
  void print(Stream &stream, const String &name) {
    stream.print(name);
    stream.print(F("Start = "));
    stream.println(fromTime(start_time));
    stream.print(name);
    stream.print(F("Duration = "));
    stream.println(fromTime(duration));
    stream.print(name);
    stream.print(F("Repeat = "));
    stream.println(fromTime(repeat));
    stream.print(name);
    stream.print(F("AdjustSetpoint = "));
    stream.println(getAdjustSetpoint());
    stream.print(name);
    stream.print(F("AdjustOffset = "));
    stream.println(fromSignedTime(adjustOffset));
  }

  bool preactive(const time_t& current_time, const time_t& in) {
    bool is_active = false;
    
    if ((elapsedSecsToday(current_time) + in) >= start_time) {
      if (repeat > 0) {
        time_t delta = elapsedSecsToday(current_time) + in - start_time;
        is_active = ((delta % repeat) < duration) || (duration == 0);
      }
      else {
        is_active = ((elapsedSecsToday(current_time) + in) < (start_time + duration)) || (duration == 0);
      }
    }
    return is_active;
  }

  bool active(const time_t& current_time) {
    return preactive(current_time, 0);
  }

  void setStartTime(const uint8_t& hour, const uint8_t& minute, const uint8_t& second){
    start_time = hour * SECS_PER_HOUR + minute * SECS_PER_MIN + second;
  }

  void setStartTime(const time_t& new_time){
    start_time = new_time;
  }

  time_t getStartTime(){
    return start_time;
  }

  time_t getNextStartTime(const time_t& current_time){
    if ((repeat != 0) && (duration != 0) && (elapsedSecsToday(current_time) >= start_time)) {
      time_t delta = elapsedSecsToday(current_time) - start_time;
      uint32_t reps = (delta / repeat) + 1;
      return start_time + (reps * repeat);
    }
    return start_time;
  }

  void setDuration(const time_t& new_duration)
  {
    duration = new_duration;
  }

  time_t getDuration()
  {
    return duration;
  }

  void setRepeat(const time_t& new_repeat)
  {
    repeat = new_repeat;
  }

  time_t getRepeat()
  {
    return repeat;
  }

  void setAdjustSetpoint(String new_setpoint)
  {
    new_setpoint.trim();
    if (new_setpoint.equalsIgnoreCase("sunrise")) {
      adjustSetpoint = ESunrise;
    }
    else if (new_setpoint.equalsIgnoreCase("noon")) {
      adjustSetpoint = ENoon;
    }
    else if (new_setpoint.equalsIgnoreCase("sunset")) {
      adjustSetpoint = ESunset;
    }
    else {
      adjustSetpoint = ENone;
    }
  }

  void setAdjustSetpoint(const uint8_t& new_setpoint)
  {
    adjustSetpoint = new_setpoint;
  }

  String getAdjustSetpoint()
  {
    return String(setpoint_names[adjustSetpoint]);
  }

  bool isAdjustEnabled()
  {
    return adjustSetpoint > 0;
  }

  void setAdjustOffset(const signed_time_t& new_offset)
  {
    adjustOffset = new_offset;
  }

  signed_time_t getAdjustOffset()
  {
    return adjustOffset;
  }
};

class Counter {
public:
  uint32_t start_count = 0;
  uint32_t count = 0;

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
  uint32_t delay = 0;

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
  bool enabled = false;
  bool active = false;
  bool was_active = false;
  bool skip = false;
  bool aux = false;
  byte output = 0;
  byte input = 0;
  byte signal = 0;

  Timer time;
  Counter flow;
  DigitalSwitch movement;
  AnalogSwitch moisture;
  AnalogSwitch rain;
  AnalogSwitch brightness;

public:
  void enable(const bool& value){
    enabled = value;
  }

  bool isEnabled(){
    return enabled;
  }

  void activate(const bool& value){
    active = value;
  }

  bool isActive(){
    return active;
  }

  bool wasActive(){
    return was_active;
  }

  void doSkip(const bool& value){
    skip = value;
  }

  bool isSkipped(){
    return skip;
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
    stream.print(F("InputPin = "));
    stream.println(input);
    stream.print(F("SignalPin = "));
    stream.println(signal);
    stream.print(F("AuxPin = "));
    stream.println(fromBool(aux));
    stream.println();

    time.print(stream, F("Time"));
    stream.println();

    flow.print(stream, F("Flow"));
    stream.println();

	  movement.print(stream, F("Movement"));
    stream.println();

    moisture.print(stream, F("Moisture"));
    stream.println();

    rain.print(stream, F("Rain"));
    stream.println();

    brightness.print(stream, F("Brightness"));
    stream.println();
  }
  
  void printXML(Stream& stream, const String &name, const bool& header = true) {
    String state;
    if (skip) {
      state = F("skip");
    }
    else if (active) {
      state = F("active");
    }
    else {
      state = F("idle");
    }

    if (header) {
      stream.println(F("<?xml version = \"1.0\" ?>"));
      stream.println(F("<channels>"));
    }
    
    stream.print(F("<channel id=\""));
    stream.print(name);
    stream.println(F("\">"));
    
    stream.print(F("<enabled>"));
    stream.print(fromBool(isEnabled()));
    stream.println(F("</enabled>"));
    
    stream.print(F("<state>"));
    stream.print(state);
    stream.println(F("</state>"));
    
    stream.print(F("<time>"));
    stream.print(fromTime(time.getStartTime()));
    stream.println(F("</time>"));
    
    stream.print(F("<duration>"));
    stream.print(time.getDuration());
    stream.println(F("</duration>"));
    
    stream.print(F("<repeat>"));
    stream.print(time.getRepeat());
    stream.println(F("</repeat>"));

    stream.print(F("<setpoint>"));
    stream.print(time.getAdjustSetpoint());
    stream.println(F("</setpoint>"));

    stream.print(F("<offset>"));
    stream.print(time.getAdjustOffset());
    stream.println(F("</offset>"));
    
    stream.println(F("</channel>"));

    if (header) {
      stream.println(F("</channels>"));
      stream.println('\n');
    }
  }
};

#endif
