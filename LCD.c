/* 
 This program was modified from LCD code originally created by John Bibin. I'm not trying to claim any undeserved
 credit for the setup of the LCD code, but I did change quiet a bit and made it what it is.
 All the comments are my own.


 Author: Roznerd   http://roznerd.blogspot.com/
 AVR:  ATtiny2313   with internal clock set at 4MHz  and CLKDIV8 Fuse deactivated so delays are timely
 Compiler: AVRStudio 4.17 build 666 + WinAVR GCC 20090313
 LCD:  JHD162A 16x2 H44780 Character LCD on 5v used in 8 bit mode
 Size:  When burned to my ATtiny2313 it takes up 22% of my flash

 Purpose:

 Display "SCORE = 0" and using a momentary switch to gnd on ext. interrupt INT1
 to iterate the counter variable and refresh the screen very quickly so the display
 "instantly" shows the value of the variable

 When the counter reaches 10, Display "A Winner Is You!" for ~3-5 seconds and reset the counter and clear the display.

 Layout:  PORTB is setup to handle the 8 data pins of the LCD. Some of PORTD is handling the control pins of the LCD. The backlight is handled on the breadboard. INT1 is used to trigger the ISR for the counter.
 */
#include "LCD.h"

/*------------------------------------------------------------------
 ----------------TEST FUNCTION--------------------------------------
 -----------------------------------------------------------------*/
void test(void) {
	int len, i; 								// set up length variable and index for topstring
	char topstring[] = { "This is a test" }; 	// this is the string that will be written on the top row
	char bottomstring[] = { "This is line 2" };	// this is the string that will be written on the bottom row

	Init_Lcd(); 								// Run Init_Lcd function which sets up LCD with your preferences

	len = strlen(topstring); // measure the length of topstring using strlen() and store the into in len1 variable
	{ // lets write topstring to the LCD
		for (i = 0; i < len; i++) // the for loop steps through the string sending each letter
			Lcd_Send(topstring[i]); // the Lcd_Send function is run for the given letter of the topstring at index i
	}

	delay(50); // a quick delay in ms to allow LCD to display the value for a little bit before refreshing

	len = strlen(bottomstring); // measure length of the bottom string using strlen() from string.h

	Set_Cursor(0xC0); //set cursor to the beginning of the second line

	for (i = 0; i < len; i++) {
		Lcd_Send(bottomstring[i]); // Display the winner string on the LCD
	}
	delay(5000); // wait 5000ms so you can enjoy the feeling of victory

	Clear_LCD();
} // end test

/*----------------------------------------------------------------
 -----------------SEND A CHARACTER TO LCD-------------------------
 -----------------------------------------------------------------*/
void Lcd_Send(unsigned char a)
// by now you have see most of my comments regarding these lines of code because they were used above
// nothing really changes here except we have a function which has a character input, which means you
// feed in characters from your strings using a for loops and it will clock them into the LCD

{
	cli();					//disable interrupts
	Select_DataRegister;	// Sets R/S pin on LCD HI so the LCD knows we are giving it characters, not instructions
	Write_Lcd;				// R/W pin of LCD is set low so LCD knows we are writing to it, not reading from
	Data_Lcd(a);			// put ASCII hex code for the given letter/character out on PORTB for the LCD data pins. The ASCII hex codes are handled in the compiler, nice!
	Set_Enable;				// Sets the Enable pin HI on the LCD to clock in the data presented to the Data pins
	delay(1);				// delay 1 ms to let things settle
	Clear_Enable;			// Clears Enable by seting it back low.
	delay(1);				// delay 1 ms to let things settle
	sei();					// re-enable interrupts
}

void Set_Cursor(unsigned char position)
{
	/*
		Possible locations:
		-------------------
		   Top Display Row |80|81|82|83|84|85|86|87|88|89|8A|8B|8C|8D|8E|8F|
		Bottom Display Row |C0|C1|C2|C3|C4|C5|C6|C7|C8|C9|CA|CB|CC|CD|CE|CF|
	*/
	cli();						// Disable interrupts
	Select_InstructionRegister; // Sets R/S pin on LCD low so the LCD knows we are giving it instructions, not characters
	Write_Lcd;					// R/W pin of LCD is set low so LCD knows we are writing to it, not reading from
	Data_Lcd(position);			// have the AVR push a cursor location for the LCD out of PORTB to the LCD's data pins - this will move the cursor to the 88 position to overwrite previous number
	Set_Enable;					// Sets the Enable pin HI on the LCD to clock in the data presented to the Data pins
	delay(1);					// delay 1 ms to let things settle
	Clear_Enable;				// Clears Enable by seting it back low.
	delay(1);					// delay 1 ms to let things settle
	sei();						// re-enable interrupts
}

void Clear_LCD(void)
{
	cli();						// Disable interrupts
	Select_InstructionRegister;	// Sets R/S pin on LCD low so the LCD knows we are giving it instructions, not characters
	Write_Lcd;					// R/W pin of LCD is set low so LCD knows we are writing to it, not reading from
	Data_Lcd(0x01);				// this is the command for completely clearing the display and returning the cursor to the home location
	Set_Enable;					// Sets the Enable pin HI on the LCD to clock in the data presented to the Data pins
	delay(1);					// delay 1 ms to let things settle
	Clear_Enable;				// Clears Enable by seting it back low.
	delay(1);					// delay 1 ms to let things settle
	sei();						// re-enable interrupts
}

/*----------------------------------------------------------------
 -----------------FUNCTIONS TO INITIALIZE PORTS--------------------
 Initialize the naming for the ports and set up output register
 -----------------------------------------------------------------*/
void Init_Ports(void) {
	DATA_DDR = 0XFF; 			//Setting data port for output
	CONTROL_DDR = CONTROL_MASK; //setting selected pins of control port for output
	CONTROL_PORT &= ~(_BV(Enable_Pin) | _BV(RegSelect_Pin )
			| _BV(ReadWrite_Pin)); //setting values to 0 at starting
}

void lcdInit(void) {
	Init_Ports();	// configure the ports
	Init_Lcd();		// initialize the LCD
	delay(100);		// wait for the LCD to boot up
}

/*------------FUNCTION TO INITIATE LCD -----------------------------
 Initializes the LCD with the instruction commands we want to use for writing
 to the display. You can do alot here, but you want to avoid having to clear the
 display too frequenlty because it can cause flicker, its better over over write.
 There's alot of cool stuff you can mess with here.
 Check out these 2 sites for more instruction code
 http://www.dinceraydin.com/lcd/commands.htm
 http://ouwehand.net/~peter/lcd/lcd0.shtml
 -----------------------------------------------------------------*/
void Init_Lcd(void) {
	char init[4];
	int i;

	init[0] = 0x0C; // turn off cursor  - 0x0E blinking underline  - 0x0F blinking block
	init[1] = 0x38; //  Use LCD in 8bit mode with both lines of the display
	init[2] = 0x06; //  entry mode set - increment cursor position to write left to right with no character shifting
	init[3] = 0x80; //  home ddram position 0x80 - 0x8F
	//   Top Display Row |80|81|82|83|84|85|86|87|88|89|8A|8B|8C|8D|8E|8F|
	//Bottom Display Row |C0|C1|C2|C3|C4|C5|C6|C7|C8|C9|CA|CB|CC|CD|CE|CF|

	cli(); // disable interrupts
	Select_InstructionRegister;
	Write_Lcd;
	for (i = 0; i < 4; i++) // same story, set LCD in instruction mode and clock in command data
	{
		Data_Lcd(init[i]);
		Set_Enable;
		delay(1);
		Clear_Enable;
		delay(1);
	} // end for
	sei(); // re-enable interrupts
} //end function
