#include "FileConfig.h"

bool Switch::isEnabled() {
  return enabled;
}

void Switch::enable(const bool& value) {
  enabled = value;
}

bool Switch::isInverted() {
  return inverted;
}

void Switch::invert(const bool& value) {
  inverted = value;
}

byte Switch::getPin() {
  return input;
}

void Switch::setPin(const byte& pin) {
  input = pin;
}

void Switch::print() {
  Serial.print(F("  Enabled = "));
  Serial.println(FileConfig::fromBool(isEnabled()));
  Serial.print(F("  Invert = "));
  Serial.println(FileConfig::fromBool(isInverted()));
  Serial.print(F("  Input = "));
  Serial.println(input);
}



void Timer::print() {
  Serial.println(F("[Timer]"));
  Serial.print(F("  Start = "));
  Serial.println(FileConfig::fromMinutes(start_time));
  Serial.print(F("  Duration = "));
  Serial.println(FileConfig::fromMinutes(duration));
}

bool Timer::active(const time_t& now) {
  uint16_t now_minutes = hour(now) * 60 + minute(now);
  return (now_minutes >= start_time) && ((now_minutes < start_time + duration) || (duration == 0));
}

void Timer::setStartTime(const uint8_t& hour, const uint8_t& minute) {
  start_time = hour * 60 + minute;
}

void Timer::setStartTime(const uint16_t& minute_of_day) {
  start_time = minute_of_day;
}

void Timer::setStartTime(const time_t& time) {
  setStartTime(hour(time), minute(time));
}



void Counter::print() {
  Serial.println(F("[Counter]"));
  Serial.print(F("  StartCount = "));
  Serial.println(start_count);
  Serial.print(F("  Count = "));
  Serial.println(count);
}

bool Counter::active(const volatile unsigned long& current) {
  return (current - start_count) < count;
}



void DigitalSwitch::print() {
  Serial.println(F("[DigitalSwitch]"));
  Switch::print();
  Serial.print(F("  Value = "));
  Serial.println(FileConfig::fromBool(getValue()));
}

bool DigitalSwitch::active() {
  return getValue() ^ isInverted();
}

bool DigitalSwitch::getValue() {
  return digitalRead(getPin());
}



void AnalogSwitch::print() {
  Serial.println(F("[AnalogSwitch]"));
  Switch::print();
  Serial.print(F("  Value = "));
  Serial.println(getValue());
  Serial.print(F("  Threshold_Low = "));
  Serial.println(threshold_low);
  Serial.print(F("  Threshold_High = "));
  Serial.println(threshold_high);
}

bool AnalogSwitch::active() {
  int value = getValue();

  if(value > threshold_low && value < threshold_high) {
    return !isInverted();
  }
  return isInverted();
}

int AnalogSwitch::getValue() {
  return analogRead(getPin());
}


bool FileConfig::toBool(const String& value) {
  return value.equalsIgnoreCase(F("true")) ? true : false;
}

String FileConfig::fromBool(const bool& value) {
  return value == true ? F("true") : F("false");
}

unsigned int FileConfig::toMinutes(const String& value) {
  int i = value.indexOf(':');
  if (i != -1) {
    // Duration in HH:MM format
    return value.substring(0, i).toInt() * 60 + value.substring(i+1, i+3).toInt();
  }
  else {
    // Duration in MMM format
    return value.toInt();
  }
}

String FileConfig::fromMinutes(const unsigned int& value) {
  unsigned int hour = value / 60;
  unsigned int minute = value % 60;

  return fromTime(hour, minute);
}

String FileConfig::fromTime(const unsigned int& hour, const unsigned int& minute) {
  String out;

  if (hour < 10) {
    out += '0';
  }

  out += hour;
  out += ':';

  if (minute < 10) {
    out += '0';
  }

  out += minute;

  return out;
}

bool FileConfig::readConfig(File configFile, Channel* channel, const byte& channelSize){
  if (configFile) {
    byte channelIndex = 255; // Remember which channel is currently processed

    while (configFile.available()) {
      String linebuffer;

      char symbol = configFile.read();
      while (symbol != '\n') {
        if (symbol == ' ' || symbol == '\t') {
          // Whitespace, do nothing
        }
        else if (symbol == '#') {
          // Comment, proceed to EOL
          configFile.readStringUntil('\n');
        }
        else if (symbol == '[') {
          // Header, extract channel index
          channelIndex = configFile.parseInt();
        }
        else {
          // Regular character, add to linebuffer
          linebuffer += symbol;
        }

        // Read next character
        symbol = configFile.read();
      }

      // Evaluate linebuffer
      if (linebuffer.length() != 0) {
        // Check if line contains =
        int index = linebuffer.indexOf('=');
        if (index >= 0) {
          String key, value;
          key = linebuffer.substring(0, index);
          value = linebuffer.substring(index + 1, linebuffer.length());
          if (channelIndex < channelSize) {
            if (!parseConfig(key, value, channel[channelIndex])) {
              return false;
            }
          }
          else {
            Serial.println(F("Channel index out of bounds"));
            return false;
          }
        }
      }
    }
  }
  else {
    return false;
  }

  return true;
}

bool FileConfig::parseConfig(const String& key, const String& value, Channel& channel){
  if (key.equalsIgnoreCase(F("Enabled"))) {
    channel.enabled = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("TimeStart"))) {
    channel.time.start_time = FileConfig::toMinutes(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("TimeDuration"))) {
    channel.time.duration = FileConfig::toMinutes(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("FlowCount"))) {
    channel.flow.count = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("MovementEnabled"))) {
    channel.movement.enabled = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("MovementInvert"))) {
    channel.movement.inverted = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("MovementPin"))) {
    channel.movement.input = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("MoistureEnabled"))) {
    channel.moisture.enabled = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("MoistureInvert"))) {
    channel.moisture.inverted = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("MoisturePin"))) {
    channel.moisture.input = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("MoistureThresholdLow"))) {
    channel.moisture.threshold_low = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("MoistureThresholdHigh"))) {
    channel.moisture.threshold_high = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("RainEnabled"))) {
    channel.rain.enabled = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("RainInvert"))) {
    channel.rain.inverted = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("RainPin"))) {
    channel.rain.input = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("RainThresholdLow"))) {
    channel.rain.threshold_low = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("RainThresholdHigh"))) {
    channel.rain.threshold_high = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("BrightnessEnabled"))) {
    channel.brightness.enabled = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("BrightnessInvert"))) {
    channel.brightness.inverted = FileConfig::toBool(value);
    return true;
  }
  else if (key.equalsIgnoreCase(F("BrightnessPin"))) {
    channel.brightness.input = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("BrightnessThresholdLow"))) {
    channel.brightness.threshold_low = value.toInt();
    return true;
  }
  else if (key.equalsIgnoreCase(F("BrightnessThresholdHigh"))) {
    channel.brightness.threshold_high = value.toInt();
    return true;
  }

  Serial.print(F("Unknown key "));
  Serial.println(key);
  return false;
}
