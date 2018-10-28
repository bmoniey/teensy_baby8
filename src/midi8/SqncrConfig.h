/**
* #defines used by the sequencer and it subclasses
*/
#ifndef SQNCR_CONFIG_H
#define SQNCR_CONFIG_H

#define NSTEP 8  //Number of steps the sequencer supports
				 //each step is a beat
#define RSTEP 32 //Number of counts per step
				 //defines number of beat fractions
#define  PCF8574_ISR_PIN 5
#define  PCF8574_ADDR_ARRY {0x20,0x21} //should be as long as there are boards
                       //should also be in order from lowest to highest
#define  PCF8574_NUM_BOARDS 2
  
#define NTRACK 1 //number of tracks i.e. could have multiple tracks
#define TICK_US(x)	(60000000/RSTEP/(x));
#define TMR_MAX      0xFFFFFFFF //microseconds
#define CLK_TIME_TO_NORM 0x7FFFFFFF //microseconds
#define MIDI_SERIAL Serial1
#define MIDI_SERIAL_SPEED 31250
#define BPM_MAX  999
#define BPM_MIN   10

#define DSPLY_I2C_ADDR 0x3C


#define LED_PIN_ONBOARD 13

#define BTN_LONGPRESS_MS 500

#define MIDI_NOTE_ON 0x90
#define MIDI_NOTE_OFF 0x80
#define ON 1
#define OFF 0

// Uncomment to enable printing out nice debug messages.
#define STEP_SEQUENCER_DEBUG

// Define where debug output will be printed.
#define SQNCR_DEBUG_PRINTER Serial

// Setup debug printing macros.
#ifdef STEP_SEQUENCER_DEBUG
	  #define SQNCR_DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
	  #define SQNCR_DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
	  #define SQNCR_DEBUG_PRINT(...) {}
	  #define SQNCR_DEBUG_PRINTLN(...) {}
#endif

#endif
