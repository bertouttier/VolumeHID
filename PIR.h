#ifndef	pir_h
#define	pir_h

#include <avr/interrupt.h>  // sets inturrupts for the external interrupt used to run the counter button
#include <util/delay.h>  // sets up the use of _delay_ms  and _delay_us

volatile unsigned char pirState;

void pirInit(void);

#endif
