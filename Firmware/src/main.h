
#ifndef MAIN_H_
#define MAIN_H_

#include <SI_EFM8BB1_Register_Enums.h>                  // SFR declarations
#include <SI_EFM8BB1_Defs.h>
#include <stdint.h>

/***** COMPILATION VARIANTS *****/
//#define DEBUGUART
//#define DEBUG
#define PREVENTFLASHWRITE

#define BATMINVOLTAGE 3100
#define BATCRITICALVOLTAGE 3000

SI_SBIT(PIN_1, SFR_P1, 0); //P+
SI_SBIT(PIN_2, SFR_P1, 1); //P-

typedef enum {DISPLAY_EC,DISPLAY_CALIB,DISPLAY_CALIBNUMBLINK,DISPLAY_CALIBBLINK} DISPLAY_STATE;

typedef struct
{
	DISPLAY_STATE displaystate; // what is currently on display
	uint8_t calibselection; // what EC value to calibrate
	uint8_t calibblinkntr;
	uint8_t batcheckcntr;
	uint8_t holdstate; // 0 - no hold, 1 - hold
	uint8_t probereadcntr; // count intervals between probe read
	uint8_t probereadinterval;
} glob_t;

typedef struct
{
	int16_t ADCval; // calibration ADC value
	uint16_t divconst; // 0 - data predefined, 1 - valid calibration value
} calib_t;


extern glob_t glob;
#define CALIBRATIONVALUES 6
extern calib_t xdata calib_data[CALIBRATIONVALUES]; // calibration values 0 - 5


#endif /* MAIN_H_ */
