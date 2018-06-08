#ifndef _FILECONFIG_H_
#define _FILECONFIG_H_

#include <Arduino.h>
#include <TimeLib.h>
#include <SD.h>

class Switch {
public:
  bool isEnabled();
  void enable(const bool& value);
  bool isInverted();
  void invert(const bool& value);
  byte getPin();
  void setPin(const byte& pin);

  void print();

  bool enabled;
  bool inverted;
  byte input;
};

class Timer {
public:
  uint16_t start_time;
  uint16_t duration;
  
  void print();
  bool active(const time_t& now);

  void setStartTime(const uint8_t& hour, const uint8_t& minute);
  void setStartTime(const uint16_t& minute_of_day);
  void setStartTime(const time_t& time);
};

class Counter {
public:
  uint32_t start_count;
  uint32_t count;

  void print();
  bool active(const volatile unsigned long& current);
};

class DigitalSwitch : public Switch {
public:
  void print();
  bool active();
  bool getValue();
};

class AnalogSwitch : public Switch {
public:
  uint16_t threshold_low = 0;
  uint16_t threshold_high = 1024;

  void print();
  bool active();
  int getValue();
};

class Channel {
public:
  bool enabled;
  bool active;
  bool was_active;
  byte pin_out;

  Timer time;
  Counter flow;
  DigitalSwitch movement;
  AnalogSwitch moisture;
  AnalogSwitch rain;
  AnalogSwitch brightness;

  bool isEnabled() { return enabled; }
};


class FileConfig
{
  public:
    static bool toBool(const String& value);
    static String fromBool(const bool& value);

    static unsigned int toMinutes(const String& value);
    static String fromMinutes(const unsigned int& value);
    static String fromTime(const unsigned int& hour, const unsigned int& minute);

    static bool readConfig(File configFile, Channel* channel, const byte& channelSize);
    static bool parseConfig(const String& key, const String& value, Channel& channel);
  private:

};

#endif // _FILECONFIG_H_
