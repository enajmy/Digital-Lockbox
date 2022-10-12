/* ---------- lcd.c ---------- */
#include "main.h"
#include "lcd.h"
#include "delay.h"

void LCD_init(void) {						// Initialize LCD function

	/* -- Configure GPIO D -- */
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIODEN);	// Enable GPIO D Clock

	LCDD->MODER &= ~(GPIO_MODER_MODE0		// Mask MODER[0:2,4:7]
			| GPIO_MODER_MODE1
			| GPIO_MODER_MODE2
			| GPIO_MODER_MODE4
			| GPIO_MODER_MODE5
			| GPIO_MODER_MODE6
			| GPIO_MODER_MODE7);

	LCDD->MODER |= (GPIO_MODER_MODE0_0		// Set MODER[0:2,4:7][1] to one (output mode)
			| GPIO_MODER_MODE1_0
			| GPIO_MODER_MODE2_0
			| GPIO_MODER_MODE4_0
			| GPIO_MODER_MODE5_0
			| GPIO_MODER_MODE6_0
			| GPIO_MODER_MODE7_0);

	LCDD->OTYPER &= ~(GPIO_OTYPER_OT0		// Set OTYPER[0:2,4:7] to zero (push-pull)
			| GPIO_OTYPER_OT1
			| GPIO_OTYPER_OT2
			| GPIO_OTYPER_OT4
			| GPIO_OTYPER_OT5
			| GPIO_OTYPER_OT6
			| GPIO_OTYPER_OT7);

	LCDD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0	// Set OSPEEDR[0:2,4:7] to zero (low speed)
			| GPIO_OSPEEDR_OSPEED1
			| GPIO_OSPEEDR_OSPEED2
			| GPIO_OSPEEDR_OSPEED4
			| GPIO_OSPEEDR_OSPEED5
			| GPIO_OSPEEDR_OSPEED6
			| GPIO_OSPEEDR_OSPEED7);

	LCDD->PUPDR &= ~(GPIO_PUPDR_PUPD0		// Set PUPDR[O:2,4:7] to zero (no PU/PD resistor)
			| GPIO_PUPDR_PUPD1
			| GPIO_PUPDR_PUPD2
			| GPIO_PUPDR_PUPD4
			| GPIO_PUPDR_PUPD5
			| GPIO_PUPDR_PUPD6
			| GPIO_PUPDR_PUPD7);

	/* -- Begin LCD Initialization Process -- */
	SysTick_Init();

	delay_us(40000);						// Initial 40ms delay
	LCD_command(0x03);						// Set RS & R/W to zero (PD0 & PD1)
	delay_us(37);							// Delay 37 us
	LCD_command(0x28);						// Set font and line number
	delay_us(37);							// Delay 37 us
	LCD_command(0x28);						// Set font and line number, again
	delay_us(37);							// Delay 37 us
	LCD_command(DISPLAY_ON);				// Turn display, cursor, and blink on
	delay_us(37);							// Delay 37 us
	LCD_command(CLEAR_HOME);				// Clear display, return home
	delay_us(1520);							// Delay 1.52 ms
	LCD_command(ENTRY_MODE_SET);			// Set mode to increment
}

void LCD_command(uint8_t command) {  		// Send LCD a single 8-bit command
	uint8_t hiNibble, loNibble;				// Initialize variables for upper 4 bits and lower 4 bits

	SysTick_Init();							// Initialize delay function

	hiNibble = (command & 0xF0);			// Split command into hiNibble
	loNibble = ((command & 0x0F) << 4);		// Split command into loNibble
		/* Bits 4-7 are used (in F0 and with the shift left by 4)
		 * due to use of GPIO PD4-7, thus making outputting easier
		 */

	LCDD->ODR &= ~(RS_RW_E_MASK);			// Mask RS, R/W, and E to zero (PD0, PD1, PD2)
	LCDD->ODR &= ~(DB_MASK);				// Mask DB4-7 to zero (PD4-7)
	LCDD->ODR |= hiNibble;					// Set DB4-7 to hiNibble
	LCDD->ODR &= ~(E_MASK);					// Mask E to zero (PD2)
	LCDD->ODR |= E;							// Set E to one
	delay_us(1);							// Delay > 460ns
	LCDD->ODR &= ~(E_MASK);					// Mask E to zero
	LCDD->ODR &= ~(DB_MASK);				// Mask DB4-7 to zero
	LCDD->ODR |= loNibble;					// Set DB4-7 to loNibble
	LCDD->ODR &= ~(E_MASK);					// Mask E to zero
	LCDD->ODR |= E;							// Set E to one
	delay_us(1);							// Delay > 460ns
	LCDD->ODR &= ~(E_MASK);					// Mask E to zero

	/* If statement to check timing, only two commands have timing
	 * different than 37 us, and those two commands have a HEX value
	 * equal or less than 2 */
	if (command > 0x02){
		delay_us(37);		// Delay by 37 us
	} else {
		delay_us(1520);		// Delay by 1.52 ms
	}
}

void LCD_write_char(char letter) { 		// Write a character to the LCD
	uint8_t hiNibble, loNibble;			// Initialize variables for upper and lower 4 bits

	SysTick_Init();						// Initialize delay function

	hiNibble = (letter & 0xF0);			// Split command into hiNibble
	loNibble = ((letter & 0x0F) << 4);	// Split command into loNibble

	LCDD->ODR &= ~(RS_RW_E_MASK);		// Mask RS, RW, and E to zero
	LCDD->ODR |= (RS);					// Set RS to 1, allowing for writing
	LCDD->ODR &= ~(DB_MASK);			// Mask only data pins to zero
	LCDD->ODR |= hiNibble;				// Set only data pins to hiNibble
	LCDD->ODR &= ~(E_MASK);				// Mask enable (E) to zero
	LCDD->ODR |= E;						// Set E high
	delay_us(1);						// Delay > 460ns
	LCDD->ODR &= ~(E_MASK);				// Set E to zero
	LCDD->ODR &= ~(DB_MASK);			// Mask only data pins to zero
	LCDD->ODR |= loNibble;				// Set only data pins to loNibble
	LCDD->ODR &= ~(E_MASK);				// Mask E to zero
	LCDD->ODR |= E;						// Set E to one
	delay_us(1);						// Delay > 460ns
	LCDD->ODR &= ~(E_MASK);				// Set E to zero
	delay_us(37);						// Delay 37 ns
}

void LCD_write_str(const char *str){
	uint8_t i = 0;			 	// Initialize variables

	/* The following while statement indexes through the string that was inputed
	 * starting at bit 0, printing each character until the index is greater than
	 * the number of characters found to be on the first line, it will then move
	 * to line 2 and begin printing the rest of the characters.
	 */
	while (str[i] != '\0'){
		LCD_write_char(str[i++]);
	}
}

// THIS FUNCTION IS MEANT FOR PERSONAL USE AND NOT MEANT FOR GRADING / SUBMISSION
void LCD_write_n_ctr_str(const char *str){	// Function to write and center a string to LCD

	uint8_t i = 0, str_length = 0,	// Variable initialization, 'i' used for looping,
		line_num = 1,				// 'line_num' to keep track of what line LCD is on
		str_lengthL1 = 15,			// 'str_lengthL1' - determine length of characters on line 1
		str_lengthL2 = 31,			// 'str_lengthL2' - determine length of characters on line 2
		line1center = 0,			// 'line1center' - track spaces needed to center line 1
		line2center = 0;			// 'line2center' - track spaces needed to center line 2

	LCD_command(CLEAR_HOME);		// Clear LCD before writing just to ensure proper display

	/* The below while loop is used to ensure words don't get split in the middle and how to center
	 * the lines. It begins by checking the overall string length. If a string was passed in,
	 * continue with code, otherwise return to main. Next, it will check if str_length > 15,
	 * meaning if more than one line on the LCD will be used. If so, calculate total length
	 * of line 1 and print spaces to center it. Next, it will do the same for line 2 of the LCD,
	 * beginning from bit 31 (last slot on LCD). If only one line is used, instead of checking
	 * for a space between words, it will simply use the string length to center the line.
	 */
	while (str[str_length] != '\0')		// Calculate total string length
		str_length++;

	if (str_length == 0)				// If no string was passed, return
		return;

	if (str_length > 15) {				// If more than one line on LCD will be needed, continue...

		/* This while loop checks where a space between characters occurs in order
		 * to know where to split the lines. It also ensures that str_lengthL1
		 * remains positive and does not enter an infinite loop in the event a
		 * string with no spaces is passed in.
		 */
		while(str[str_lengthL1] != ' ' && str_lengthL1 > 0) {
			str_lengthL1 -= 1;
		}

		/* If str_lengthL1 = 0, the string passed in is continuous and has no spaces,
		 * therefore no need to try to center. If str_lengthL1 != 0, then the string
		 * needs to be split.
		 */
		if (str_lengthL1 != 0) {

			/* str_lengthL1 gives us total number of characters that will be printed on
			 * line 1. Subtract it from 16 and divide by 2 to determine how much space
			 * is needed to be printed first to center.
			 */
			line1center = (16 - str_lengthL1) / 2;
			while (line1center != 0) {
				LCD_write_char(' ');
				line1center--;
			}
			/* str_lengthL2 does the same as str_lengthL1, just beginning at bit 31 of
			 * the string and decrementing until there is text to determine how many bits
			 * come after the end of the string.
			 */
			while(str[str_lengthL2] == '\0') {
				str_lengthL2 -= 1;
			}
			/* Same calculation, just subtracting str_lengthL1 to determine where to begin
			 * after the separation
			 */
			line2center = (16 - (str_lengthL2 - str_lengthL1)) / 2;
		}

		/* This else is used if the string is only one line and used str_length instead
		 * of str_lengthL1.
		 */
	} else {

		line1center = (16 - str_length) / 2;
		while (line1center != 0) {
			LCD_write_char(' ');
			line1center--;
		}
	}

	/* The below while loop is used to print a string onto an LCD using by printing its
	 * individual characters. It begins by indexing through the string, checking for
	 * where it terminates (thus the '\0' check). As it indexes, it calls the LCD_write_char
	 * function and prints each individual character. It will also split the string where
	 * needed. It checks to see if 'i' has exceeded 'str_lengthL1' (mentioned above) and if the
	 * display is already on line 2 of the LCD or not. If it has exceeded 'str_lengthL1' and on
	 * line 1, it will then tell the LCD to move to the 2nd line. It also ensures
	 * 'str_lengthL1 != 0'. If str_legnthL1 = 0, then the string is continuous and will simply
	 * need to change to line 2 when i = 16, otherwise if str_lengthL1 != 0, then it will use the
	 * calculated spot to split the string.
	 */
	while(str[i] != '\0') {
		LCD_write_char(str[i++]);

		if (str_lengthL1 != 0) {

			if (i > str_lengthL1 && line_num != 2) {
				LCD_command(NEW_LINE);
				line_num = 2;

				while (line2center != 0 && line_num == 2) {
					LCD_write_char(' ');
					line2center--;
				}
			}
		} else {
			if (i == 16)
				LCD_command(NEW_LINE);
		}
	}
}

