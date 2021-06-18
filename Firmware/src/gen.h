#ifndef GEN_H_
#define GEN_H_

#include "main.h"

extern uint16_t tmp_millis;
extern bit delay_on;

void delay_ms(uint16_t ms_count);
void pinsHighZ(void);
void pinsPushPull(void);

#endif /* GEN_H_ */
