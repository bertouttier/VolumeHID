#ifndef	encoder_h
#define	encoder_h
#include <avr/io.h>
//_________________________________________
//encoder port & pins
#define PORT_Enc 	PORTD 	
#define PIN_Enc 	PIND
#define DDR_Enc 	DDRD
#define Pin1_Enc 	4
#define Pin2_Enc 	5
#define Btn_Enc 	1
//______________________
#define RIGHT_SPIN 0x01 
#define LEFT_SPIN 0xff

void ENC_InitEncoder(void);
void ENC_PollEncoder(void);
unsigned char ENC_GetStateEncoder(void);
unsigned char ENC_GetBtnState(void);
#endif  //encoder_h
