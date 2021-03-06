/**
*Menu.cpp
*/
#include "Arduino.h"
#include "Menu.h"
#include "Encoder.h"
#include "Timer.h"

void Menu::begin(void){
  Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, MENU_I2C_ADR);
  enc.setup();
  reset();
  draw();
  timer.begin();
}

void Menu::reset(void){
  itmTopIdx = 0;//item at top of page
  itmSelIdx = 0;
  itmBotIdx = pages[pageIdx]->numberOfItems < MENU_WINDOW_LEN ? pages[pageIdx]->numberOfItems: MENU_WINDOW_LEN-1;//item at bottom of page
}

void Menu::update(void){
  //check to see if the encoder has moved;
  //or if the button has been pressed
  if(enc.getBtnDn()){
     enc.clrBtnDn();
     Serial.println("ButtonPress");
     //toggle the select active
     if(pages[pageIdx]->items[itmSelIdx]->isGoto == MENU_ITEM_IS_GOTO){
      //jump to the goto page
      jump();  
     }else{
      //toggle the select
      if(itmSel == MENU_ITEM_SEL_OFF){
          itmSel = MENU_ITEM_SEL_ON;
       }else{
          itmSel = MENU_ITEM_SEL_OFF;
       }
     }
     
     draw();
  }
  
  if(enc.encDatAvail()){
    if(itmSel == MENU_ITEM_SEL_OFF){
	    scroll();
	  }else{
      increment();
	  }
	  draw();
  }

  //do the timer
  if(timer.get(TIMER_IDX_TICK)> MENU_TICK_US){
      timer.reset(TIMER_IDX_TICK);  
  }
}
void Menu::jump(void){
  Goto * gptr = (Goto *)(pages[pageIdx]->items[itmSelIdx]);
  if(gptr->gotoPageIdx >=0 && gptr->gotoPageIdx < numberOfPages){
    pageIdx = gptr->gotoPageIdx;
    reset();
  }
}

void Menu::scroll(void){
  if(enc.getDir() == 1){
      //go down   
      if(itmSelIdx == itmBotIdx){
        itmBotIdx++;
        itmTopIdx++;
      }
      itmSelIdx++;        
    }else if(enc.getDir() == -1){
      //go up    
      if(itmSelIdx == itmTopIdx){
        itmTopIdx--;
        itmBotIdx--;
      }
      itmSelIdx--;
    }
    //check for wrapping
    if(itmTopIdx < 0){itmTopIdx = pages[pageIdx]->numberOfItems - 1;}
    if(itmTopIdx > (pages[pageIdx]->numberOfItems - 1)){itmTopIdx = 0;}
    if(itmBotIdx < 0){itmBotIdx = pages[pageIdx]->numberOfItems - 1;}
    if(itmBotIdx > (pages[pageIdx]->numberOfItems - 1)){itmBotIdx = 0;}
    if(itmSelIdx < 0){itmSelIdx = pages[pageIdx]->numberOfItems - 1;}
    if(itmSelIdx > (pages[pageIdx]->numberOfItems - 1)){itmSelIdx = 0;}
  
    sprintf(cstr,"%s,top:%d,sel:%d,bot:%d",enc.getDir()==1 ? "down":"up",itmTopIdx,itmSelIdx,itmBotIdx);
    Serial.println(cstr);
}

void Menu::increment(void){
    if(enc.getDir() > 0){
      pages[pageIdx]->items[itmSelIdx]->increment();
    }else{
      pages[pageIdx]->items[itmSelIdx]->decrement();
    }
}

void Menu::draw(void){
  clearDisplay();
  setTextSize(MENU_TITLE_TEXT_SIZE);
  setCursor(0,0);
  setTextColor(WHITE,BLACK);
  //print the page title
  println(pages[pageIdx]->title);
  setTextSize(MENU_ITEM_TEXT_SIZE);
  uint8_t idx = itmTopIdx;
  for(int ii = 0; ii < MENU_WINDOW_LEN; ii++){
    char str1[MENU_MAX_STRING_LEN];
    char str2[MENU_MAX_STRING_LEN];
    if(ii < pages[pageIdx]->numberOfItems){
      setCursor(0,MENU_TITLE_HEIGHT + ii*MENU_ITEM_HEIGHT);
      setTextColor(WHITE,BLACK);
      if(pages[pageIdx]->items[idx]->isGoto == 0){
        pages[pageIdx]->items[idx]->cstr(str2);
        snprintf(str1,MENU_MAX_STRING_LEN,"%s%s:%s",idx==itmSelIdx ? ">" : " ",pages[pageIdx]->items[idx]->str,str2);
        if((idx == itmSelIdx) && (itmSel == MENU_ITEM_SEL_ON)){
          setTextColor(BLACK, WHITE); //invert the text
        }
      }else{
        //this is a goto item
        snprintf(str1,MENU_MAX_STRING_LEN,"%s%s",idx==itmSelIdx ? ">" : " ",pages[pageIdx]->items[idx]->str);
      }
      
      //check for wrap
      idx++;
      if(idx > (pages[pageIdx]->numberOfItems-1)){
        idx = 0;
      }
      println(str1);
    }
  }
  display();
}

void Menu::readEncClkISR(void){
  enc.isrReadClkPin();
}

void Menu::readEncSwISR(void){
  enc.isrReadSwPin();
}
