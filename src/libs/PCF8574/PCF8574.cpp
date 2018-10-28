#include "PCF8574.h"
#include "Wire.h"

/**
 * Constructor
 * @param address: i2c address
 */
PCF8574::PCF8574( ){
};


/**
 * Constructor
 * @param address: i2c address
 */
PCF8574::PCF8574(uint8_t address){
	_address = address;
};

/**
 * Constructor
 * @param address: i2c address
 * @param interruptPin: pin to set interrupt
 * @param interruptFunction: function to call when interrupt raised
 */
PCF8574::PCF8574(uint8_t address, uint8_t interruptPin,  void (*interruptFunction)() ){
	_address = address;
	_interruptPin = interruptPin;
	_interruptFunction = interruptFunction;
	_usingInterrupt = true;
	
};

#ifndef __AVR
	/**
	 * Constructor
	 * @param address: i2c address
	 * @param sda: sda pin
	 * @param scl: scl pin
	 */
	PCF8574::PCF8574(uint8_t address, uint8_t sda, uint8_t scl){
		_address = address;
		_sda = sda;
		_scl = scl;
		
	};

	/**
	 * Constructor
	 * @param address: i2c address
	 * @param sda: sda pin
	 * @param scl: scl pin
	 * @param interruptPin: pin to set interrupt
 	 * @param interruptFunction: function to call when interrupt raised
	 */
	PCF8574::PCF8574(uint8_t address, uint8_t sda, uint8_t scl, uint8_t interruptPin,  void (*interruptFunction)() ){
		_address = address;
		_sda = sda;
		_scl = scl;

		_interruptPin = interruptPin;
		_interruptFunction = interruptFunction;

		_usingInterrupt = true;
		
	};
#endif

/**
 * wake up i2c controller
 */
void PCF8574::begin(){
	DEBUG_PRINTLN("PCF8574::begin()");
	DEBUG_PRINTLN(_address);
	Wire.begin();
	//start by setting all pins high. all pins only have weak pullups.
	//io port is a quasi-io. meaning they are all inputs.
	//a read from the port get all the pins whether you want it or not!
	portState = 0xFF;
	Wire.write(portState);
	Wire.endTransmission();

	// If using interrupt set interrupt value to pin
	if (_usingInterrupt){
		DEBUG_PRINTLN("Using interrupt pin (not all pin is interrupted)");
		::pinMode(_interruptPin, INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(_interruptPin), (*_interruptFunction), FALLING );
	}

	// inizialize last read
	lastReadMillis = millis();
}

/**
 * Set if fin is OUTPUT or INPUT
 * OUTPUT us defined as 1
 * INPUT  is defined as 0
 * @param pin: pin to set
 * @param mode: mode, supported only INPUT or OUTPUT (to simplify)
 */
void PCF8574::pinMode(uint8_t pin, uint8_t mode){
	DEBUG_PRINT("Set pin: ");
	DEBUG_PRINT(pin);
	DEBUG_PRINT(" as ");
	DEBUG_PRINTLN(mode ? "OUPUT" : "INPUT");

	if(mode){
		//OUTPUT
		rwMode |= bit(pin);
	}
	else{
		rwMode &= ~bit(pin);
	}
	DEBUG_PRINT("rwMode: ");
	DEBUG_PRINTLN(rwMode, BIN);
};

#ifndef PCF8574_LOW_MEMORY
	/**
	 * Read value of all INPUT pin
	 * Debounce read more fast than 10millis, non managed for interrupt mode
	 * @return
	 */
	PCF8574::DigitalInput PCF8574::digitalReadAll(void){
		byte iInput = portState;//should at least have the result from the last read/write.
		DEBUG_PRINT("digitalReadAll:");
		Wire.requestFrom(_address,(uint8_t)1);// Begin transmission to PCF8574 with the buttons
		lastReadMillis = millis();
		errno = 0;
		if(Wire.available())   // If bytes are available to be recieved
		{
			  iInput = Wire.read();// Read a byte
			  DEBUG_PRINTLN(iInput, BIN);		
		}
		else{
			errno = 1;
					  DEBUG_PRINT("Err:");
					  DEBUG_PRINTLN(errno);
		}
		digitalInput.port = iInput;
		digitalInput.p0 = iInput & bit(P0) ? 1 : 0;
		digitalInput.p1 = iInput & bit(P1) ? 1 : 0;
		digitalInput.p2 = iInput & bit(P2) ? 1 : 0;
		digitalInput.p3 = iInput & bit(P3) ? 1 : 0;
		digitalInput.p4 = iInput & bit(P4) ? 1 : 0;
		digitalInput.p5 = iInput & bit(P5) ? 1 : 0;
		digitalInput.p6 = iInput & bit(P6) ? 1 : 0;
		digitalInput.p7 = iInput & bit(P7) ? 1 : 0;

		return digitalInput;
	};
#else
	/**
	 * Read value of all INPUT pin in byte format for low memory usage
	 * Debounce read more fast than 10millis, non managed for interrupt mode
	 * @return
	 */
	byte PCF8574::digitalReadAll(void){
		byte iInput = portState;//should at least have the result from the last read/write.
		errno =0;
		DEBUG_PRINTLN("digitalReadAll::");
		Wire.requestFrom(_address,(uint8_t)1);// Begin transmission to PCF8574 with the buttons
		lastReadMillis = millis();
		if(Wire.available())   // If bytes are available to be received
		{
			  iInput = Wire.read();// Read a byte
			  //set the portState to keep in sync with the actual port
			  portState = iInput;			  
			  DEBUG_PRINTLN(iInput, BIN);
		}
		else{
			errno=2;
		  DEBUG_PRINT("Err:");
		  DEBUG_PRINTLN(errno);
		}
		
		return iInput;
	};
#endif

/**
 * Read value of specified pin
 * Debounce read more fast than 10millis, non managed for interrupt mode
 * @param pin
 * @return
 */
uint8_t PCF8574::digitalRead(uint8_t pin){
	byte iInput = portState;//should at least have the result from the last read/write.
	errno=0;
	DEBUG_PRINT("digitalRead Pin:");
	DEBUG_PRINT(pin);
	// Check if pin already HIGH than read and prevent reread of i2c
	  Wire.requestFrom(_address,(uint8_t)1);// Begin transmission to PCF8574 with the buttons
	  lastReadMillis = millis();
	  if(Wire.available())   // If bytes are available to be recieved
	  {
		  DEBUG_PRINT(" Value:");
		  iInput = Wire.read();// Read a byte
		  //set the portState to keep in sync with the actual port
		  portState = iInput;
		  DEBUG_PRINTLN((bit(pin) & iInput) ? 1 : 0);			  
	  }else{
		  errno = 3;
		  DEBUG_PRINT("Err:");
		  DEBUG_PRINTLN(errno);
	  }
	
	return (uint8_t)(bit(pin) & iInput) ? 1 : 0;
};

void PCF8574::digitalWriteAll(uint8_t portVal){
	DEBUG_PRINT(" digitalWriteALL::value");
	DEBUG_PRINT(portVal,BIN);
	errno=0;
	Wire.beginTransmission(_address);     //Begin the transmission to PCF8574
	portState = portVal;
	portState |= ~rwMode; //sets all the input to high
	DEBUG_PRINT(" portState: ");
	DEBUG_PRINTLN(portState,HEX);
	Wire.write(portState);
	Wire.endTransmission();
}

/**
 * Write on pin
 * @param pin
 * @param value
 */
void PCF8574::digitalWrite(uint8_t pin, uint8_t value){
    DEBUG_PRINT("digitalWrite::pin ");
	DEBUG_PRINT(pin);
	DEBUG_PRINT(" value: ");
	DEBUG_PRINT(value);
	errno=0;
    if(value == HIGH){
		//Set the port bit high
		portState |= bit(pin);
	}
	else{
		portState &= ~bit(pin);
	}
	digitalWriteAll(portState);
};
/**
 * getPortState
 * @param pin
 * @return portState (doesn't execute a read to the bus, just a reflection from last operations)
 */
uint8_t PCF8574::getPortState(void){
	return portState;
}

void    PCF8574::PCF8574::setAddr(uint8_t addr){
	_address = addr;
}

void	PCF8574::setISR(void (*interruptFunction)()){
		_interruptFunction = interruptFunction;
		_usingInterrupt = true;
}
void    PCF8574::setISRPin(uint8_t isrPin){
		_interruptPin = isrPin;
}

