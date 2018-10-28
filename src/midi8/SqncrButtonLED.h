/**
* Class to handle the LEDs and Buttons on this step sequencer
* The Sequencer has a button and LED for each Step
* The Buttons are read using the PCF8574 IO chip.
* This class relies on a helper class for the PCF8574 called PCF8574.cpp
* knowledge of how the PCF8574 is connected is essential
* This design assumes each PCF8574 has 4 button inputs on P0-P3,
* and the LEDS on P4-P7. 
*/
#ifndef SQNCRBLED_H
#define SQNCRBLED_H
	#include "Arduino.h"
	#include "SqncrConfig.h"
	#include "PCF8574.h"
	
	class SqncrButtonLED{
		public:
			SqncrButtonLED();
      void begin(void);
      void read(void);//call in ISR or as during loop to update button status
      void write(void);//call to update LEDS.
      void ledSet(uint8_t led_no, uint8_t state);
      uint8_t ledGet(uint8_t led_no);
      void ledSetAll(uint8_t state);
      uint8_t btnGet(uint8_t no);
      uint8_t btnGetDn(uint8_t no);
      uint8_t btnGetUp(uint8_t no);
      void btnClrDn(uint8_t no);
      void btnClrUp(uint8_t no);
      uint8_t btnGetLong(uint8_t no);
      void btnClrLong(uint8_t no);
      void btnClrAll(void);
      void isrTrigger(void);
      uint8_t isrTriggerGet(void);
      void isrClear(void);
      void isrRead(void);
      
		private:
      uint8_t   isrTriggerd;
      char cstr[255];
			const uint8_t   pcf8574_addr[PCF8574_NUM_BOARDS] = PCF8574_ADDR_ARRY;
			PCF8574         pcf8574[PCF8574_NUM_BOARDS];
      uint8_t         wport[PCF8574_NUM_BOARDS];//holds the port value to write the led ports
      uint8_t         rport[PCF8574_NUM_BOARDS][2];//hold the port data from the last reads
      uint8_t         led[NSTEP];
			uint8_t         btn[NSTEP];   //holds last read of all the buttons
			uint8_t         btn_dn[NSTEP];//when a button is pressed down this holds the result
      uint8_t         btn_up[NSTEP];//when a button is up this holds the result
      uint8_t         btn_long[NSTEP];//used to hold long press
			uint32_t        tmr[NSTEP][2];//used to detect length of press
							        //when the the event is serviced the flag should be cleared
	};
  
#endif
