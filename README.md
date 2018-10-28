# teensy_baby8
music sequencer based on teensy3.2 micro

Features:
8 Steps, 32 divisions per step
8 Pots read on analog inputs. Each pot set the values value of note for each step
8 Lighted buttons
--Lights to show which step is active
--Ligts when button is pressed
--Button to play note
--Buttone to reset sequence
MIDI - This sequmcer sends MIDI.
Used midi commands send via serial to a program like Ableton Live. Serial is used to be compatible with older synthesizers.

<Todo> Make Midi interface condfigurable so that the Teensy build in MIDI HID interface could be used. Would eliminate the need
for a MIDI interface. This project was tested uising an inexpensive USB midi interface connected to a PC.

<TODO> add Gate and CV outs using the Teensy onboard DAC and digital IO.

Oled Display SSD1306 128 x 64
-used to provide menu interface to make changes to the behavior of the sequencer
-set BPM (beats per minute)
-select midi channel
-turn on/off tracks

Encoder KY-40 (Amazon)
-used incremental encoder with pushbutton to navigate menu
-Custom class used to handle increment,decrement and pushbutton events.
provides tracker for position and direction, and interrupts.

Menu
Class which derives from the Adafruit grpahics libary.
Defines Pages,GotoItems, ListItems,IntItems,FloatItems.
