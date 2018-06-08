// =============================================================================
//
// Menu callback functions
//
// =============================================================================

uint8_t col_count = 0;
uint8_t blink_count = false;
bool blink = true;
bool modify = false;
unsigned long timer = 0;
TimeElements dateTime;


// *****************************************************************************
// Set time
// *****************************************************************************
void mFunc_set_time(uint8_t param) {
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    breakTime(now(), dateTime);

    // Disable the screensaver for this function until it is closed
    LCDML.FUNC_disableScreensaver();
    LCDML.TIMER_msReset(timer);
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    if(LCDML.TIMER_ms(timer, 500))
    {
      blink = !blink;
      timer = millis();
    }

    if(LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      if(LCDML.BT_checkEnter())
      {
        modify = !modify;
        LCDML.BT_resetEnter();
      }

      if(LCDML.BT_checkUp())
      {
        if(!modify)
        {
          col_count = (col_count <= 3) ? col_count + 3 : 0;
        }
        else
        {
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
        LCDML.BT_resetUp();
      }

      if(LCDML.BT_checkDown())
      {
        if(!modify)
        {
          col_count = (col_count >= 3) ? col_count - 3 : 6;
        }
        else
        {
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
        LCDML.BT_resetDown();
      }


      if(LCDML.BT_checkLeft())
      {
        col_count = (col_count >= 3) ? col_count - 3 : 6;
        LCDML.BT_resetLeft();
      }

      if(LCDML.BT_checkRight())
      {
        col_count = (col_count <= 3) ? col_count + 3 : 0;
        LCDML.BT_resetRight();
      }
    }

    char buf[8];
    sprintf (buf, "%02d:%02d:%02d", dateTime.Hour, dateTime.Minute, dateTime.Second);

    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( 0, (LCDML_DISP_FONT_H * 1), "Adjust Time:");
      u8g2.drawStr( 1, (LCDML_DISP_FONT_H * 2), buf);

      if(!modify || blink) {
        u8g2.drawFrame( (LCDML_DISP_FONT_W * col_count), (LCDML_DISP_FONT_H * 1) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 2) + 1, LCDML_DISP_FONT_H - 1);
      }
    } while( u8g2.nextPage() );
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    setTime(makeTime(dateTime));
  }
}


// *****************************************************************************
// Set date
// *****************************************************************************
void mFunc_set_date(uint8_t param) {
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    breakTime(now(), dateTime);

    // Disable the screensaver for this function until it is closed
    LCDML.FUNC_disableScreensaver();
    LCDML.TIMER_msReset(timer);
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    if(LCDML.TIMER_ms(timer, 500))
    {
      blink = !blink;
      timer = millis();
    }

    if(LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      if(LCDML.BT_checkEnter())
      {
        modify = !modify;
        LCDML.BT_resetEnter();
      }

      if(LCDML.BT_checkUp())
      {
        if(!modify)
        {
          col_count = (col_count <= 3) ? col_count + 3 : 0;
        }
        else
        {
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
        LCDML.BT_resetUp();
      }

      if(LCDML.BT_checkDown())
      {
        if(!modify)
        {
          col_count = (col_count >= 3) ? col_count - 3 : 6;
        }
        else
        {
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
        LCDML.BT_resetDown();
      }


      if(LCDML.BT_checkLeft())
      {
        col_count = (col_count >= 3) ? col_count - 3 : 6;
        LCDML.BT_resetLeft();
      }

      if(LCDML.BT_checkRight())
      {
        col_count = (col_count <= 3) ? col_count + 3 : 0;
        LCDML.BT_resetRight();
      }
    }

    char buf[11];
    sprintf (buf, "%02d.%02d.%04d", dateTime.Day, dateTime.Month, dateTime.Year + 1970);

    u8g2.setFont(LCDML_DISP_FONT);
    u8g2.firstPage();
    do {
      u8g2.drawStr( 0, (LCDML_DISP_FONT_H * 1), "Adjust Date:");
      u8g2.drawStr( 1, (LCDML_DISP_FONT_H * 2), buf);

      if(!modify || blink) {
        if (col_count == 6) {
          u8g2.drawFrame( (LCDML_DISP_FONT_W * col_count), (LCDML_DISP_FONT_H * 1) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 4) + 1, LCDML_DISP_FONT_H - 1);
        }
        else {
          u8g2.drawFrame( (LCDML_DISP_FONT_W * col_count), (LCDML_DISP_FONT_H * 1) + (LCDML_DISP_FONT_H / 4), (LCDML_DISP_FONT_W * 2) + 1, LCDML_DISP_FONT_H - 1);
        }
      }
    } while( u8g2.nextPage() );
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
// Homescreen
// *****************************************************************************
void mFunc_home(uint8_t param) {
  // Setup
  if(LCDML.FUNC_setup()) {
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
        u8g2.drawStr(LCDML_DISP_FONT_W * 6, (LCDML_DISP_FONT_H * 2), date_buf);
        u8g2.drawStr(LCDML_DISP_FONT_W * 7, (LCDML_DISP_FONT_H * 3), time_buf);
      } while( u8g2.nextPage() );
    }
    else {
      u8g2.setFont(LCDML_DISP_FONT);
      u8g2.firstPage();
      do {
        u8g2.drawStr( LCDML_DISP_FONT_W * 4, (LCDML_DISP_FONT_H * 2), "Date not set!");
        u8g2.drawStr( LCDML_DISP_FONT_W * 4, (LCDML_DISP_FONT_H * 3), "Time not set!");
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

