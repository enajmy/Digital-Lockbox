/* ---------- led.c ---------- */

#include "main.h"
#include "led.h"

void LED_init(void) {

	// Initialize GPIOB and PIN 14
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);

	// Set pin 14 to output mode
	LED->MODER &= ~(GPIO_MODER_MODE14);
	LED->MODER |= (GPIO_MODER_MODE14_0);

	// Set pin 14 to PP
	LED->OTYPER &= ~(GPIO_OTYPER_OT14);

	// Set pin 14 to low speed
	LED->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED14);

	// Set pin 14 to no PUPD
	LED->PUPDR &= ~(GPIO_PUPDR_PUPD14);
}

// Turn LED on
void LED_on(void) {
	// LED3 has hex value 0x4000, giving a 1 at bit 15
	LED->ODR &= ~(LED3);
	LED->ODR |= (LED3);
}

// Turn LED off
void LED_off(void) {
	LED->ODR &= ~(LED3);
}
