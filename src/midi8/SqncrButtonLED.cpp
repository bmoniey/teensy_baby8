/**
* SqncrButtonLED.cpp
*/
#include "SqncrConfig.h"
#include "SqncrButtonLED.h"

SqncrButtonLED::SqncrButtonLED(){

};

void SqncrButtonLED::begin(void){
  //create the new button boards
  for(int ii = 0; ii< PCF8574_NUM_BOARDS; ii++){   
    pcf8574[ii].setAddr(pcf8574_addr[ii]); 
    // Set pinMode on PCF8574
    pcf8574[ii].pinMode(P0, INPUT);
    pcf8574[ii].pinMode(P1, INPUT);
    pcf8574[ii].pinMode(P2, INPUT);
    pcf8574[ii].pinMode(P3, INPUT);
    pcf8574[ii].pinMode(P4, OUTPUT);
    pcf8574[ii].pinMode(P5, OUTPUT);
    pcf8574[ii].pinMode(P6, OUTPUT);
    pcf8574[ii].pinMode(P7, OUTPUT);
    pcf8574[ii].begin();
  }
  
  //initialize the button states
  for(int ii  = 0; ii< NSTEP; ii++){
    btn[ii] = 0;
    btn_dn[ii] = 0;
  }

 //initialize the rwport values
 for(int brd = 0; brd < PCF8574_NUM_BOARDS; brd++){
    rport[brd][0]=0xff;
    rport[brd][1]=0xff;
    wport[brd]=0xff;
 }
};

void SqncrButtonLED::read(void){
    //read data from the boards
    //update the rport data
      for(int brd = 0; brd < PCF8574_NUM_BOARDS; brd++){
        pcf8574[brd].digitalReadAll();
        rport[brd][1] = rport[brd][0];
        rport[brd][0] = pcf8574[brd].digitalInput.port;
        
        //update the button states
        //the buttons are on the first 4 inputs of each board
        for(int ii = 0; ii < 4; ii++){
          //set the button states
          btn[ii+4*brd] = rport[brd][0] & (1 << ii) ? 0 : 1;
          
          //if button is now down and last call was up the we have a press event
          if(!(rport[brd][0] & (1<<ii)) && (rport[brd][1] & (1<<ii))){
            btn_dn[ii+4*brd] =  1;
            //record the time
            tmr[ii+4*brd][1]= millis();
          }
          
          //if the button is now up and the last call was down then we have a release event
          if((rport[brd][0] & (1<<ii)) && !(rport[brd][1] & (1<<ii))){
            btn_up[ii+4*brd] =  1;
            
            tmr[ii+4*brd][0]= millis();
            if((tmr[ii+4*brd][0] - tmr[ii+4*brd][1]) > BTN_LONGPRESS_MS){
               btn_long[ii+4*brd] =  1;
               tmr[ii+4*brd][0]=0;
               tmr[ii+4*brd][1]=0;
            }
          }
        }
      }
}

void SqncrButtonLED::write(void){
      //The LEDS are on when the port IO is LOW
      //initialize the port to 0xFF
      for(int ii = 0; ii< PCF8574_NUM_BOARDS; ii++){
        wport[ii]=0xFF;
      }

      //Serial.print("brd:led::");
      for(int ii=0, brd=0;ii < NSTEP;ii++){
        if(!(ii % 4) && (ii>0)){
           brd++;           
        }
        //sprintf(cstr,"%d:%d,",brd,led[ii]);
        //Serial.print(cstr);
        if(led[ii]){
          wport[brd] ^= 1 << (ii-brd*4 + 4); //the leds are on the upper 4 outputs
        }
      }
      //Serial.println("");

        for(int brd = 0; brd < PCF8574_NUM_BOARDS; brd++){
          //sprintf(cstr,"wport[%d]:%02X",brd,wport[brd]);
          //Serial.println(cstr);
          pcf8574[brd].digitalWriteAll(wport[brd]);
        }

}
/**
 * turn a led on or off
 */
void SqncrButtonLED::ledSet(uint8_t led_no, uint8_t state){
      if(led_no >=0 && led_no < NSTEP){
        led[led_no] = state ? 1 : 0;
      } 
}

uint8_t SqncrButtonLED::ledGet(uint8_t led_no)
{
      if(led_no >=0 && led_no < NSTEP){
        return led[led_no];
      } 
      else{
        return 0;
      }
}
/**
 *  Sets all the leds to the same state
 */
void SqncrButtonLED::ledSetAll(uint8_t state){
     for(int ii = 0; ii < NSTEP;ii++){
      led[ii]= state ? 1 : 0;
     }
}

uint8_t SqncrButtonLED::btnGet(uint8_t no){
  if(no >=0 && no < NSTEP){
    return btn[no];
  }
  return 0;
}

uint8_t SqncrButtonLED::btnGetUp(uint8_t no){
  if(no >=0 && no < NSTEP){
    return btn_up[no];
  }
  return 0;
}

uint8_t SqncrButtonLED::btnGetDn(uint8_t no){
  if(no >=0 && no < NSTEP){
    return btn_dn[no];
  }
  return 0;
}

uint8_t SqncrButtonLED::btnGetLong(uint8_t no){
  if(no >=0 && no < NSTEP){
    return btn_long[no];
  }
  return 0;
}

void SqncrButtonLED::btnClrDn(uint8_t no){
  if(no >=0 && no < NSTEP){
    btn_dn[no]=0;
  }
}

void SqncrButtonLED::btnClrUp(uint8_t no){
  if(no >=0 && no < NSTEP){
    btn_up[no]=0;
  }
}

void SqncrButtonLED::btnClrLong(uint8_t no){
  if(no >=0 && no < NSTEP){
    btn_long[no]=0;
    tmr[no][0]=0;
    tmr[no][1]=0;
  }
}

void SqncrButtonLED::btnClrAll(void){
    for(int ii   = 0;ii<NSTEP;ii++){
      btn_dn[ii] = 0;
      btn_up[ii] = 0;
      btn_long[ii]=0;
  }
}

void SqncrButtonLED::isrTrigger(void){
  isrTriggerd = 1;
}

void SqncrButtonLED::isrClear(void){
  isrTriggerd = 0;
}

void SqncrButtonLED::isrRead(void){
  if(isrTriggerd){
    read();
    isrTriggerd=0;
  }
}

uint8_t SqncrButtonLED::isrTriggerGet(void){
  return isrTriggerd;
}


