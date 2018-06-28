// =============================================================================
//
// Control
//
// =============================================================================


/*
 * Thanks to "MuchMore" (Arduino forum) to add this encoder functionality
 *
 * rotate left = Up
 * rotate right = Down
 * push = Enter
 * push long = Quit
 * push + rotate left = Left
 * push + rotate right = Right
 */

/* encoder connection
 * button * (do not use an external resistor, the internal pullup resistor is used)
 * .-------o  Arduino Pin
 * |
 * |
 * o  /
 *   /
 *  /
 * o
 * |
 * '-------o   GND
 *
 * encoder * (do not use an external resistor, the internal pullup resistors are used)
 *
 * .---------------o  Arduino Pin A
 * |        .------o  Arduino Pin B
 * |        |
 * o  /     o  /
 *   /        /
 *  /        /
 * o        o
 * |        |
 * '--------o----o   GND (common pin)
 */


#define ENCODER_OPTIMIZE_INTERRUPTS //Only when using pin2/3 (or 20/21 on mega)
#include <Encoder.h> //for Encoder    Download:  https://github.com/PaulStoffregen/Encoder

Encoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);

long g_LCDML_CONTROL_button_press_time = 0;
bool g_LCDML_CONTROL_button_prev = HIGH;

void lcdml_menu_control(void) {

  // If something must init, put in in the setup condition
  if (LCDML.BT_setup()) {
    // runs only once

    // init pins
    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);
    pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(EXTERNAL_BUTTON_PIN, INPUT_PULLUP);
  }

  //volatile Variable
  long g_LCDML_CONTROL_Encoder_position = encoder.read();
  bool enter = digitalRead(ENCODER_BUTTON_PIN);
  bool quit = digitalRead(EXTERNAL_BUTTON_PIN);

  if (g_LCDML_CONTROL_Encoder_position <= -3) {

    if (!enter) {
      LCDML.BT_left();
      g_LCDML_CONTROL_button_prev = LOW;
      g_LCDML_CONTROL_button_press_time = -1;
    }
    else {
      LCDML.BT_down();
    }
    encoder.write(g_LCDML_CONTROL_Encoder_position + 4);
  }
  else if (g_LCDML_CONTROL_Encoder_position >= 3) {

    if (!enter) {
      LCDML.BT_right();
      g_LCDML_CONTROL_button_prev = LOW;
      g_LCDML_CONTROL_button_press_time = -1;
    }
    else {
      LCDML.BT_up();
    }
    encoder.write(g_LCDML_CONTROL_Encoder_position - 4);
  }
  else {
    if (!enter && g_LCDML_CONTROL_button_prev)  //falling edge, button pressed
    {
      g_LCDML_CONTROL_button_prev = LOW;
      g_LCDML_CONTROL_button_press_time = millis();
    }
    else if (enter && !g_LCDML_CONTROL_button_prev) //rising edge, button not active
    {
      g_LCDML_CONTROL_button_prev = HIGH;

      if (g_LCDML_CONTROL_button_press_time < 0) {
        g_LCDML_CONTROL_button_press_time = millis();
        //Reset for left right action
      }
      else if ((millis() - g_LCDML_CONTROL_button_press_time) >= CONTROL_BUTTON_LONG_PRESS) {
        LCDML.BT_quit();
      }
      else if ((millis() - g_LCDML_CONTROL_button_press_time) >= CONTROL_BUTTON_SHORT_PRESS) {
        LCDML.BT_enter();
      }
    }
  }

  if (!quit) {
    if ((millis() - g_LCDML_CONTROL_button_press_time) >= CONTROL_BUTTON_DEBOUNCE) {
      g_LCDML_CONTROL_button_press_time = millis();
      click_count = 0;
      LCDML.BT_quit();
    }
  }
}
