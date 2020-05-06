// =============================================================================
//
// Dynamic content
//
// =============================================================================


// *****************************************************************************
// Global variables
// *****************************************************************************

// Current channel
byte current = 0;


// *****************************************************************************
// Helper
// *****************************************************************************
uint8_t isSelected(const uint8_t& cursorPos, const uint8_t& count = 1) {
   // Check if this function is active (cursor is on this line)
  if (cursorPos == LCDML.MENU_getCursorPos()) {
    if (LCDML.BT_checkAny()) {
      if (LCDML.BT_checkEnter()) {
        click_count = (click_count < count) ? click_count + 1 : 0;

        if (count == 0) {
          LCDML.BT_resetEnter();
          return 1;
        }
        else if (LCDML.MENU_getScrollDisableStatus() == 0) {
          // This function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
          LCDML.FUNC_disableScreensaver();
        }
        else if(click_count == 0) {
          // Enable the normal menu scroll function
          LCDML.MENU_enScroll();
          //LCDML.FUNC_enableScreensaver();
        }
        LCDML.BT_resetEnter();
      }
    }
    return click_count;
  }

  return 0;
}


// *****************************************************************************
// Select channel
// *****************************************************************************
void mDyn_ch_select(uint8_t line) {

  // Check if this function is active (cursor is on this line)
  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      current = (current < (NUM_CHANNEL - 1)) ? current + 1 : 0;
      LCDML.BT_resetUp();
    }
    if (LCDML.BT_checkDown()) {
      current = (current > 0) ? current - 1 : (NUM_CHANNEL - 1);
      LCDML.BT_resetDown();
    }
  }

  String str(F("Channel "));
  str += current;
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 8, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 1), LCDML_DISP_FONT_H - 1);
  }
}


// *****************************************************************************
// Enable channel
// *****************************************************************************
void mDyn_ch_enable(uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    if (channel[current].enabled && !channel[current].skip) {
      channel[current].skip = true;
    }
    else {
      channel[current].enabled = !channel[current].enabled;
      channel[current].skip = false;
    }
    dirty_eeprom = true;
    dirty_sdcard = true;
    LCDML.BT_resetAll();
  }

  String str(F("Enabled :"));
  if (channel[current].skip) {
    str += F("Skip");
  }
  else {
    str += (channel[current].enabled) ? F("True") : F("False");
  }

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 10, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}


// *****************************************************************************
// Skip next execution
// *****************************************************************************
void mDyn_ch_skip(uint8_t line) {
  String elements;
  byte skipcount = 0;
  for (byte i = 0; i < NUM_CHANNEL; i++) {
    if (channel[i].skip) {
      skipcount++;
      elements += i;
      elements += ' ';
    }
  }

  // Skip next execution cycle
  if (isSelected(line, 0)) {
    if (skipcount == NUM_CHANNEL) {
      skipcount = 0;
      for (byte i = 0; i < NUM_CHANNEL; i++) {
        channel[i].skip = false;
      }
    }
    else {
      skipcount = NUM_CHANNEL;
      for (byte i = 0; i < NUM_CHANNEL; i++) {
        channel[i].skip = true;
      }
    }
    LCDML.BT_resetAll();
  }

  String str;
  if (skipcount == 0) {
    str = "Skip:None";
  }
  else if (skipcount == NUM_CHANNEL) {
    str = "Skip:All";
  }
  else {
    str = "Skip:[ " + elements + "]";
  }

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
}


// *****************************************************************************
// Set start time
// *****************************************************************************
void mDyn_ch_start(uint8_t line) {
  static uint8_t col_count = 0;
  TimeElements dateTime;
  breakTime( channel[current].time.getStartTime(), dateTime);
//  dateTime.Hour = channel[current].time.getStartTime() / 3600;
//  dateTime.Minute = channel[current].time.getStartTime() % 3600 / 60;

  if (isSelected(line, 3)) {
    col_count = (isSelected(line, 3) * 3) - 3;

    if (LCDML.BT_checkUp()) {
      switch (isSelected(line, 3)) {
        case 1:
          dateTime.Hour   = (dateTime.Hour   < 23) ? (dateTime.Hour   + 1 ) : (dateTime.Hour   - 23);
          break;
        case 2:
          dateTime.Minute = (dateTime.Minute < 59) ? (dateTime.Minute + 1 ) : (dateTime.Minute - 59);
          break;
        case 3:
          dateTime.Second = (dateTime.Second < 59) ? (dateTime.Second + 1 ) : (dateTime.Second - 59);
          break;
        default:
          ;
      }
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      switch (isSelected(line, 3)) {
        case 1:
          dateTime.Hour   = (dateTime.Hour   >= 1 ) ? (dateTime.Hour   - 1 ) : (dateTime.Hour   + 23);
          break;
        case 2:
          dateTime.Minute = (dateTime.Minute >= 1 ) ? (dateTime.Minute - 1 ) : (dateTime.Minute + 59);
          break;
        case 3:
          dateTime.Second = (dateTime.Second >= 1 ) ? (dateTime.Second - 1 ) : (dateTime.Second + 59);
          break;
        default:
          ;
      }
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[18];
  sprintf (buf, "Start   :%02d:%02d:%02d", dateTime.Hour, dateTime.Minute, dateTime.Second);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line, 3)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 10), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 2), LCDML_DISP_FONT_H - 1);
  }
  channel[current].time.setStartTime(dateTime.Hour, dateTime.Minute, dateTime.Second);
}


// *****************************************************************************
// Set duration
// *****************************************************************************
void mDyn_ch_duration(uint8_t line) {
  static uint8_t col_count = 0;
  uint32_t duration = channel[current].time.duration / 60;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      duration = (duration < 999) ? duration + 1 : 999;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      duration = (duration > 0) ? duration - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[17];
  sprintf (buf, "Duration:%03d min.", duration);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 10), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 3), LCDML_DISP_FONT_H - 1);
  }
  channel[current].time.duration = duration * 60;
}


// *****************************************************************************
// Set repeat interval
// *****************************************************************************
void mDyn_ch_repeat(uint8_t line) {
  static uint8_t col_count = 0;
  uint32_t repeat = channel[current].time.repeat / 3600;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      repeat = (repeat < 23) ? repeat + 1 : 23;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      repeat = (repeat > 0) ? repeat - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[15];
  sprintf (buf, "Every   :%02d h", repeat);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 10), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 3), LCDML_DISP_FONT_H - 1);
  }
  channel[current].time.repeat = repeat * 3600;
}

// *****************************************************************************
// Set quantity
// *****************************************************************************
void mDyn_ch_flow (uint8_t line) {
  static uint8_t col_count = 0;
  uint32_t count = channel[current].flow.count;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      count = (count < 9990) ? count + 10 : 9999;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      count = (count > 9) ? count - 10 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[20];
  sprintf (buf, "Quantity:%04d lit.", count);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 10), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
  }
  channel[current].flow.count = count;
}

// *****************************************************************************
// Moisture
// *****************************************************************************
void mDyn_moist_enable (uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    channel[current].moisture.enabled = !channel[current].moisture.enabled;
    markDirty(true, true);
    LCDML.BT_resetAll();
  }

  String str(F("Enabled :"));
  str += (channel[current].moisture.enabled) ? F("True") : F("False");
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 9, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}

void mDyn_moist_invert (uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    channel[current].moisture.inverted = !channel[current].moisture.inverted;
    markDirty(true, true);
    LCDML.BT_resetAll();
  }

  String str(F("Inverted:"));
  str += (channel[current].moisture.inverted) ? F("True") : F("False");
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 9, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}

void mDyn_moist_low (uint8_t line) {
  static uint8_t col_count = 0;
  uint16_t threshold = channel[current].moisture.threshold_low;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      threshold = (threshold < 1024) ? threshold + 1 : 1024;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      threshold = (threshold > 0) ? threshold - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[13];
  sprintf (buf, "Low     :%04d", threshold);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 9), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
  }
  channel[current].moisture.threshold_low = threshold;
}

void mDyn_moist_high (uint8_t line) {
  static uint8_t col_count = 0;
  uint16_t threshold = channel[current].moisture.threshold_high;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      threshold = (threshold < 1024) ? threshold + 1 : 1024;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      threshold = (threshold > 0) ? threshold - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[13];
  sprintf (buf, "High    :%04d", threshold);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 9), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
  }
  channel[current].moisture.threshold_high = threshold;
}


// *****************************************************************************
// Rain
// *****************************************************************************
void mDyn_rain_enable (uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    channel[current].rain.enabled = !channel[current].rain.enabled;
    markDirty(true, true);
    LCDML.BT_resetAll();
  }

  String str(F("Enabled :"));
  str += (channel[current].rain.enabled) ? F("True") : F("False");
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 9, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}

void mDyn_rain_invert (uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    channel[current].rain.inverted = !channel[current].rain.inverted;
    markDirty(true, true);
    LCDML.BT_resetAll();
  }

  String str(F("Inverted:"));
  str += (channel[current].rain.inverted) ? F("True") : F("False");
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 9, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}

void mDyn_rain_low (uint8_t line) {
  static uint8_t col_count = 0;
  uint16_t threshold = channel[current].rain.threshold_low;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      threshold = (threshold < 1024) ? threshold + 1 : 1024;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      threshold = (threshold > 0) ? threshold - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[13];
  sprintf (buf, "Low     :%04d", threshold);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 9), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
  }
  channel[current].rain.threshold_low = threshold;
}

void mDyn_rain_high (uint8_t line) {
  static uint8_t col_count = 0;
  uint16_t threshold = channel[current].rain.threshold_high;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      threshold = (threshold < 1024) ? threshold + 1 : 1024;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      threshold = (threshold > 0) ? threshold - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[13];
  sprintf (buf, "High    :%04d", threshold);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 9), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
  }
  channel[current].rain.threshold_high = threshold;
}


// *****************************************************************************
// Brightness
// *****************************************************************************
void mDyn_bright_enable (uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    channel[current].brightness.enabled = !channel[current].brightness.enabled;
    markDirty(true, true);
    LCDML.BT_resetAll();
  }

  String str(F("Enabled :"));
  str += (channel[current].brightness.enabled) ? F("True") : F("False");
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 9, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}

void mDyn_bright_invert (uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    channel[current].brightness.inverted = !channel[current].brightness.inverted;
    markDirty(true, true);
    LCDML.BT_resetAll();
  }

  String str(F("Inverted:"));
  str += (channel[current].brightness.inverted) ? F("True") : F("False");
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 9, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}

void mDyn_bright_low (uint8_t line) {
  static uint8_t col_count = 0;
  uint16_t threshold = channel[current].brightness.threshold_low;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      threshold = (threshold < 1024) ? threshold + 1 : 1024;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      threshold = (threshold > 0) ? threshold - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[13];
  sprintf (buf, "Low     :%04d", threshold);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 9), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
  }
  channel[current].brightness.threshold_low = threshold;
}

void mDyn_bright_high (uint8_t line) {
  static uint8_t col_count = 0;
  uint16_t threshold = channel[current].brightness.threshold_high;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      threshold = (threshold < 1024) ? threshold + 1 : 1024;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      threshold = (threshold > 0) ? threshold - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[13];
  sprintf (buf, "High    :%04d", threshold);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 9), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
  }
  channel[current].brightness.threshold_high = threshold;
}


// *****************************************************************************
// Movement
// *****************************************************************************
void mDyn_move_enable (uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    channel[current].movement.enabled = !channel[current].movement.enabled;
    markDirty(true, true);
    LCDML.BT_resetAll();
  }

  String str(F("Enabled :"));
  str += (channel[current].movement.enabled) ? F("True") : F("False");
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 9, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}

void mDyn_move_invert (uint8_t line) {
  // Check if this function is active (cursor is on this line)
  if (isSelected(line) && LCDML.BT_checkAny()) {
    channel[current].movement.inverted = !channel[current].movement.inverted;
    markDirty(true, true);
    LCDML.BT_resetAll();
  }

  String str(F("Inverted:"));
  str += (channel[current].movement.inverted) ? F("True") : F("False");
  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str() );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 9, (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 5), LCDML_DISP_FONT_H - 1);
  }
}

void mDyn_move_wait (uint8_t line) {
  static uint8_t col_count = 0;
  uint32_t duration = channel[current].movement.delay;

  if (isSelected(line)) {
    if (LCDML.BT_checkUp()) {
      duration = (duration < 999) ? duration + 1 : 999;
      markDirty(true, true);
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      duration = (duration > 0) ? duration - 1 : 0;
      markDirty(true, true);
      LCDML.BT_resetDown();
    }
  }

  char buf[13];
  sprintf (buf, "Duration:%04d", duration);

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * 0,  (LCDML_DISP_FONT_H * (1 + line)), buf );
  if(isSelected(line)) {
    u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 9), (LCDML_DISP_FONT_H * (line)) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
  }
  channel[current].movement.delay = duration;
}
