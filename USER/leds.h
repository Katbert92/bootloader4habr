#ifndef __LEDS_H
#define __LEDS_H

#include "main.h"

#define LED_RED		0
#define LED_GREEN	1
#define LED_BLUE	2

void LED_RGB_SetBrightness(u32 color, u32 brightness);
void LED_RGB_EnableColors(void);
void LED_RGB_DisableColors(void);

#endif /*__LEDS_H */
