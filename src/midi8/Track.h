/**
*Track.h
*/
#ifndef TRACK_H
#define TRACK_H
#include "Notes.h"
#include "Menu.h"
#include "SqncrConfig.h"
class Track{
  public:
  Track(const char * name_):
  lockList{"UnLock","Lock"},
  enableList{"Dis","En"},
  octaveList{OCTAVE_LIST}
  {      
        page        = new Page(name_);
        back        = new Goto(0,"<==");
        itm0_Chan   = new IntItem("Chan",1,1,11,1);
        itm1_Enable = new ListItem("En",enableList,2);
        itm2_Lock   = new ListItem("Lk",lockList,2);
        itm3_Octave = new ListItem("Oct",octaveList,OCTAVE_LIST_SIZE);
        itm3_Octave->setVal(OCTAVE_MIDDLE_C_IDX);
        itm4_Span   = new IntItem("Span",1,1,5,1);
        itm5_Vel    = new IntItem("Vel",65,0,127,1);
       
        page->addItem(back);
        page->addItem(itm0_Chan);
        page->addItem(itm1_Enable);
        page->addItem(itm2_Lock);
        page->addItem(itm3_Octave);
        page->addItem(itm4_Span);
        page->addItem(itm5_Vel);
        
    }

    void update(uint16_t samples_[]){
      
      chan = itm0_Chan->getVal();
      enable = itm1_Enable->getVal();
      //log the samples
      lock = itm2_Lock->getVal();
      if(!lock){
        for(int ii=0;ii<NSTEP;ii++){
          samples[ii][1]=samples[ii][0];
          samples[ii][0]=samples_[ii];
        }
      }
      
      octave = itm3_Octave->getVal();//0-10
      span   = itm4_Span->getVal();//0-3 in number of octaves
      vel    = itm5_Vel->getVal();
    }
    
    Page * getPage(){
      return this->page;
    }
    
    uint8_t getChan(){
      return chan-1;
    }

    uint8_t getVel(){
      return vel;
    }
    
    void setChan(uint8_t chan_){
      chan = chan_;
      itm0_Chan->setVal(chan_);
    }

    uint8_t getEnable(){
      return enable;
    }

    void setOctave(uint8_t oct_){
      itm3_Octave->setVal(oct_);
    }
    
    uint8_t getKnob(uint8_t step_){
      knob[step_]   = samples[step_][0] / (0x400 / (NOTES_LIST_SIZE*span + 1));
      return knob[step_];
    }
    
    uint8_t getNoteStep(uint8_t step_){
      uint8_t retval = 60;//return a middle C
      if(step_ >=0 && step_ < NSTEP){
        //there are 12 notes in each octave
        int nt = 0;
        nt     = (NOTES_LIST_SIZE * octave) + getKnob(step_) - 1;
        if(nt > 127){nt = 127;}
        retval = (uint8_t)nt;
      }
      note[step_][1] = note[step_][0];
      note[step_][0] = retval;
      return retval;
    }
    
    uint8_t getNoteStepLast(uint8_t step_){
      return note[step_][1]; 
    }
    
  private:
    Goto     * back;
    IntItem  * itm0_Chan;
    ListItem * itm1_Enable;
    ListItem * itm2_Lock;
    ListItem * itm3_Octave;
    IntItem  * itm4_Span;
    IntItem  * itm5_Vel;
    const char * lockList[2];
    const char * enableList[2];
    const char * octaveList[OCTAVE_LIST_SIZE];
    const char * noteList[NOTES_LIST_SIZE];
    Page       * page;
    int16_t  chan;//what channel to play the track on
    int16_t  samples[NSTEP][2];//holds adc samples to lock second row of samples for detecting changes
    int16_t  enable;
    int16_t  lock;//0|1 0 takes readings from the samples, 1 locks the samples
    int16_t  octave;//0-9
    int16_t  span;//how many octaves the knob spans 12,24,36
    uint8_t  knob[NSTEP];
    uint8_t  note[NSTEP][2];
    uint8_t  vel;
};
#endif
