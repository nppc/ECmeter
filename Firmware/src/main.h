#include <SI_EFM8BB1_Register_Enums.h>                  // SFR declarations
#include <SI_EFM8BB1_Defs.h>
#include <stdint.h>

#ifndef MAIN_H_
#define MAIN_H_

/***** COMPILATION VARIANTS *****/
//#define DEBUGUART
//#define PREVENTFLASHWRITE

#define BATMINVOLTAGE 3100
#define BATCRITICALVOLTAGE 3000

SI_SBIT(PIN_1, SFR_P1, 0); //P+
SI_SBIT(PIN_2, SFR_P1, 1); //P-


#endif /* MAIN_H_ */
