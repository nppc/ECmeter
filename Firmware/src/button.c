//#pragma src
// Button routines
#include "button.h"

volatile int16_t buttoncntr;
volatile BUT_STATE buttonstate;

uint8_t getButtonState(void){
	uint8_t tmp = buttonstate;
	if(buttonstate==BUT_PRESSED){
		// button is pressed
		if(buttoncntr >= 161) {
			glob.calibselection = (buttoncntr-161)/64;
		}else{
		  glob.calibselection = 0;
			return BUT_NOTPRESSED;
		}
		return BUT_PRESSED;
	}
	buttonstate = BUT_NOTPRESSED;
	return tmp;
}
