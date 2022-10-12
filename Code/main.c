// EE 329 - 01
// Authors: Ethan Najmy
// Project #1 - Digital Lockbox

/* ----------- main.c ------------ */

// Include header files
#include "main.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"
#include "led.h"

// Initialize local function
void SystemClock_Config(void);

int main(void) {
	/* Create the states and a new variable type called 'State_Type'
	 * The states include ST_INIT, ST_LOCKED, ST_UNLOCKED, ST_WRONG, and ST_NEW_PIN
	 * The states begin in ST_INIT
	 */
	typedef enum {ST_INIT, ST_LOCKED, ST_UNLOCKED, ST_WRONG, ST_NEW_PIN} State_Type;
	State_Type state = ST_INIT;

	// Declare variables, DEFAULT_PIN = 1234 and can be changed
	uint8_t button, count;
	uint16_t DEFAULT_PIN = 1234, pin;

	// Initialize System Functions
	HAL_Init();
	SystemClock_Config();

	// Call functions to initialize LCD, keypad, and LED
	LCD_init();
	keypad_init();
	LED_init();

	// Enter FSM in infinite while loop
	while (1) {
		// Switch statement to jump between states
		switch(state) {

		// Inital State - runs on startup, resets system
		case ST_INIT:
			LCD_command(CLEAR_HOME);			// Clear LCD
			LED_on();							// Turn on locking indicator
			LCD_write_str("SYSTEM LOCKED.");	// Print
			LCD_command(NEW_LINE);				// New line to not cut off text
			LCD_write_str("ENTER PIN: ");		// Print
			state = ST_LOCKED;					// Set state to ST_LOCKED
			pin = 0;							// Reset pin variable
			count = 0;							// Reset count variable
			break;								// Break out of this state, will go to ST_LOCKED

		// Locked State
		case ST_LOCKED:
			// Read button continuously and wait for a press
			button = keypad_read();
			while (button == KEYPAD_NO_PRESS) {
				button = keypad_read();
			}

			// If star was pressed, reset system and clear screen
			if (button == KEYPAD_STAR) {
				state = ST_INIT;
				break;
			}

			// If a digit was pressed print the digit
			if (button <= 9) {
				LCD_write_char(convertNum(button));	// Write the char after converting the to ASCII
				pin += button;						// Add to pin with new button value
				count++;							// Increment count
				if (count < 4)					// Multiply if 3 or less digits have been entered
					pin *= 10;					// Allows for the int to be shifted hundreds places
			}

			// If pound was pressed and pin entered matches, UNLOCK
			// If pound was pressed and pin doesnt match, WRONG
			if (button == KEYPAD_POUND){
				if ((pin == DEFAULT_PIN) && (count >= 1)) {
					state = ST_UNLOCKED;
					break;
				} else {
					state = ST_WRONG;
					break;
				}
			}
			break;

		// UNLOCKED State - will shut off LED and print.
		case ST_UNLOCKED:
			LED_off();
			LCD_command(CLEAR_HOME);
			LCD_write_str("UNLOCKED");
			LCD_command(NEW_LINE);
			LCD_write_str("*-lock/#-new pin");
			button = keypad_read();

			// Wait for button to be pressed
			while (button == KEYPAD_NO_PRESS)
				button = keypad_read();

			// If the button pressed was star, lock back up
			if (button == KEYPAD_STAR) {
				state = ST_INIT;
				break;

			// If pound was pressed, go to NEW_PIN state
			} else if (button == KEYPAD_POUND){
				state = ST_NEW_PIN;
				break;
			// If nothing pressed, break and repeat
			} else {
				break;
			}

		// WRONG State - will print saying wrong pin and wait for #
		case ST_WRONG:
			LCD_command(CLEAR_HOME);
			LCD_write_str("WRONG PIN.");
			LCD_command(NEW_LINE);
			LCD_write_str("TRY AGAIN.");

			// Wait for # to be pressed so the user can acknowledge wrong pin
			button = keypad_read();
			while (button != KEYPAD_POUND){
				button = keypad_read();
			}
			// When # is pressed, go back to INIT state
			state = ST_INIT;
			break;

		// NEW_PIN State - user can enter new pin
		case ST_NEW_PIN:
			// Reset pin and count variables
			pin = 0;
			count = 0;

			// Clear LCD and print
			LCD_command(CLEAR_HOME);
			LCD_write_str("ENTER NEW 4");
			LCD_command(NEW_LINE);
			LCD_write_str("DIGIT PIN: ");

			// Same process as printing to LCD in ST_LOCKED
			while (1) {
				button = keypad_read();
				if (button <= 9) {
					LCD_write_char(convertNum(button));
					pin += button;
					count++;
					if (count < 4)
						pin *= 10;
				}

				// If # is pressed and 4 digits have been entered,
				// set the entered pin as the default pin and go to INIT
				if ((button == KEYPAD_POUND) && (count == 4)) {
					DEFAULT_PIN = pin;
					state = ST_INIT;
					break;
				}
				// If * pressed, reset NEW_PIN state and clear LCD
				if (button == KEYPAD_STAR)
					break;
			}

		}

	}

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
