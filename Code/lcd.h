/* ---------- lcd.h ---------- */

#ifndef SRC_LCD_H_
#define SRC_LCD_H_

/* -- GPIO Ports -- */
// These #defines are used to easily change GPIO pins if needed
#define LCDD GPIOD	// Allows easy change of port for LCD if needed
#define RS  0x01	// RS is on GPIO PD0, therefore gets bit 1
#define RW  0x02	// RW is on GPIO PD1, therefore gets bit 2
#define E   0x04	// E is on GPIO PD2, therefore gets bit 3
#define DB4 0x10	// LCD DB4 is on GPIO PD4, therefore gets bit 5
#define DB5 0x20	// LCD DB5 is on GPIO PD5, therefore gets bit 6
#define DB6 0x40	// LCD DB6 is on GPIO PD6, therefore gets bit 7
#define DB7 0x80	// LCD DB7 is on GPIO PD7, therefore gets bit 8

/* -- Masks -- */
// These masks are used to only modify desired bits
#define DB_MASK (DB4 | DB5 | DB6 | DB7)	// Allows masking of DB ports
#define RS_RW_E_MASK (RS | RW | E)		// Allows masking of RS, RW, & E
#define E_MASK 0x04						// Allows masking of only E

/* -- Commands -- */
// These #defines correspond to hex values used by the ST7066U to process commands
#define NEW_LINE 0xC0					// 0b11000000 - new line command
#define CLEAR_HOME 0x01					// 0b00000001 - clear & go home command
#define DISPLAY_ON 0x0F					// 0b00001111 - turn display on
#define ENTRY_MODE_SET 0x06				// 0b00000110 - set entry mode

/* -- Function declarations / prototypes -- */
void LCD_init(void);		        		// Initialize LCD
void LCD_command(uint8_t);   				// Send LCD a single 8-bit command
void LCD_write_char(char); 					// Write a character to the LCD
void LCD_write_str(const char *str);		// Write a string to the LCD
void LCD_write_n_ctr_str(const char *str); 	// Write and center a string to LCD

#endif
