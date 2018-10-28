/**
*Timer.h
*/
#ifndef TIMER_H
#define TIMER_H
#include "Arduino.h"
#define TIMER_NUM   4
#define TIMER_NAMES {"tick","step","buttons","sampler"}

#define TIMER_IDX_TICK 0
#define TIMER_IDX_STEP 1
#define TIMER_IDX_BTN 2
#define TIMER_IDX_SMPLR 3

#define TIMER_STAT_RUN 1
#define TIMER_STAT_STOP 0


class Timer{
	public:
	void begin(){
		for(int ii=0; ii < TIMER_NUM; ii++){
			tmr_us[ii][1] = micros();
			tmr_us[ii][0] = tmr_us[ii][1];
		}
	}
 
	void start(uint8_t no){
		if((no >= 0) && (no<TIMER_NUM)){
      stat[no] = TIMER_STAT_RUN;
			tmr_us[no][1]=micros();
      tmr_us[no][0]=tmr_us[no][1];
		}
	}
 
	void stop(uint8_t no){
		if((no >= 0) &&( no < TIMER_NUM)){
      stat[no] = TIMER_STAT_STOP;
			tmr_us[no][0]=micros();
		}
	}
 
	void reset(uint8_t no){
		if((no >= 0) && (no < TIMER_NUM)){
      stat[no] = TIMER_STAT_STOP;
			tmr_us[no][1]=micros();
			tmr_us[no][0]=tmr_us[no][1];
		}
	}

  void toggle(uint8_t no){
    if((no >= 0) && (no < TIMER_NUM)){
      if(stat[no] == TIMER_STAT_STOP){
          start(no);
      }else{
          stop(no);
      }
    }
  }
	
	uint32_t get(uint8_t no){
		if((no >= 0) && (no < TIMER_NUM)){
			return tmr_us[no][0]-tmr_us[no][1];
		}
		else { return 0;}
	}
	
	private:
    uint8_t    stat[TIMER_NUM]; //0 not running 1 running
	const char * names[TIMER_NUM] = TIMER_NAMES;
	uint32_t tmr_us[TIMER_NUM][2];//array of micros
};
#endif
