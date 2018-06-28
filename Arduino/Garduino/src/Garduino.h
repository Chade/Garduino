#ifndef _GARDUINO_H_
#define _GARDUINO_H_


// *****************************************************************************
// Defines
// *****************************************************************************

#define BAUD_RATE                    9600

// Pin settings
#define FLOW_PIN                     21
#define SD_CS_PIN                    47
#define LCD_CS_PIN                   53
#define RELAY1_PIN                   4
#define RELAY2_PIN                   5
#define RELAY3_PIN                   6
#define RELAY4_PIN                   7
#define MOISTURE_PIN                 A0
#define RAIN_PIN                     A1
#define BRIGHTNESS_PIN               A2
#define MOVEMENT_PIN                 3
#define MOVEMENT_DECAY               60
// Encoder
#define ENCODER_A_PIN                18    // physical pin has to be 2 or 3 to use interrupts (on mega e.g. 20 or 21), use internal pullups
#define ENCODER_B_PIN                19    // physical pin has to be 2 or 3 to use interrupts (on mega e.g. 20 or 21), use internal pullups
#define ENCODER_BUTTON_PIN           49    // physical pin , use internal pullup
#define EXTERNAL_BUTTON_PIN          48    // physical pin , use internal pullup
// Button
#define CONTROL_BUTTON_DEBOUNCE      200   // ms
#define CONTROL_BUTTON_LONG_PRESS    800   // ms
#define CONTROL_BUTTON_SHORT_PRESS   120   // ms
// Settings for u8g lib and LCD
#define LCDML_DISP_WIDTH             128            // LCD width
#define LCDML_DISP_HEIGHT            64             // LCD height
// Font settings
#define LCDML_DISP_FONT              u8g_font_6x12  // u8glib font (more fonts under u8g.h line 1520 ...)
#define LCDML_DISP_FONT_W            6              // font width
#define LCDML_DISP_FONT_H            12             // font height
// Cursor settings
#define LCDML_DISP_CURSOR_CHAR       ">"            // cursor char
#define LCDML_DISP_CUR_SPACE_BEFORE  2              // cursor space between
#define LCDML_DISP_CUR_SPACE_BEHIND  4              // cursor space between
// Menu position and size
#define LCDML_DISP_BOX_X0            0              // start point (x0, y0)
#define LCDML_DISP_BOX_Y0            0              // start point (x0, y0)
#define LCDML_DISP_BOX_X1            128            // width x  (x0 + width)
#define LCDML_DISP_BOX_Y1            64             // hight y  (y0 + height)
#define LCDML_DISP_FRAME             1              // draw a box around the menu
#define LCDML_DISP_FRAME_OFFSET      LCDML_DISP_BOX_X0 + LCDML_DISP_FONT_W + LCDML_DISP_CUR_SPACE_BEHIND
// Scrollbar settings
#define LCDML_DISP_SCROLL_WIDTH      6              // scrollbar width (if this value is < 3, the scrollbar is disabled)
// Rows and cols
#define LCDML_DISP_COLS_MAX          ((LCDML_DISP_BOX_X1-LCDML_DISP_BOX_X0)/LCDML_DISP_FONT_W)
#define LCDML_DISP_ROWS_MAX          ((LCDML_DISP_BOX_Y1-LCDML_DISP_BOX_Y0-((LCDML_DISP_BOX_Y1-LCDML_DISP_BOX_Y0)/LCDML_DISP_FONT_H))/LCDML_DISP_FONT_H)
// When you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
// The program needs more ram with this changes
#define LCDML_DISP_ROWS              LCDML_DISP_ROWS_MAX  // max rows
#define LCDML_DISP_COLS              20                   // max cols
// Channel settings
#define NUM_CHANNEL                  4
#define FILE_NAME                    "config.txt"


// *****************************************************************************
// Sanity checks
// *****************************************************************************
# if(LCDML_DISP_ROWS > _LCDML_DISP_cfg_max_rows)
# error ERROR: Change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib2.h
# endif

# if(LCDML_DISP_BOX_X1 > LCDML_DISP_WIDTH)
# error ERROR: LCDML_DISP_BOX_X1 is to big
# endif

# if(LCDML_DISP_BOX_Y1 > LCDML_DISP_HEIGHT)
# error ERROR: LCDML_DISP_BOX_Y1 is to big
# endif



#endif
