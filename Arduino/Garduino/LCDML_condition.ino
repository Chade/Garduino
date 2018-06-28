// =============================================================================
//
// Conditions to show or hide a menu element on the display
//
// =============================================================================


// *****************************************************************************
// Condition show
// *****************************************************************************
boolean COND_show() {
  return true;  // shown
}


// *****************************************************************************
// Condition hide
// *****************************************************************************
boolean COND_hide() {
  return false;  // hidden
}


// *****************************************************************************
// Condition reset
// *****************************************************************************
boolean COND_reset() {
  click_count = 0;
  return true;  // hidden
}

// *****************************************************************************
// Condition channel is enabled
// *****************************************************************************
boolean COND_enabled_ch0() {
  return channel[0].enabled;
}

boolean COND_enabled_ch1() {
  return channel[1].enabled;
}

boolean COND_enabled_ch2() {
  return channel[2].enabled;
}

boolean COND_enabled_ch3() {
  return channel[3].enabled;
}

boolean COND_enabled_ch4() {
  return channel[4].enabled;
}

boolean COND_enabled_ch5() {
  return channel[5].enabled;
}

boolean COND_enabled_ch6() {
  return channel[6].enabled;
}

boolean COND_enabled_ch7() {
  return channel[7].enabled;
}
