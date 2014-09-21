#ifndef	lcd_h
#define	lcd_h
/*-----------------HEADER FILES-------------------------------------
 -----------------------------------------------------------------*/
#include<string.h>   // sets up string functions - specifically strlen which is used later
#include<util/delay.h>  // sets up the use of _delay_ms  and _delay_us
#include<avr/interrupt.h>  // sets inturrupts for the external interrupt used to run the counter button

/*-------------CONNECTION BETWEEN LCD AND ATTINY2313-----------------
 This is where all the ports are labeled so the program can use named items
 vs showing raw code everywhere. The naming corresponds to the pin naming on the LCD
 -----------------------------------------------------------------*/
#define DATA_DDR	 DDRB			// this is where you will change the port if you are using a different AVR
#define DATA_PORT 	 PORTB

#define CONTROL_DDR	 	 DDRD       // this is where you will change the port if you are using a different AVR
#define CONTROL_PORT 	 PORTD
#define Enable_Pin		  6
#define RegSelect_Pin     4
#define ReadWrite_Pin     5
#define CONTROL_MASK     0X70

/*-------------------CONTROL BITS OF LCD --------------------------------
 This is basically just renaming everything to make it easy to work with
 -----------------------------------------------------------------------*/

#define Set_Enable  				CONTROL_PORT|=_BV(Enable_Pin)
#define Clear_Enable 				CONTROL_PORT&=~_BV(Enable_Pin)
#define Write_Lcd			    	CONTROL_PORT&=~_BV(ReadWrite_Pin)
#define Read_Lcd			   		CONTROL_PORT|=_BV(ReadWrite_Pin)
#define Select_InstructionRegister 	CONTROL_PORT&=~_BV(RegSelect_Pin)
#define Select_DataRegister	    	CONTROL_PORT|=_BV(RegSelect_Pin)
#define Data_Lcd(a)					DATA_PORT=a
#define delay(a)					_delay_ms(a)

/*-----------------FUNCTIONS---------------------------------------
 Initialize functions that will be used later on
 -----------------------------------------------------------------*/
void lcdInit(); // initializes port and LCD
void Init_Ports(void); // function sets up the ports using the naming defined above
void Init_Lcd(void); // function sets LCD into instruction mode and reads in all the preferences you have for the display
void delay_ms(unsigned char time_ms); // define delay -- TEST IF THIS IS NEEDED
void Lcd_Send(unsigned char a); // Function that actually pushes each letter into the LCD driver
void Set_Cursor(unsigned char a);
void Clear_LCD(void);
void test(void); //test function (was main function)

#endif  //lcd_h

