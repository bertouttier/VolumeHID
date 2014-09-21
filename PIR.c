#include "PIR.h"

void pirInit(void)
{
	pirState = 0;

	DDRD = 1<<PD3;		// Set PD3 as input (Using for interupt INT1)
	PORTD = 1<<PD3;		// todo:Enable PD3 pull-up resistor

	PCMSK |= (1 << PIND2); // This tells the external interrupt which input we are looking at.

	GIMSK |= (1 << INT1);			// Enable INT1
	MCUCR = 1<<ISC11 | 1<<ISC10;	// todo:Trigger INT1 on rising edge
	/*
	|-------|-------|-----------------------------------------------------------|
	|ISC11	|ISC10	|Description												|
	|-------|-------|-----------------------------------------------------------|
	|0		|0		|The low level of INT1 generates an interrupt request.		|
	|0		|1		|Any logical change on INT1 generates an interrupt request.	|
	|1		|0		|The falling edge of INT1 generates an interrupt request.	|
	|1		|1		|The rising edge of INT1 generates an interrupt request.	|
	|-------|-------|-----------------------------------------------------------|
	*/
}

/*-----------INTRRUPT SERVICE ROUTINE (ISR) FOR INT1 -----------------------------
 When the INT1 interrupt is triggered by your PIR sensor, the AVR jumps to this
 code no matter what its doing and in this case enables pirState
 --------------------------------------------------------------------------------*/

ISR(INT1_vect) // External Interrupt 0 ISR  Interrupt Service Routine
{
	_delay_ms(500); // Software debouncing control
	pirState = 1;
}
