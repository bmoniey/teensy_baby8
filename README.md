# teensy_baby8
Music sequencer based on teensy3.2 micro

## Features:

..*8 Steps, 32 divisions per step

..*8 Pots read on analog inputs. Each pot sets the values value of note for each step

..*8 Lighted buttons

	Lights to show which step is active

	Lights when button is pressed

##Buttons:

	Regular Mode: Just to plays notes on a selected channel

	Skip Mode: Buttons cause sequencer to reset at the step of the pressed button

##PCF8574

Uses PCF8574 IO expander to read buttons, drive LEDS and issue an interrupt when any of the input IO's change.
The design used 2 of the boards each driving 4 LEDS and reading 4 Buttons. The PCF8574 library is an adaptation of 
https://github.com/xreef/PCF8574_library

##MIDI - This sequencer sends MIDI via serial.

	Used midi commands send via serial to a program like Ableton Live. Serial is used to be compatible with older synthesizers.

##OLED Display SSD1306 128 x 64

	..*used to provide menu interface to make changes to the behaviour of the sequencer

	..*set BPM (beats per minute)

	..*select midi channel

	..*turn on/off tracks

##Encoder KY-40 (Amazon)

	..*used incremental encoder with push-button to navigate menu

	..*Custom class used to handle increment,decrement and push-button events.
	provides tracker for position and direction, and interrupts.

##Menu

Class which derives from the Adafruit SSD1306 and GFX Library

See:
https://github.com/adafruit/Adafruit_SSD1306
https://github.com/adafruit/Adafruit-GFX-Library

Defines elements needed for a basic menu:

	..*Pages

	..*GotoItems, which are links to other pages

	..*ListItems, Items on a page that display as text strings

	..*IntItems, Items which control an integer number

	..*FloatItems, Items which control floating point numbers

TODO: Make Midi interface configurable so that the Teensy built in MIDI HID interface could be used. Would eliminate the need
for a MIDI interface. This project was tested using an inexpensive USB midi interface connected to a PC.

TODO: Add Gate and CV outs using the Teensy on-board DAC and digital IO.
