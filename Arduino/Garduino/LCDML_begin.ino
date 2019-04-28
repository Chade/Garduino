// =============================================================================
//
// Menu setup
//
// =============================================================================


// *****************************************************************************
// Menu
// *****************************************************************************

//LCDML_addAdvanced (id, prev_layer   , num, condition           , name              , callback_function , param, menu function type   );
  LCDML_addAdvanced ( 0, LCDML_0      ,   1, NULL                , "Home"            , mFunc_home        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 1, LCDML_0      ,   2, NULL                , "Channel"         , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 2, LCDML_0_2    ,   1, NULL                , ""                , mDyn_ch_select    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 3, LCDML_0_2    ,   2, NULL                , ""                , mDyn_ch_enable    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 4, LCDML_0_2    ,   3, NULL                , ""                , mDyn_ch_start     , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 5, LCDML_0_2    ,   4, NULL                , ""                , mDyn_ch_duration  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 6, LCDML_0_2    ,   5, NULL                , ""                , mDyn_ch_flow      , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 7, LCDML_0_2    ,   6, NULL                , " Moisture"       , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced ( 8, LCDML_0_2_6  ,   1, NULL                , ""                , mDyn_moist_enable , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced ( 9, LCDML_0_2_6  ,   2, NULL                , ""                , mDyn_moist_invert , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (10, LCDML_0_2_6  ,   3, NULL                , ""                , mDyn_moist_low    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (11, LCDML_0_2_6  ,   4, NULL                , ""                , mDyn_moist_high   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (12, LCDML_0_2_6  ,   5, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (13, LCDML_0_2    ,   7, NULL                , " Rain"           , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (14, LCDML_0_2_7  ,   1, NULL                , ""                , mDyn_rain_enable  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (15, LCDML_0_2_7  ,   2, NULL                , ""                , mDyn_rain_invert  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (16, LCDML_0_2_7  ,   3, NULL                , ""                , mDyn_rain_low     , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (17, LCDML_0_2_7  ,   4, NULL                , ""                , mDyn_rain_high    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (18, LCDML_0_2_7  ,   5, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (19, LCDML_0_2    ,   8, NULL                , " Movement"       , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (20, LCDML_0_2_8  ,   1, NULL                , ""                , mDyn_move_enable  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (21, LCDML_0_2_8  ,   2, NULL                , ""                , mDyn_move_invert  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (22, LCDML_0_2_8  ,   3, NULL                , ""                , mDyn_move_wait    , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (23, LCDML_0_2_8  ,   5, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (24, LCDML_0_2    ,   9, NULL                , " Brightness"     , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (25, LCDML_0_2_9  ,   1, NULL                , ""                , mDyn_bright_enable, 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (26, LCDML_0_2_9  ,   2, NULL                , ""                , mDyn_bright_invert, 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (27, LCDML_0_2_9  ,   3, NULL                , ""                , mDyn_bright_low   , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (28, LCDML_0_2_9  ,   4, NULL                , ""                , mDyn_bright_high  , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (29, LCDML_0_2_9  ,   5, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (30, LCDML_0_2    ,  10, NULL                , " <Back>"         , mFunc_back        , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (31, LCDML_0      ,   3, NULL                , ""                , mDyn_ch_skip      , 0    , _LCDML_TYPE_dynParam );
  LCDML_addAdvanced (32, LCDML_0      ,   4, NULL                , "Settings"        , NULL              , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (33, LCDML_0_4    ,   1, NULL                , "Set Clock"       , mFunc_set_clock   , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (34, LCDML_0_4    ,   2, NULL                , "Read from SD"    , mFunc_readSD      , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (35, LCDML_0_4    ,   3, NULL                , "Write to SD"     , mFunc_writeSD     , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (36, LCDML_0_4    ,   4, NULL                , "Read from EEPROM", mFunc_readEEPROM  , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (37, LCDML_0_4    ,   5, NULL                , "Write to EEPROM" , mFunc_writeEEPROM , 0    , _LCDML_TYPE_default  );
  LCDML_addAdvanced (38, LCDML_0_4    ,   6, NULL                , "<Back>"          , mFunc_back        , 0    , _LCDML_TYPE_default  );

// Menu element count
// This value must be the same as the last menu element
#define LCDML_DISP_COUNT 38

// Create menu
LCDML_createMenu(LCDML_DISP_COUNT);


// *****************************************************************************
// Functions
// *****************************************************************************

void initMenu() {
  u8g2.begin();

  // LCDMenuLib Setup
  LCDML_setup(LCDML_DISP_COUNT);

  // Enable Menu Rollover
  LCDML.MENU_enRollover();

  // Enable home menu
  LCDML.SCREEN_enable(mFunc_home, 100000); // set to 10 seconds
}


void updateMenu() {
  LCDML.loop();
}
