/* 
	Editor: https://www.visualmicro.com/
			visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
			all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino/Genuino Mega w/ ATmega2560 (Mega 2560), Platform=avr, Package=arduino
*/

#define __AVR_ATmega2560__
#define ARDUINO 10803
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define F_CPU 16000000L
#define ARDUINO 10803
#define ARDUINO_AVR_MEGA2560
#define ARDUINO_ARCH_AVR
bool parseConfig(const byte& idx);
bool parseConfig();
void flowCounterInterrupt();
void sdDetectInterrupt();
//
//
void initMenu();
void updateMenu();
boolean COND_show();
boolean COND_hide();
void lcdml_menu_control(void);
void mDyn_ch_select(uint8_t line);
void mDyn_ch_enable(uint8_t line);
void mDyn_ch_skip(uint8_t line);
void mDyn_ch_start(uint8_t line);
void mDyn_ch_duration(uint8_t line);
void mDyn_ch_flow (uint8_t line);
void mDyn_moist_enable (uint8_t line);
void mDyn_moist_invert (uint8_t line);
void mDyn_moist_low (uint8_t line);
void mDyn_moist_high (uint8_t line);
void mDyn_rain_enable (uint8_t line);
void mDyn_rain_invert (uint8_t line);
void mDyn_rain_low (uint8_t line);
void mDyn_rain_high (uint8_t line);
void mDyn_bright_enable (uint8_t line);
void mDyn_bright_invert (uint8_t line);
void mDyn_bright_low (uint8_t line);
void mDyn_bright_high (uint8_t line);
void mDyn_move_enable (uint8_t line);
void mDyn_move_invert (uint8_t line);
void mDyn_move_wait (uint8_t line);
void mFunc_set_clock(uint8_t param);
void mFunc_readSD(uint8_t param);
void mFunc_writeSD(uint8_t param);
void mFunc_home(uint8_t param);
void mFunc_back(uint8_t param);

#include "pins_arduino.h" 
#include "arduino.h"
#include "Garduino.ino"
#include "LCDML_begin.ino"
#include "LCDML_condition.ino"
#include "LCDML_control.ino"
#include "LCDML_display_dynFunction.ino"
#include "LCDML_display_menu.ino"
#include "LCDML_display_menuFunction.ino"
