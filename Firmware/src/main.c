//#pragma src
//=========================================================
// src/EFM8BB31F32I-C-QFP32_main.c: generated by Hardware Configurator
//
// This file will be updated when saving a document.
// leave the sections inside the "$[...]" comment tags alone
// or they will be overwritten!!
//=========================================================

// Button logic
// short press - fix/release values
// long press (5sec) - calibration. It will cycle from 0EC til 5EC. Release button when desired calibration is found. 
//  Then insert the probe into calibration liquid and wait until it will say, "OK".

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "main.h"
#include "InitDevice.h"
#include "gen.h"
#include "i2c.h"
#include "SSD1306.h"
#include "bat.h"
#include "probe.h"
#include "button.h"
#include "pgm.h"

volatile glob_t glob;
volatile calib_t xdata calib_data[6];

//-----------------------------------------------------------------------------
// SiLabs_Startup() Routine
// ----------------------------------------------------------------------------
// This function is called immediately after reset, before the initialization
// code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
// useful place to disable the watchdog timer, which is enable by default
// and may trigger before main() in some instances.
//-----------------------------------------------------------------------------
void SiLabs_Startup(void) {
	// $[SiLabs Startup]
	// [SiLabs Startup]$
}

//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
int main(void) {
  // Call hardware initialization routine
  int16_t bat;
  enter_DefaultMode_from_RESET();

  buttonstate = BUT_NOTPRESSED;
  glob.displaystate = DISPLAY_EC;
  glob.calibselection = 0;
  glob.calibblinkntr = 0;
  glob.batcheckcntr = 0;
  glob.holdstate = 0;

#ifdef DEBUGUART
	prnUART("START",1);
#endif

  delay_ms(80); // need for ssd1306 init

  loadSettingsEE();

  ssd1306_init();
  ssd1306_clear_display();
  ssd1306_send_command(SSD1306_DISPLAYON);
  //ssd1306_printBitmap(0,1,57,2,calib_bitmap);

  // check battery at startup
  bat = getBatVoltageMv();
  if(bat<BATMINVOLTAGE){
      // do we at minimal or critical voltage?
      if(bat<BATCRITICALVOLTAGE){
          ssd1306_printBitmap(1,1,126,2,batCritical_bitmap);
          while(1); // endless loop here as battery is critical
      }else{
          ssd1306_printBitmap(20,1,87,2,batLow_bitmap);
          delay_ms(3000);
      }
  }

	while(1){
	    int16_t probe;
	    uint8_t but = getButtonState();
	    
		if(glob.holdstate==0) probe = GetProbeADC();
		// check battery every minute
	    if(glob.batcheckcntr>60){
	        if(getBatVoltageMv()<BATMINVOLTAGE){
	            // reset device to show warning
	            RSTSRC=RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET;
	        }
	    }

	    // check button
	    if(but==1){
	        // enter calibration mode
          if(glob.displaystate==DISPLAY_CALIB) glob.displaystate=DISPLAY_CALIBNUMBLINK; else glob.displaystate=DISPLAY_CALIB;
          ssd1306_clear_display();
          ssd1306_printBitmap(0,1,57,2,calib_bitmap);
          if(glob.displaystate==DISPLAY_CALIB) ssd1306_printNumber(glob.calibselection*100);
          delay_ms(250);
      }else if(but==3){
          uint8_t i;
          // begin calibration with selected value
          for(i=0;i<20;i++){
            if(glob.displaystate==DISPLAY_CALIB) glob.displaystate=DISPLAY_CALIBBLINK; else glob.displaystate=DISPLAY_CALIB;
            ssd1306_clear_display();
            if(glob.displaystate==DISPLAY_CALIB) ssd1306_printBitmap(0,1,57,2,calib_bitmap);
            ssd1306_printNumber(glob.calibselection*100);
            delay_ms(500);
          }
      }else if(but==2){
          // freeze/unfreeze display
		  if(glob.holdstate==0){
			  glob.holdstate=1;
			  ssd1306_printBitmap(0,0,24,1,hold_bitmap);
			  
		  }else{
			  glob.holdstate=0;
			  ssd1306_printBitmapClear(0,0,24,1);
		  }
      }else{
        if(glob.displaystate!=DISPLAY_EC) ssd1306_clear_display();
          glob.displaystate=DISPLAY_EC;
          ssd1306_printBitmap(0,1,29,3,EC_bitmap);
          ssd1306_printNumber(probe);
          delay_ms(1000);
        }

	}

}
