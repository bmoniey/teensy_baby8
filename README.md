# teensy_baby8
Music sequencer based on teensy3.2 micro

## 1. Features:

* 8 Steps, 32 divisions per step

* 8 Pots read on analog inputs. Each pot sets the values value of note for each step

* 8 Lighted buttons
	Lights to show which step is active
	Lights when button is pressed

## 2. Buttons:
	Regular Mode: Just to plays notes on a selected channel
	Skip Mode: Buttons cause sequencer to reset at the step of the pressed button

## 3. PCF8574
Uses PCF8574 IO expander to read buttons, drive LEDS and issue an interrupt when any of the input IO's change.
The design used 2 of the boards each driving 4 LEDS and reading 4 Buttons. The PCF8574 library is an adaptation of 
https://github.com/xreef/PCF8574_library

## 4. MIDI - This sequencer sends MIDI via serial port on Teensy 3.2
	Uses MIDI commands sent via serial to a program like Ableton Live.
	Needs a USB to MIDI adpatper.

## 5. OLED Display SSD1306 128 x 64
* Used to provide menu interface to make changes to the behaviour of the sequencer
* Set BPM (beats per minute)
* Select MIDI channel
* Turn On/Off tracks
* Select Octave
* Set Note Velocity
* Select mode of operation for buttons

## 6. Encoder KY-40 (Amazon)
* Uses incremental encoder with push-button to navigate menu
* Custom class used to handle increment,decrement and push-button events.
provides tracker for position and direction, and interrupts.

## 7. Menu
Class which derives from the Adafruit SSD1306 and GFX Library for simple text based
menu system. Encoder moves focus up and down page. Depress button to select. While selected
the item can be manipulated. Changes take immediate effect!

If Adafruit libraries are not already installed in your arduino lib then see here:
https://github.com/adafruit/Adafruit_SSD1306
https://github.com/adafruit/Adafruit-GFX-Library

### 8 Menu Elements:

* Pages

* GotoItems: links to other pages

* ListItems: Items on a page that display as text strings

* IntItems: Items which control an integer number

* FloatItems: Items which control floating point numbers

## TODO:
1. Make Midi interface configurable so that the Teensy built in MIDI HID interface could be used. Would eliminate the need
for a MIDI interface. This project was tested using an inexpensive USB midi interface connected to a PC.
2. Add Gate and CV outs using the Teensy on-board DAC and digital IO.
