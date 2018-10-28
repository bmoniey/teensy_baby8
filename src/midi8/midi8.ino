#include "Arduino.h"
#include "StepSequencer.h"

StepSequencer sqncr(120);

/*
 * Forward declarations for the interrupt service routines
 */
void readButtonsISR(void);
void encoderSwISR(void);
void encoderClkISR(void);

char cstr[255];

void setup() {
 Serial.begin(9600);  
 
 digitalWrite(13,ON);
 
 //button boards interrupts are on pin5
 pinMode(5, INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(5), readButtonsISR, FALLING );
 pinMode(ENC_DT_PIN, INPUT_PULLUP);
 pinMode(ENC_CLK_PIN, INPUT_PULLUP);
 pinMode(ENC_SW_PIN, INPUT_PULLUP);
  
 attachInterrupt(digitalPinToInterrupt(ENC_CLK_PIN), encoderClkISR, FALLING );//attach interrupts if using interrupts
 attachInterrupt(digitalPinToInterrupt(ENC_SW_PIN), encoderSwISR, CHANGE );//attach interrupts if using interrupts
 
 sqncr.begin();
 
 delay(1000);
 Serial.println("Starting Midi7");
}

void loop() { 
  sqncr.loop();
}

void readButtonsISR(void){
  sqncr.readButtonISR();
}

void encoderClkISR(void){
  sqncr.readEncClkISR();
  //Serial.print(".");
}

void encoderSwISR(void){
  sqncr.readEncSwISR();
  //Serial.print("-");
}

