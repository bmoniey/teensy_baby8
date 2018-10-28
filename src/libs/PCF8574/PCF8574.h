/** \mainpage PCF8574 library
 *
 * MIT license
 * written by Renzo Mischianti
 */

#ifndef PCF8574_h
#define PCF8574_h

#include "Wire.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Uncomment to enable printing out nice debug messages.
// #define PCF8574_DEBUG

// Uncomment for low memory usage this prevent use of complex DigitalInput structure and free 7byte of memory
//#define PCF8574_LOW_MEMORY

// Define where debug output will be printed.
#define DEBUG_PRINTER Serial

// Setup debug printing macros.
#ifdef PCF8574_DEBUG
	#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
	#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
	#define DEBUG_PRINT(...) {}
	#define DEBUG_PRINTLN(...) {}
#endif

#define READ_ELAPSED_TIME 10

#define P0  	0
#define P1  	1
#define P2  	2
#define P3  	3
#define P4  	4
#define P5  	5
#define P6  	6
#define P7  	7

#include <math.h>


class PCF8574 {
public:
	PCF8574();
	PCF8574(uint8_t address);
	PCF8574(uint8_t address, uint8_t sda, uint8_t scl);
	PCF8574(uint8_t address, uint8_t interruptPin,  void (*interruptFunction)() );
	PCF8574(uint8_t address, uint8_t sda, uint8_t scl, uint8_t interruptPin,  void (*interruptFunction)());

	void begin();
	void pinMode(uint8_t pin, uint8_t mode);

	void readBuffer(bool force = true);
	uint8_t digitalRead(uint8_t pin);
	#ifndef PCF8574_LOW_MEMORY
		struct DigitalInput {
			uint8_t port;
			uint8_t p0;
			uint8_t p1;
			uint8_t p2;
			uint8_t p3;
			uint8_t p4;
			uint8_t p5;
			uint8_t p6;
			uint8_t p7;
		} digitalInput;


		DigitalInput digitalReadAll(void);
	#else
		byte digitalReadAll(void);
	#endif
	void digitalWrite(uint8_t pin, uint8_t value);
	void digitalWriteAll(uint8_t portVal);
	uint8_t getPortState(void);
	void    setAddr(uint8_t addr);
	void	setISR(void (*interruptFunction)());
	void    setISRPin(uint8_t isrPin);

private:
	uint8_t errno = 0;
	uint8_t _address;
	uint8_t _sda = SDA;
	uint8_t _scl = SCL;
	uint8_t portState = 0xFF;//when writing to the port
							 //must take into consideration
							 //state of all values since
							 //the all port pins must be written to at the same time!!

	bool _usingInterrupt = false;
	uint8_t _interruptPin = 2;
	void (*_interruptFunction)(){};

	//byte writeMode 	= 	B00000000;
	//byte readMode 	= 	B00000000;
	byte 	 rwMode		=   B00000000;//define all the pins as inputs
									  //the chips wakes up with all pins set High with weak pullups.
	unsigned long lastReadMillis = 0;

};

#endif
