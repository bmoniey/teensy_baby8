/**
*StepSequencer.cpp
*/
#include "StepSequencer.h"
#include "Encoder.h"

StepSequencer::StepSequencer(uint16_t bpm_){
  pinMode(LED_PIN_ONBOARD, OUTPUT); 
	cntr 	 = 0; //tracks each fractional step
	curStep	 = 0;
	bpm		 = bpm_;//0 to 255 bpm
  tick_us = TICK_US(bpm);
	state    = 0;
	stateEntry = 1;
  snd=0;
	tmr[0] = millis(); //used to hold current and last call to millis;
	tmr[1] = tmr[0];
  clk[0] = 0;//clk is the current time pointer
  clk[1] = tick_us;//clk1 is in the future
	for(int ii = 0; ii<NSTEP; ii++){
	    stp[ii].actv =1;//step is on or off
	    stp[ii].dc   = (RSTEP/8);//how many beat fractions the note is on
	    stp[ii].note = 65;//the midi note to play 1-127
	    stp[ii].vel  = 45; //velocity to play the note
	    stp[ii].note_on_cntr  = ii*RSTEP;//if the current counter matches then send the note!!
	    stp[ii].note_off_cntr = stp[ii].note_on_cntr+stp[ii].dc;//if the current counter matched then send the note off!!	
	}
 
  MIDI_SERIAL.begin(MIDI_SERIAL_SPEED);
  btnModeResetRequestedAtStep = -1;
}

void StepSequencer::begin(void){
  SQNCR_DEBUG_PRINTLN("StepSequencer::begin()");
  sbled.begin();
  timer.begin();
  menu.begin();
}

void StepSequencer::loop(void){
  
  //update our tick time
  bpm = menu.mdata.bpm;
  uint32_t bpm32 = (uint32_t)bpm;
  tick_us = TICK_US(bpm32);//calculates how long a tick is!
  
	//update the timer
	tmr[1]=tmr[0];
	tmr[0]=micros();
  if(tmr[0]>tmr[1]){
    dT    = tmr[0]-tmr[1];
  }else{
    //look for over flow
    dT = tmr[0] + TMR_MAX - tmr[1];
  }

  //maintain our clock which avoids over flow
  if(clk[0] > CLK_TIME_TO_NORM){
    clk[0] += dT;
  }
  else{
    clk[0] -= CLK_TIME_TO_NORM;
    clk[0] +=dT;
    clk[1]  -= CLK_TIME_TO_NORM;
  }
  
	//is it time to tick the counter
  if(clk[0] > clk[1])
  {
    clk[1] += tick_us;
    //run the sequencer
    timer.toggle(TIMER_IDX_TICK);
    doTick();
    sample();
    menu.update(samples);
  }
}

void StepSequencer::doTick(void){
      cntr32++; //update the global 32 bit counter
      //read the buttons
      sbled.isrRead();      

      monBtn();
     
     //check noteon
     if(cntr == stp[curStep].note_on_cntr)
     {
       if(stp[curStep].actv){
         //send a note_on
         //stp[curStep].note = samples[curStep]/8;//convert to a midi note
         for(int ii = 0; ii< 3;ii++){
          if(menu.track[ii]->getEnable() && menu.track[ii]->getKnob(curStep)>0){
            lastTrackNote[ii] = menu.track[ii]->getNoteStep(curStep);
            writeMIDI(MIDI_NOTE_ON, lastTrackNote[ii], menu.track[ii]->getVel(),menu.track[ii]->getChan());
          }
         }
         //turn on the LED
         sbled.ledSet(curStep,ON);
         snd=1;
         showStep();
         timer.start(TIMER_IDX_STEP);
       }
     }else if(cntr == stp[curStep].note_off_cntr){
       if(stp[curStep].actv){
        //send a note_off
        for(int ii = 0; ii< 3;ii++){
          if(menu.track[ii]->getEnable() && menu.track[ii]->getKnob(curStep)>0){
            writeMIDI(MIDI_NOTE_OFF, lastTrackNote[ii], menu.track[ii]->getVel(),menu.track[ii]->getChan());
          }
        }
        //turn off
        sbled.ledSet(curStep,OFF);
        snd=1;
        showStep();
        timer.stop(TIMER_IDX_STEP);
       }
     }
     
     //check to see if its time to advance a step
     if(cntr == (curStep*RSTEP + RSTEP - 1)){
        curStep++;
        if(curStep >= NSTEP){
          curStep=0;//reset the step
        }
     }
    
     cntr++;
        
     if(cntr > NSTEP*RSTEP){
        cntr = 0;
     }

     //do the skip reset check
     if(cntr == stp[curStep].note_off_cntr && curStep == btnModeResetRequestedAtStep){
       btnModeResetRequestedAtStep = -1;//clear the request
       cntr = 0;
       curStep = 0;
     }
     
     //update leds
     sbled.write();
}

void StepSequencer::sample(void){
    timer.start(TIMER_IDX_SMPLR);
      for(int ii=0;ii<NSTEP;ii++){
        samples[ii] = analogRead(potPins[ii]);//note is from 0 to 127 and the sensor reads 0-1023
      }
      timer.stop(TIMER_IDX_SMPLR);
}

void StepSequencer::monBtn(void){
     timer.start(TIMER_IDX_BTN);
     
     //see if we are on start tick
     //start with all the leds cleared
     sbled.ledSetAll(OFF);
     
     //check to see if any buttons were pressed
       if(menu.button->getMode() == 0){
        //normal
        doBtnModeNormal();
       }
       else{
         doBtnModeSkip();
       }
         
     timer.stop(TIMER_IDX_BTN);  
}

void StepSequencer::doBtnModeSkip(){
  //the mode is skip mode
  for(int ii=0;ii<NSTEP;ii++){
      if(sbled.btnGet(ii)){
        sbled.ledSet(ii,ON);
      }
  }
  
  //check to see if the button is down for the current step
  if(sbled.btnGet(curStep)){
    btnModeResetRequestedAtStep = curStep;   
  }
}

void StepSequencer::doBtnModeNormal(){
  for(int ii=0;ii<NSTEP;ii++){
              if(sbled.btnGet(ii)){
                sbled.ledSet(ii,ON);
              }   
              //check to see if it was recently pressed
              if(sbled.btnGetDn(ii)){
                writeMIDI(MIDI_NOTE_OFF, btnNote, menu.button->getVel(),menu.button->getChan());//cancels the last note
                btnNote = menu.button->getNote(ii);
                writeMIDI(MIDI_NOTE_ON, btnNote, menu.button->getVel(),menu.button->getChan());
                btnNoteOffCntr = cntr32 + TIME_US(5000000);//make the note of 5s from now as longes 
                btnNoteOn = 1;
                sbled.btnClrDn(ii);
             }
        
             if(sbled.btnGetUp(ii)||(cntr32 > btnNoteOffCntr)){
              //send the note off command
              writeMIDI(MIDI_NOTE_OFF, btnNote, menu.button->getVel(),menu.button->getChan());
              //writeMIDI(MIDI_NOTE_ALL_OFF, 123, 0);
              btnNoteOn = 0;
              btnNoteOffCntr = cntr32;
              sbled.btnClrUp(ii);
             }
         }
}

void StepSequencer::setBpm(uint16_t bpm_){
	if((bpm_ > BPM_MIN) && (bpm_ < BPM_MAX)){ bpm =  bpm_;}
}

//  plays a MIDI note.  Doesn't check to see that
//  cmd is greater than 127, or that data values are  less than 127:
void StepSequencer::writeMIDI(uint8_t cmd, uint8_t pitch, uint8_t velocity) {
  MIDI_SERIAL.write(cmd);
  MIDI_SERIAL.write(pitch);
  MIDI_SERIAL.write(velocity);
}

void StepSequencer::writeMIDI(uint8_t cmd, uint8_t pitch, uint8_t velocity, uint8_t chan) {
  uint8_t xcmd;
  xcmd   = (cmd&0xf0) | (chan & 0x0f);
  writeMIDI(xcmd, pitch, velocity);
}


void StepSequencer::toggleOnBrdLED(void){
      if(brdLED){
      digitalWrite(LED_PIN_ONBOARD,ON);
      brdLED=0;
     }
     else{
      digitalWrite(LED_PIN_ONBOARD,OFF);
      brdLED=1;
     }
}

void StepSequencer::showStep(void){
     toggleOnBrdLED();
     if(showStepActv == SHOW_STP_ACTV_ON){
      sprintf(cstr,"cntr:%3d stp:%d actv:%d on:%d off:%d note:%d snd:%d stmr:%07ld btmr:%07ld\n",cntr,curStep,
             stp[curStep].actv,stp[curStep].note_on_cntr,stp[curStep].note_off_cntr,stp[curStep].note,snd,
             timer.get(TIMER_IDX_STEP),timer.get(TIMER_IDX_BTN));
      SQNCR_DEBUG_PRINT(cstr);
     }
     snd=0;
}

void StepSequencer::readButtonISR(void){
  sbled.isrTrigger();
}

void StepSequencer::readEncClkISR(void){
  menu.readEncClkISR();  
}

void StepSequencer::readEncSwISR(void){
  menu.readEncSwISR();
}

  
