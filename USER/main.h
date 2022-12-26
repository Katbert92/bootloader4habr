#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <string.h>

#include "stm32f10x.h"

//Standard Peripheral Lib
#include "stm32f10x_adc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_can.h"
#include "stm32f10x_cec.h"
#include "stm32f10x_crc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dbgmcu.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_fsmc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_sdio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_wwdg.h"
#include "misc.h"

//USB core
#include "usb_core.h"
#include "usb_def.h"
#include "usb_init.h"
#include "usb_int.h"
#include "usb_mem.h"
#include "usb_regs.h"
#include "usb_sil.h"
#include "usb_type.h"

//USB(MSD)
#include "mass_mal.h"
#include "memory.h"
#include "usb_bot.h"
#include "usb_conf.h"
#include "usb_desc.h"
#include "usb_hw_config.h"
#include "usb_istr.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "usb_scsi.h"

//USER
#include "hw_driver.h"
#include "leds.h"
#include "interrupts.h"
#include "ff.h"

#define DEBUG 0

#define SYSCLK_FREQ			72000000
#define TICK_1_KHz 			((SYSCLK_FREQ / 1000) - 1)
#define TICK_1_MHz 			((SYSCLK_FREQ / 1000000) - 1)

//Ports and pins

//USART1 (debug USART - CP2102)
#define USART1_CLK_PINS		RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO
#define USART1_CLK			RCC_APB2Periph_USART1
#define USART1_PORT			GPIOA
#define USART1_TX_PIN		GPIO_Pin_9
#define USART1_RX_PIN		GPIO_Pin_10
#define USART1_BAUDRATE		115200

//RGB LEDs
#define LED_RED_CLK			RCC_APB2Periph_GPIOB
#define LED_RED_PORT		GPIOB
#define LED_RED_PIN			GPIO_Pin_1

#define LED_GREEN_CLK		RCC_APB2Periph_GPIOB
#define LED_GREEN_PORT		GPIOB
#define LED_GREEN_PIN		GPIO_Pin_0

#define LED_BLUE_CLK		RCC_APB2Periph_GPIOA
#define LED_BLUE_PORT		GPIOA
#define LED_BLUE_PIN		GPIO_Pin_7

//Button pin
#define BUTTON_CLK			RCC_APB2Periph_GPIOB
#define BUTTON_PORT			GPIOB
#define BUTTON_PIN			GPIO_Pin_2

//USB disconnect pin
#define USB_DISCONNECT_CLK	RCC_APB2Periph_GPIOA
#define USB_DISCONNECT_PORT	GPIOA
#define USB_DISCONNECT_PIN	GPIO_Pin_15

#endif /*__MAIN_H */
