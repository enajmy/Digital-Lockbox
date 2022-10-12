/* ---------- led.h ---------- */

#ifndef SRC_LED_H_
#define SRC_LED_H_


// Include Pound Defines
#define LED GPIOB	// Define LED as GPIOB
#define LED3 0x4000 // Define LED3 as Pin 15

// Include function definitions / prototypes
void LED_init(void);
void LED_on(void);
void LED_off(void);

#endif
