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
 ----------------MAIN FUNCTION--------------------------------------
 -----------------------------------------------------------------*/
void test(void) {
	delay(100);
	// Wait for LCD to boot up
	int len1, i; // set up length variable and index for topstring
	int len2, j; // set up length variable and index for variable string
	int len3, k; // set up length variable and index for winner string

	counter = 0; // set initial value of counter

	char variable[2] = { 00 }; // initialize variable as a character with enough space to hold the digits we will use

	Init_Ports(); // Run the Init_Ports function to set up the avr for talking to the LCD

	//---------------------------------MAIN LOOP---------------------------------------------
	while (1) // sets up a forever loop
	{

		itoa(counter, variable, 10); // integer to string conversion, takes value of integer "counter" and converts it to a string and places it in "varialbe" char  using base 10 numbering system
									 // this is the secret to getting variable values to display on the LCD.
									 // everything has to be converted into a char string so it can be sent to the LCD as ASCII characters

		char topstring[] = { "SCORE = " }; // this is the string that will be written on the top row along with the variable
		char winner[] = { "A Winner Is You!" }; /// this is the string that is written when the counter reaches 10

		cli(); // globally disable interrupts - this is done when important events are occuring and you don't want to get interrupted during them
		Init_Lcd(); // Run Init_Lcd function which sets up LCD with your preferences
		sei(); // globally enables the interrupts

		len1 = strlen(topstring); // measure the length of topstring using strlen() and store the into in len1 variable
		{ // lets write topstring to the LCD
			for (i = 0; i < len1; i++) // the for loop steps through the string sending each letter
				Lcd_Send(topstring[i]); // the Lcd_Send function is run for the given letter of the topstring at index i
		}

		len2 = strlen(variable); // measure the length of the "variable" string using strlen() function from string.h
		{
			for (j = 0; j < len2; j++) // the for loop steps through the string sending each letter
				Lcd_Send(variable[j]); // the Lcd_Send function is run for the given letter of the "varialbe" string at index j
		}

		delay(50); // a quick delay in ms to allow LCD to display the value for a little bit before refreshing

		if (counter == 10) // checks if the counter has reached 10 which would indicate you have won
				{
			// this section is done to write the varialbe value of 10 for the score - without this, the SCORE will reach 9, then as soon as you click it
			// tenth time it wi display that you've won, but the ISR happens so fast that it doesn't update the LCD
			// alot of this will be described better in the Lcd_Send function

			cli(); // globally disable interrupts - this is done when important events are occuring and you don't want to get interrupted during them
			Select_InstructionRegister; // Sets R/S pin on LCD low so the LCD knows we are giving it instructions, not characters
			Write_Lcd; // R/W pin of LCD is set low so LCD knows we are writing to it, not reading from
			Data_Lcd(0x88); // have the AVR push a cursor location for the LCD out of PORTB to the LCD's data pins - this will move the cursor to the 88 position to overwrite previous number
			Set_Enable; // Sets the Enable pin HI on the LCD to clock in the data presented to the Data pins
			delay(1); // delay 1 ms to let things settle
			Clear_Enable; // Clears Enable by seting it back low.
			delay(1); // delay 1 ms to let things settle

			len2 = strlen(variable); // again the length of the varialbe string
			itoa(counter, variable, 10); // convert the couter to a string
			for (j = 0; j < len2 + 1; j++) {
				Lcd_Send(variable[j]); // display on the LCD the latest value of the counter, by overwriting the 9 that used to be there since we moved the cursor to do so
			}

			len3 = strlen(winner); // measure length of the winner string using strlen() from string.h

			Select_InstructionRegister; // Sets R/S pin on LCD low so the LCD knows we are giving it instructions, not characters
			Write_Lcd; // R/W pin of LCD is set low so LCD knows we are writing to it, not reading from
			Data_Lcd(0xC0); // have the AVR push a cursor location for the LCD out of PORTB to the LCD's data pins - this will move the cursor to the the 1st location on the 2nd line of the display
			Set_Enable; // Sets the Enable pin HI on the LCD to clock in the data presented to the Data pins
			delay(1); // delay 1 ms to let things settle
			Clear_Enable; // Clears Enable by seting it back low.
			delay(1); // delay 1 ms to let things settle

			for (k = 0; k < len3; k++) {
				Lcd_Send(winner[k]); // Display the winner string on the LCD
			}
			delay(5000); // wait 5000ms so you can enjoy the feeling of victory

			Select_InstructionRegister; // see other similar sections for comments, it's getting redundant
			Write_Lcd;
			Data_Lcd(0x01); //  this is the command for completely clearing the display and returning the cursor to the home location
			Set_Enable;
			delay(1);
			Clear_Enable;
			delay(1);

			counter = 0; // reset counter to 0
			sei(); // this actually globally enables the interrupts, GIMSK just turned them on
		} // end if
	} // end while
} // end main

/*----------------------------------------------------------------
 -----------------SEND A CHARACTER TO LCD-------------------------
 -----------------------------------------------------------------*/
void Lcd_Send(unsigned char a)
// by now you have see most of my comments regarding these lines of code becuase they were used above
// nothing really changes here except we have a function which has a character input, which means you
// feed in characters from your strings using a for loops and it will clock them into the LCD

{
	cli(); //disable interrupts
	Select_DataRegister; // Sets R/S pin on LCD HI so the LCD knows we are giving it characters, not instructions
	Write_Lcd; // R/W pin of LCD is set low so LCD knows we are writing to it, not reading from
	Data_Lcd(a); // put ASCII hex code for the given letter/character out on PORTB for the LCD data pins. The ASCII hex codes are handled in the compiler, nice!
	Set_Enable; // Sets the Enable pin HI on the LCD to clock in the data presented to the Data pins
	delay(1); // delay 1 ms to let things settle
	Clear_Enable; // Clears Enable by seting it back low.
	delay(1); // delay 1 ms to let things settle
	sei(); // re-enable interrupts
}

/*----------------------------------------------------------------
 -----------------FUNCTIONS TO INITIALIZE PORTS--------------------
 Initialize the naming for the ports and set up output register
 -----------------------------------------------------------------*/
void Init_Ports(void) {
	DATA_DDR = 0XFF; //Setting data port for output
	CONTROL_DDR = CONTROL_MASK; //setting selected pins of control port for output
	CONTROL_PORT &= ~(_BV(Enable_Pin) | _BV(RegSelect_Pin )
			| _BV(ReadWrite_Pin)); //setting values to 0 at starting
}

/*----------------------------------------------------------------
 ------------FUNCTION TO INITIATE LCD -----------------------------
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
	// 0x01 is clear display, but I'm avoiding that and just overwriting until the user is a "winner" see line 178
	init[0] = 0x0C; // turn off cursor  - 0x0E blinking underline  - 0x0F blinking block
	init[1] = 0x38; //  Use LCD in 8bit mode with both lines of the display
	init[2] = 0x06; //  entry mode set - increment cursor position to write left to right with no character shifting
	init[3] = 0x80; //  home ddram position 0x80 - 0x8F
	//   Top Display Row |80|81|82|83|84|85|86|87|88|89|8A|8B|8C|8D|8E|8F|
	//Bottom Display Row |C0|C1|C2|C3|C4|C5|C6|C7|C8|C9|CA|CB|CC|CD|CE|CF|
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

} //end function

/*-------------------------------------------------------------------------------
 ------------INTRRUPT SERVICE ROUTINE (ISR) FOR INT1 -----------------------------
 When the INT1 interrupt is triggered by your momentary switch, the AVR jumps to this
 code no matter what its doing and in this case iterates the global counter value
 --------------------------------------------------------------------------------*/

//ISR(INT1_vect) // External Interrupt 0 ISR  Interrupt Service Routine
//{
//	counter = counter + 1; //iterate counter up one  this can be changed to -/or* too, and the increment #
//						   //can change, but the "variable" size might need to be increased to accomodate more digits
//	delay(1); // put in a quick 1ms delay
//	// the ISR is complete - return to your regularly scheduled program
//}
