// =============================================================================
//
// Menu callback functions
//
// =============================================================================


// *****************************************************************************
// Global variables
// *****************************************************************************

unsigned long timer = 0;

File dataFile;


// *****************************************************************************
// Helper
// *****************************************************************************
void progressBar(const byte& percent, const byte& row = 0) {
  byte pos = constrain(percent, 0, 100);
  pos = map(pos, 0, 100, 1, 14);

  char bar[20];
  sprintf(bar, "[             ]%3d%%", percent);
  for(byte i = 1; i < pos; i++) {
    bar[i] = '=';
  }

  u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * row), bar);
  u8g2.drawFrame( LCDML_DISP_BOX_X0, LCDML_DISP_BOX_Y0, (LCDML_DISP_BOX_X1 - LCDML_DISP_BOX_X0), (LCDML_DISP_BOX_Y1 - LCDML_DISP_BOX_Y0) );
}


// *****************************************************************************
// Set clock
// *****************************************************************************
void mFunc_set_clock(uint8_t param) {
  static bool blink = true;
  static bool modify = false;
  static uint8_t col_count = 0;
  static uint8_t row_count = 0;
  static TimeElements dateTime;

  if (LCDML.FUNC_setup())     // ****** Setup *********
  {
    LCDML.FUNC_disableScreensaver();
    LCDML.TIMER_msReset(timer);
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
    breakTime(now(), dateTime);
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    if (LCDML.TIMER_ms(timer, 500)) {
      blink = !blink;
      timer = millis();
    }

    if (LCDML.BT_checkAny()) {
      if(LCDML.BT_checkEnter()) {
        modify = !modify;
        LCDML.BT_resetEnter();
      }

      if (LCDML.BT_checkUp()) {
        if (!modify) {
          if (col_count <= 3) {
            col_count += 3;
          }
          else {
            col_count = 0;
            row_count = (row_count == 0) ? 1 : 0;
          }
        }
        else  {
          if (row_count == 0) {
            switch(col_count) {
              case 0:
              dateTime.Day   = (dateTime.Day   < 31) ? (dateTime.Day   + 1 ) : (dateTime.Day   - 31);
              break;
              case 3:
              dateTime.Month = (dateTime.Month < 12) ? (dateTime.Month + 1 ) : (dateTime.Month - 12);
              break;
              case 6:
              dateTime.Year = (dateTime.Year < 99) ? (dateTime.Year + 1 ) : (dateTime.Year - 99);
              break;
              default:
              ;
            }
          }
          else {
            switch (col_count) {
              case 0:
              dateTime.Hour   = (dateTime.Hour   < 23) ? (dateTime.Hour   + 1 ) : (dateTime.Hour   - 23);
              break;
              case 3:
              dateTime.Minute = (dateTime.Minute < 59) ? (dateTime.Minute + 1 ) : (dateTime.Minute - 59);
              break;
              case 6:
              dateTime.Second = (dateTime.Second < 59) ? (dateTime.Second + 1 ) : (dateTime.Second - 59);
              break;
              default:
              ;
            }
          }
        }
        LCDML.BT_resetUp();
      }

      if (LCDML.BT_checkDown()) {
        if (!modify) {
          if (col_count >= 3) {
            col_count -= 3;
          }
          else {
            col_count = 6;
            row_count = (row_count == 0) ? 1 : 0;
          }
        }
        else {
          if (row_count == 0) {
            switch(col_count) {
              case 0:
              dateTime.Day   = (dateTime.Day   >= 1 ) ? (dateTime.Day   - 1 ) : (dateTime.Day   + 31);
              break;
              case 3:
              dateTime.Month = (dateTime.Month >= 1 ) ? (dateTime.Month - 1 ) : (dateTime.Month + 12);
              break;
              case 6:
              dateTime.Year = (dateTime.Year >= 1 ) ? (dateTime.Year - 1 ) : (dateTime.Year + 99);
              break;
              default:
              ;
            }
          }
          else {
            switch (col_count) {
              case 0:
              dateTime.Hour   = (dateTime.Hour   >= 1 ) ? (dateTime.Hour   - 1 ) : (dateTime.Hour   + 23);
              break;
              case 3:
              dateTime.Minute = (dateTime.Minute >= 1 ) ? (dateTime.Minute - 1 ) : (dateTime.Minute + 59);
              break;
              case 6:
              dateTime.Second = (dateTime.Second >= 1 ) ? (dateTime.Second - 1 ) : (dateTime.Second + 59);
              break;
              default:
              ;
            }
          }
        }
        LCDML.BT_resetDown();
      }

      if (LCDML.BT_checkLeft()) {
        if (col_count >= 3) {
          col_count -= 3;
        }
        else {
          col_count = 6;
          row_count = (row_count == 0) ? 1 : 0;
        }
        LCDML.BT_resetLeft();
      }

      if (LCDML.BT_checkRight()) {
        if (col_count <= 3) {
          col_count += 3;
        }
        else {
          col_count = 0;
          row_count = (row_count == 0) ? 1 : 0;
        }
        LCDML.BT_resetRight();
      }
    }

    char bufDate[17];
    sprintf (bufDate, "Date: %02d.%02d.%04d", dateTime.Day, dateTime.Month, dateTime.Year + 1970);

    char bufTime[15];
    sprintf (bufTime, "Time: %02d:%02d:%02d", dateTime.Hour, dateTime.Minute, dateTime.Second);

    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), bufDate);
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 2), bufTime);

      if(!modify || blink) {
        if ((row_count == 0) && (col_count == 6)) {
          u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 6), (LCDML_DISP_FONT_H * row_count) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1 );
        }
        else {
          u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + LCDML_DISP_FONT_W * (col_count + 6), (LCDML_DISP_FONT_H * row_count) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 2), LCDML_DISP_FONT_H - 1 );
        }
      }
      u8g2.drawFrame( LCDML_DISP_BOX_X0, LCDML_DISP_BOX_Y0, (LCDML_DISP_BOX_X1 - LCDML_DISP_BOX_X0), (LCDML_DISP_BOX_Y1 - LCDML_DISP_BOX_Y0) );
    } while( u8g2.nextPage() );
  }

  if(LCDML.FUNC_close())      // ****** END *********
  {
    RTC.set(makeTime(dateTime));
    setTime(makeTime(dateTime));
  }
}


// *****************************************************************************
// Read config from SD
// *****************************************************************************
void mFunc_readSD(uint8_t param) {
  static byte index = 0;

  if (LCDML.FUNC_setup())     // ****** Setup *********
  {
    Serial.print(F("Parsing config file"));
    LCDML.FUNC_disableScreensaver();
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds

    index = 0;
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    String sdStatus(F("Unknown Error"));

    if(!sdReady) {
      sdReady = SD.begin(SD_CS_PIN);
      sdStatus = F("Waiting for SD");
    }
    else {
      if(index < NUM_CHANNEL) {
        if(parseConfig(index)) {
          Serial.print('.');
          sdStatus = F("Reading from SD");
          index++;
        }
        else{
          sdStatus = F("Could not read SD");
        }
      }
      else {
        Serial.println(F("Done"));
        LCDML.MENU_goRoot();
      }
    }

    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), sdStatus.c_str());
      progressBar((index * 100) / NUM_CHANNEL, 2);
    } while( u8g2.nextPage() );
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    setupIOs();
  }
}


// *****************************************************************************
// Write config to SD
// *****************************************************************************
void mFunc_writeSD(uint8_t param) {
  static byte index = 0;

  if (LCDML.FUNC_setup())     // ****** Setup *********
  {
    LCDML.FUNC_disableScreensaver();
    LCDML.TIMER_msReset(timer);
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds

    index = 0;
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    String sdStatus(F("Unknown Error"));

    if(!sdReady) {
      sdReady = SD.begin(SD_CS_PIN);
      sdStatus = F("Waiting for SD");
    }
    else {
      if (!dataFile) {
        sdStatus = F("Waiting for SD");
        dataFile = SD.open(FILE_NAME, (O_WRITE | O_CREAT | O_TRUNC));
      }
      else {
        sdStatus = F("Saving to SD");
        if (LCDML.TIMER_ms(timer, 1000)) {
          if(index < NUM_CHANNEL) {
            String header(F("Channel"));
            header.concat(index);
            channel[index].print(Serial, header);
            channel[index].print(dataFile, header);
            index++;
          }
          else {
            LCDML.MENU_goRoot();
          }
          timer = millis();
        }
      }
    }

    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), sdStatus.c_str());
      progressBar((index * 100) / NUM_CHANNEL, 2);
    } while( u8g2.nextPage() );
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    dataFile.close();
  }
}


// *****************************************************************************
// Read config from EEPROM
// *****************************************************************************
void mFunc_readEEPROM(uint8_t param) {
  static byte index = 0;

  if (LCDML.FUNC_setup())     // ****** Setup *********
  {
    Serial.print(F("Read config"));
    LCDML.FUNC_disableScreensaver();
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds

    index = 0;
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    String eepromStatus(F("Read from EEPROM"));
    
    if(index < NUM_CHANNEL) {
      EEPROM.get(index * sizeof(Channel), channel[index]);
      Serial.print('.');
      index++;
    }
    else {
      Serial.println(F("Done"));
      LCDML.MENU_goRoot();
    }

    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), eepromStatus.c_str());
      progressBar((index * 100) / NUM_CHANNEL, 2);
    } while( u8g2.nextPage() );
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    setupIOs();
  }
}


// *****************************************************************************
// Write config to EEPROM
// *****************************************************************************
void mFunc_writeEEPROM(uint8_t param) {
  static byte index = 0;

  if (LCDML.FUNC_setup())     // ****** Setup *********
  {
    LCDML.FUNC_disableScreensaver();
    LCDML.TIMER_msReset(timer);
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds

    index = 0;
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    String eepromStatus(F("Write to EEPROM"));

    if (LCDML.TIMER_ms(timer, 1000)) {
      if(index < NUM_CHANNEL) {
        String header(F("Channel"));
        header.concat(index);
        channel[index].print(Serial, header);
        EEPROM.put(index*sizeof(Channel), channel[index]);
        index++;
      }
      else {
        LCDML.MENU_goRoot();
      }
      timer = millis();
    }

    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), eepromStatus.c_str());
      progressBar((index * 100) / NUM_CHANNEL, 2);
    } while( u8g2.nextPage() );
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    dataFile.close();
  }
}


// *****************************************************************************
// Homescreen
// *****************************************************************************
void mFunc_home(uint8_t param) {
  // Setup
  if(LCDML.FUNC_setup()) {
    click_count = 0;
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  }

  // Loop
  if(LCDML.FUNC_loop()) {
    time_t t = now();
    byte dateOffset = 0;
    byte timeOffset = 0;
    char date_buf[14];
    char time_buf[14];

    // Display date and time on homescreen
    if (timeStatus() == timeSet) {

      sprintf (date_buf, "%02d.%02d.%d", day(t), month(t), year(t));
      sprintf (time_buf, "%02d:%02d:%02d", hour(t), minute(t), second(t));

      dateOffset = 34;
      timeOffset = 40;
    }
    else {
      sprintf (date_buf, "%s", "Date not set!");
      sprintf (time_buf, "%s", "Time not set!");

      dateOffset = 25;
      timeOffset = 25;
    }

  // Display current flow
   String flowString(F("Liter : "));
   flowString.concat(flowCounter / FLOW_CONV);

   // Display current execution on homescreen
   String activeString(F("Active:"));
   for(byte i = 0; i < NUM_CHANNEL; i++) {
     if(channel[i].active && !channel[i].skip) {
       activeString.concat(" Ch");
	   activeString.concat(i);
     }
   }

    // Display next execution on homescreen
    byte nextChannel = -1;
    time_t nextExec = -1;
    for(byte i = 0; i < NUM_CHANNEL; i++) {
      time_t currentExec = channel[i].time.getNextStartTime(now());
      if(currentExec < elapsedSecsToday(now())) {
        currentExec += SECS_PER_DAY;
      }

      if(channel[i].skip) {
        currentExec += SECS_PER_DAY;
      }

      if(currentExec < nextExec) {
        nextChannel = i;
        nextExec = currentExec;
      }
    }

    String nextString(F("Next  : Ch"));
    nextString.concat(nextChannel);
    nextString.concat(' ');
    nextString.concat(fromTime(channel[nextChannel].time.start_time));

    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr(dateOffset, (LCDML_DISP_FONT_H * 1), date_buf);
      u8g2.drawStr(timeOffset, (LCDML_DISP_FONT_H * 2), time_buf);
      u8g2.drawStr( 0, (LCDML_DISP_FONT_H * 3), flowString.c_str());
      u8g2.drawStr( 0, (LCDML_DISP_FONT_H * 4), activeString.c_str());
      u8g2.drawStr( 0, (LCDML_DISP_FONT_H * 5), nextString.c_str());
    } while( u8g2.nextPage() );

    // Leave homescreen if any button is pressed
    if (LCDML.BT_checkAny()) {
      // Leave function
      LCDML.FUNC_goBackToMenu();
      LCDML.BT_resetAll();
    }
  }

  // Close
  if(LCDML.FUNC_close()) {
    // Go to the root menu
    LCDML.MENU_goRoot();
  }
}


// *****************************************************************************
// Back
// *****************************************************************************
void mFunc_back(uint8_t param) {
  if(LCDML.FUNC_setup()) {
    LCDML.FUNC_goBackToMenu(1);      // leave this function and go a layer back
  }
}
