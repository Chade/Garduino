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

void timeMenu(TimeElements& tm, unsigned long& tmr) {
  static bool blink = true;
  static bool modify = false;
  static uint8_t col_count = 0;

  if (LCDML.TIMER_ms(tmr, 500)) {
    blink = !blink;
    tmr = millis();
  }

  if (LCDML.BT_checkAny()) {
    if(LCDML.BT_checkEnter()) {
      modify = !modify;
      LCDML.BT_resetEnter();
    }

    if (LCDML.BT_checkUp()) {
      if (!modify) {
        col_count = (col_count <= 3) ? col_count + 3 : 0;
      }
      else  {
        switch (col_count) {
          case 0:
            tm.Hour   = (tm.Hour   < 23) ? (tm.Hour   + 1 ) : (tm.Hour   - 23);
            break;
          case 3:
            tm.Minute = (tm.Minute < 59) ? (tm.Minute + 1 ) : (tm.Minute - 59);
            break;
          case 6:
            tm.Second = (tm.Second < 59) ? (tm.Second + 1 ) : (tm.Second - 59);
            break;
          default:
            ;
        }
      }
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      if (!modify) {
        col_count = (col_count >= 3) ? col_count - 3 : 6;
      }
      else {
        switch (col_count) {
          case 0:
            tm.Hour   = (tm.Hour   >= 1 ) ? (tm.Hour   - 1 ) : (tm.Hour   + 23);
            break;
          case 3:
            tm.Minute = (tm.Minute >= 1 ) ? (tm.Minute - 1 ) : (tm.Minute + 59);
            break;
          case 6:
            tm.Second = (tm.Second >= 1 ) ? (tm.Second - 1 ) : (tm.Second + 59);
            break;
          default:
            ;
        }
      }
      LCDML.BT_resetDown();
    }

    if (LCDML.BT_checkLeft()) {
      col_count = (col_count >= 3) ? col_count - 3 : 6;
      LCDML.BT_resetLeft();
    }

    if (LCDML.BT_checkRight()) {
      col_count = (col_count <= 3) ? col_count + 3 : 0;
      LCDML.BT_resetRight();
    }
  }
  
  char buf[9];
  sprintf (buf, "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);

  u8g2.setFont(LCDML_DISP_FONT);
  u8g2.firstPage();
  do {
    u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), "Set Time:");
    u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 2), buf);

    if(!modify || blink) {
      u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + (LCDML_DISP_FONT_W * col_count), (LCDML_DISP_FONT_H * 1) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 2), LCDML_DISP_FONT_H - 1);
    }
    u8g2.drawFrame( LCDML_DISP_BOX_X0, LCDML_DISP_BOX_Y0, (LCDML_DISP_BOX_X1 - LCDML_DISP_BOX_X0), (LCDML_DISP_BOX_Y1 - LCDML_DISP_BOX_Y0) );
  } while( u8g2.nextPage() );
}


void dateMenu(TimeElements& tm, unsigned long& tmr) {
  static bool blink = true;
  static bool modify = false;
  static uint8_t col_count = 0;

  if (LCDML.TIMER_ms(tmr, 500)) {
    blink = !blink;
    tmr = millis();
  }

  if (LCDML.BT_checkAny()) {
    if(LCDML.BT_checkEnter()) {
      modify = !modify;
      LCDML.BT_resetEnter();
    }

    if (LCDML.BT_checkUp()) {
      if (!modify) {
        col_count = (col_count <= 3) ? col_count + 3 : 0;
      }
      else  {
        switch(col_count) {
          case 0:
            tm.Day   = (tm.Day   < 31) ? (tm.Day   + 1 ) : (tm.Day   - 31);
            break;
          case 3:
            tm.Month = (tm.Month < 12) ? (tm.Month + 1 ) : (tm.Month - 12);
            break;
          case 6:
            tm.Year = (tm.Year < 99) ? (tm.Year + 1 ) : (tm.Year - 99);
            break;
          default:
            ;
        }
      }
      LCDML.BT_resetUp();
    }

    if (LCDML.BT_checkDown()) {
      if (!modify) {
        col_count = (col_count >= 3) ? col_count - 3 : 6;
      }
      else {
         switch(col_count) {
          case 0:
            tm.Day   = (tm.Day   >= 1 ) ? (tm.Day   - 1 ) : (tm.Day   + 31);
            break;
          case 3:
            tm.Month = (tm.Month >= 1 ) ? (tm.Month - 1 ) : (tm.Month + 12);
            break;
          case 6:
            tm.Year = (tm.Year >= 1 ) ? (tm.Year - 1 ) : (tm.Year + 99);
            break;
          default:
            ;
        }
      }
      LCDML.BT_resetDown();
    }

    if (LCDML.BT_checkLeft()) {
      col_count = (col_count >= 3) ? col_count - 3 : 6;
      LCDML.BT_resetLeft();
    }

    if (LCDML.BT_checkRight()) {
      col_count = (col_count <= 3) ? col_count + 3 : 0;
      LCDML.BT_resetRight();
    }
  }

  char buf[11];
  sprintf (buf, "%02d.%02d.%04d", tm.Day, tm.Month, tm.Year + 1970);

  u8g2.setFont(LCDML_DISP_FONT);
  u8g2.firstPage();
  do {
    u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), "Set Date:");
    u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 2), buf);

    if(!modify || blink) {
      if (col_count == 6) {
        u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + (LCDML_DISP_FONT_W * col_count), (LCDML_DISP_FONT_H * 1) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4), LCDML_DISP_FONT_H - 1);
      }
      else {
        u8g2.drawFrame( LCDML_DISP_FRAME_OFFSET + (LCDML_DISP_FONT_W * col_count), (LCDML_DISP_FONT_H * 1) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 2), LCDML_DISP_FONT_H - 1);
      }
    }
    u8g2.drawFrame( LCDML_DISP_BOX_X0, LCDML_DISP_BOX_Y0, (LCDML_DISP_BOX_X1 - LCDML_DISP_BOX_X0), (LCDML_DISP_BOX_Y1 - LCDML_DISP_BOX_Y0) );
  } while( u8g2.nextPage() );
}


// *****************************************************************************
// Set channel time
// *****************************************************************************
void mFunc_set_interval(uint8_t param) {
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

    char bufStart[15];
    sprintf (bufStart, "Start: %02d:%02d:%02d", dateTime.Hour, dateTime.Minute, dateTime.Second);
    
    char bufEnd[15];
    sprintf (bufEnd, "End:  %02d:%02d:%02d", dateTime.Hour, dateTime.Minute, dateTime.Second);
  
    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), bufStart);
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 2), bufEnd);
  
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
    setTime(makeTime(dateTime));
  }
}


// *****************************************************************************
// Set time
// *****************************************************************************
void mFunc_ch_time(uint8_t param) {
  static TimeElements dateTime;

  if (LCDML.FUNC_setup())     // ****** Setup *********
  {
    LCDML.FUNC_disableScreensaver();
    LCDML.TIMER_msReset(timer);
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
    dateTime.Hour = channel[param].time.getStartTime() / 3600;
    dateTime.Minute = channel[param].time.getStartTime() % 3600 / 60;
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    timeMenu(dateTime, timer);
  }

  if(LCDML.FUNC_close())      // ****** END *********
  {
    channel[param].time.setStartTime(makeTime(dateTime));
  }
}


// *****************************************************************************
// Set duration
// *****************************************************************************
void mFunc_ch_duration(uint8_t param) {
  static TimeElements dateTime;

  if (LCDML.FUNC_setup())     // ****** Setup *********
  {
    LCDML.FUNC_disableScreensaver();
    LCDML.TIMER_msReset(timer);
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
    dateTime.Hour = channel[param].time.getStartTime() / 3600;
    dateTime.Minute = channel[param].time.getStartTime() % 3600 / 60;
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    timeMenu(dateTime, timer);
  }

  if(LCDML.FUNC_close())      // ****** END *********
  {
    channel[param].time.setStartTime(makeTime(dateTime));
  }
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
// Set global time
// *****************************************************************************
void mFunc_set_time(uint8_t param) {
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
    timeMenu(dateTime, timer);
  }

  if(LCDML.FUNC_close())      // ****** END *********
  {
    setTime(makeTime(dateTime));
  }
}


// *****************************************************************************
// Set global date
// *****************************************************************************
void mFunc_set_date(uint8_t param) {
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
    dateMenu(dateTime, timer);
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    time_t t = now();
    dateTime.Hour = hour(t);
    dateTime.Minute = minute(t);
    dateTime.Second = second(t);
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
    LCDML.FUNC_disableScreensaver();
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds

    index = 0;
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), String(F("Reading from SD")).c_str());
      progressBar((index * 100) / NUM_CHANNEL, 2);
    } while( u8g2.nextPage() );

    if(index < NUM_CHANNEL) {
      if(parseConfig(index)) {
        index++;
      }
      else{
        Serial.println(F("Could not read from SD"));
      }
    }
    else {
      LCDML.MENU_goRoot();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    
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
    dataFile = SD.open(BACKUP_NAME, (O_WRITE | O_CREAT | O_TRUNC));
  }

  if(LCDML.FUNC_loop())       // ****** LOOP *********
  {
    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( LCDML_DISP_FRAME_OFFSET, (LCDML_DISP_FONT_H * 1), String(F("Saving to SD")).c_str());
      progressBar((index * 100) / NUM_CHANNEL, 2);
    } while( u8g2.nextPage() );
    
    if (LCDML.TIMER_ms(timer, 1000)) {
      if(index >= NUM_CHANNEL) {
        LCDML.MENU_goRoot();
      }
      
      if(dataFile) {
        String header(F("Channel"));
        header.concat(index);
        channel[index].print(Serial, header);
        channel[index].print(dataFile, header);
        index++;
      }
      timer = millis();
    }    
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

    if (timeStatus() == timeSet) {
      char date_buf[11];
      sprintf (date_buf, "%02d.%02d.%d", day(t), month(t), year(t));
      
      char time_buf[8];
      sprintf (time_buf, "%02d:%02d:%02d", hour(t), minute(t), second(t));
  
      u8g2.setFont(LCDML_DISP_FONT);
      u8g2.firstPage();
      do {
        u8g2.drawStr(34, (LCDML_DISP_FONT_H * 1), date_buf);
        u8g2.drawStr(40, (LCDML_DISP_FONT_H * 2), time_buf);
      } while( u8g2.nextPage() );
    }
    else {
      u8g2.setFont(LCDML_DISP_FONT);
      u8g2.firstPage();
      do {
        u8g2.drawStr( 25, (LCDML_DISP_FONT_H * 1), "Date not set!");
        u8g2.drawStr( 25, (LCDML_DISP_FONT_H * 2), "Time not set!");
      } while( u8g2.nextPage() );
    }

    
    if (LCDML.BT_checkAny()) {
      // Leave function
      LCDML.FUNC_goBackToMenu();
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


