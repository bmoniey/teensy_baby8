/**
 * Button.h
 * create a simple UI using the Menu interface to
 * control how the buttons behave
 */
 #ifndef BUTTON_H
 #define BUTTON_H
 #include "Menu.h"
 #include "Notes.h"
 class Button{
  public:
    Button():
    octaveList{OCTAVE_LIST},
    modeList{"Reg","Skip"}{
      page = new Page("Buttons");
      itm0_goto = new Goto(0,"<==");
      itm1_Chan = new IntItem("Chan",4,1,11,1);
      itm2_Octave = new ListItem("Octv",octaveList,OCTAVE_LIST_SIZE);
      itm2_Octave->setVal(OCTAVE_MIDDLE_C_IDX);
      itm3_Vel =  new IntItem("Vel",64,0,127,1);
      itm4_Mode = new ListItem("Md",modeList,2);
      page->addItem(itm0_goto);
      page->addItem(itm1_Chan);
      page->addItem(itm2_Octave);
      page->addItem(itm3_Vel);
      page->addItem(itm4_Mode);
      chan = 4;
      note = 60;
    }
    
    void update(){
      chan = itm1_Chan->getVal();
      octave = itm2_Octave->getVal();
      vel    = itm3_Vel->getVal();
      mode   = itm4_Mode->getVal();  
    }

    uint8_t getNote(int btn){
      note = 60;//middle c
      if(btn >=0 && btn < NSTEP){
        note = octave * NOTES_LIST_SIZE + btn;
      }
      return note;
    }
    
    uint8_t getChan(){
      return chan-1;
    }
    
    uint8_t getVel(){
      return vel;
    }
    
    uint8_t getMode(){
      return mode;
    }
    
    Page * getPage(){
      return page;
    }
    
  private:
    Page * page;
    Goto * itm0_goto;
    IntItem * itm1_Chan;
    ListItem * itm2_Octave;
    IntItem * itm3_Vel;
    ListItem * itm4_Mode;
    const char * octaveList[OCTAVE_LIST_SIZE];
    const char * modeList[2];
    int16_t chan;
    int16_t octave;
    uint8_t note;
    uint8_t vel;
    uint8_t mode;
 };
 #endif
