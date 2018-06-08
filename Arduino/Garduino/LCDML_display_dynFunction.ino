// =============================================================================
//
// Dynamic content
//
// =============================================================================


// *****************************************************************************
// Helper
// *****************************************************************************
void toggle(const uint8_t & line, const uint8_t& idx) {
  // Check if this function is active (cursor is on this line)
  if (line == LCDML.MENU_getCursorPos()) {
    if (LCDML.BT_checkAny()) {
      if (LCDML.BT_checkEnter()) {
        if (LCDML.MENU_getScrollDisableStatus() == 0) {
          // Disable the menu scroll function to catch the cursor on this point
          // Now it is possible to work with BT_checkUp and BT_checkDown in this function
          // This function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else {
          // Enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }
        LCDML.BT_resetEnter();
      }
      else {
        channel[idx].enabled = !channel[idx].enabled;
      }
    
      LCDML.BT_resetAll();
    }
  }

  String str(F("Channel"));
  str += idx;
  str += (channel[idx].enabled) ? F(": Enabled") : F(": Disabled");
  u8g2.drawStr( LCDML_DISP_BOX_X0 + LCDML_DISP_FONT_W + LCDML_DISP_CUR_SPACE_BEHIND,  (LCDML_DISP_FONT_H * (1 + line)), str.c_str());
}


// *****************************************************************************
// Set channel 0
// *****************************************************************************
void mDyn_channel_0 (uint8_t line) {
  toggle(line, 0); 
}


// *****************************************************************************
// Set channel 1
// *****************************************************************************
void mDyn_channel_1 (uint8_t line) {
  toggle(line, 1); 
}


// *****************************************************************************
// Set channel 2
// *****************************************************************************
void mDyn_channel_2 (uint8_t line) {
  toggle(line, 2); 
}


// *****************************************************************************
// Set channel 3
// *****************************************************************************
void mDyn_channel_3 (uint8_t line) {
  toggle(line, 3); 
}


// *****************************************************************************
// Set channel 4
// *****************************************************************************
void mDyn_channel_4 (uint8_t line) {
  toggle(line, 4); 
}


// *****************************************************************************
// Set channel 5
// *****************************************************************************
void mDyn_channel_5 (uint8_t line) {
  toggle(line, 5); 
}


// *****************************************************************************
// Set channel 6
// *****************************************************************************
void mDyn_channel_6 (uint8_t line) {
  toggle(line, 6); 
}


// *****************************************************************************
// Set channel 7
// *****************************************************************************
void mDyn_channel_7 (uint8_t line) {
  toggle(line, 7); 
}

