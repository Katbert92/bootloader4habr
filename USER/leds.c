#include "leds.h"

void LED_RGB_SetBrightness(u32 color, u32 brightness)
{
	if(brightness >= 100)
	{
		brightness = 100;
	}

	brightness = 100 - brightness;

	switch(color)
	{
		case LED_RED:
			TIM3->CCR4 = brightness;
			break;

		case LED_GREEN:
			TIM3->CCR3 = brightness;
			break;

		case LED_BLUE:
			TIM3->CCR2 = brightness;
			break;

		default:
			break;
	}
}

void LED_RGB_EnableColors(void)
{
	TIM3 -> CCR2 = 0;
	TIM3 -> CCR3 = 0;
	TIM3 -> CCR4 = 0;
}

void LED_RGB_DisableColors(void)
{
	TIM3 -> CCR2 = 100;
	TIM3 -> CCR3 = 100;
	TIM3 -> CCR4 = 100;
}
