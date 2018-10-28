/**
*StepSequencer.h
*/
#ifndef STEP_SEQUENCER_H
#define STEP_SEQUENCER_H

#include "Arduino.h"
#include "SqncrButtonLED.h"
#include "Timer.h"
#include "Menu.h"
#include "SqncrConfig.h"
#include "Track.h"
#include "Button.h"
#define TICK_US(x)	(60000000/RSTEP/(x));
#define TIME_US(x)   (x  / tick_us)
#define SHOW_STP_ACTV_OFF 0
#define SHOW_STP_ACTV_ON 1

typedef struct{
	uint8_t actv;//step is on or off
	uint8_t dc;//how many beat fractions the note is on
	uint8_t note;//the midi note to play 1-127
	uint8_t vel; //velocity to play the note
	uint8_t note_on_cntr;//if the current counter matches then send the note!!
	uint8_t note_off_cntr;//if the current counter matched then send the note off!!
}Step_t;

class StepMenu : public Menu{
  friend class StepSequencer; 
  private:
  Page         pg0;
  IntItem      pg0Itm0_BPM;
  const char * pg0Itm1_BPM_ADJ_List[2];//{"C","F"}
  ListItem     pg0Itm1_BPM_ADJ;
  Track      * track[3];
  Goto       * gtoTrack[3];
  Goto       * gtoButton;
  Button     * button;
  public:
  struct{
    uint16_t bpm;
    uint8_t  bpm_adj;  
  }mdata;
  
  void update(uint16_t samples_[]){
    Menu::update();
    
    mdata.bpm = pg0Itm0_BPM.getVal();
    mdata.bpm_adj = pg0Itm1_BPM_ADJ.getVal();
    if(mdata.bpm_adj == 1){
      pg0Itm0_BPM.setIncrement(1);
    }else{
      pg0Itm0_BPM.setIncrement(10);
    }
    track[0]->update(samples_);
    track[1]->update(samples_);
    track[2]->update(samples_);
    button->update();
  }
  
  StepMenu() :
    pg0("MAIN"), 
    pg0Itm0_BPM("BPM",120,0,1000,1),
    pg0Itm1_BPM_ADJ_List{"C","F"},
    pg0Itm1_BPM_ADJ("BPM_ADJ",pg0Itm1_BPM_ADJ_List,2)
 {
    track[0] = new Track("Track1");
    track[1] = new Track("Track2");
    track[2] = new Track("Track3");
    track[0]->setChan(1);
    track[1]->setChan(2);
    track[2]->setChan(3);
    
    button  = new Button();
    
    gtoTrack[0] = new Goto(1,"Track1");
    gtoTrack[1] = new Goto(2,"Track2");
    gtoTrack[2] = new Goto(3,"Track3");
    gtoButton   = new Goto(4,"Button");
    
    pg0.addItem(&pg0Itm0_BPM);
    pg0.addItem(&pg0Itm1_BPM_ADJ);
    pg0.addItem(gtoTrack[0]);
    pg0.addItem(gtoTrack[1]);
    pg0.addItem(gtoTrack[2]);
    pg0.addItem(gtoButton);
    
    addPage(&pg0);
    addPage(track[0]->getPage());
    addPage(track[1]->getPage());
    addPage(track[2]->getPage());
    addPage(button->getPage());
  }
};

class StepSequencer {
public:
	/**constructor**/
	StepSequencer(uint16_t bpm_);
	
	/*
	*call play all the time
	*/
	void loop(void);
  void begin(void);
	void setBpm(uint16_t bpm);
  void toggleOnBrdLED(void);
  void readButtonISR(void);
  void readEncClkISR(void);
  void readEncSwISR(void);
	
private:
  char      cstr[255];
  uint8_t   snd;
  uint8_t   brdLED;
  uint8_t   potPins[NSTEP] = {A0,A1,A2,A3,A6,A7,A8,A9};//analog pins for the note settings
  uint16_t  samples[NSTEP];
	uint16_t 	cntr; //tracks each fractional step
  uint32_t  cntr32;//goes forever used for long notes!
	uint8_t 	curStep;
	uint16_t 	bpm;//0 to 255 bpm
  uint8_t   bpm_cf_adjust;//whether or not bpm adjustments are course or fine
	uint32_t  tick_us;//how many millis for current bpm 
  uint32_t  tick_tmr[2];//used to see how we are really doing;
	uint8_t		state;
	uint8_t		stateEntry;
	uint32_t	tmr[2]; //used to hold current and last call to millis;
  uint32_t  clk[2];//current master clock
  uint32_t  btnNoteOffCntr;
  uint8_t   btnNote;
  uint8_t   btnNoteOn;
  int16_t   btnModeResetRequestedAtStep;
  uint32_t  dT;
  uint8_t   showStepActv;
  uint8_t   lastTrackNote[3];
	Step_t    stp[NSTEP];
  void monBtn(void);//deals with the button events
  void sample(void);//read the analog inputs
  void doTick(void); //this gets called when its time to evaluate a note fraction i.e 1/RSTEP
  void writeMIDI(uint8_t cmd, uint8_t pitch, uint8_t velocity);
  void writeMIDI(uint8_t cmd, uint8_t pitch, uint8_t velocity,uint8_t chan);
  void showStep(void);
  void doBtnModeNormal();
  void doBtnModeSkip();
  SqncrButtonLED sbled;
  Timer   timer;
  StepMenu menu;
};

#endif
