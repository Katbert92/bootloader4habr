#include "hw_driver.h"

void CRC_Config(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

	CRC_ResetDR();
}

void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_Options;
	USART_InitTypeDef USART_Options;

	RCC_APB2PeriphClockCmd(USART1_CLK | USART1_CLK_PINS, ENABLE);

	GPIO_Options.GPIO_Pin = USART1_RX_PIN;
	GPIO_Options.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1_PORT, &GPIO_Options);

	GPIO_Options.GPIO_Pin = USART1_TX_PIN;
	GPIO_Options.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Options.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(USART1_PORT, &GPIO_Options);

	USART_Options.USART_BaudRate = USART1_BAUDRATE;
	USART_Options.USART_WordLength = USART_WordLength_8b;
	USART_Options.USART_StopBits = USART_StopBits_1;
	USART_Options.USART_Parity = USART_Parity_No;
	USART_Options.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Options.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_Options);

	USART_Cmd(USART1, ENABLE);
}

void LED_RGB_Config(void)
{
	GPIO_InitTypeDef GPIO_Options;
	TIM_TimeBaseInitTypeDef TIM_BaseOptions;
    TIM_OCInitTypeDef TIM_PWM_Options;

	//Clock configuration
	RCC_APB2PeriphClockCmd(LED_RED_CLK | LED_BLUE_CLK | LED_GREEN_CLK | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//PA.7 TIM3_CH2, BLUE
	GPIO_Options.GPIO_Pin = LED_BLUE_PIN;
	GPIO_Options.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Options.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(LED_BLUE_PORT, &GPIO_Options);

	//PB.01, TIM3_CH3, RED
	GPIO_Options.GPIO_Pin = LED_RED_PIN;
	GPIO_Options.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Options.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(LED_RED_PORT, &GPIO_Options);

	//PB.0, TIM3_CH4, GREEN
	GPIO_Options.GPIO_Pin = LED_GREEN_PIN;
	GPIO_Options.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Options.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(LED_GREEN_PORT, &GPIO_Options);

    //TIM3 configuration
    TIM_BaseOptions.TIM_Prescaler = TICK_1_MHz;	//clock prescaller
    TIM_BaseOptions.TIM_Period = 99; //100 - 1
    TIM_BaseOptions.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseOptions.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_BaseOptions);

    //TIM3 PWM1 Mode configuration
    TIM_PWM_Options.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_PWM_Options.TIM_OutputState = TIM_OutputState_Enable;
    TIM_PWM_Options.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_PWM_Options.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_PWM_Options.TIM_Pulse = 99; //100 - 1
    TIM_OC2Init(TIM3, &TIM_PWM_Options);
    TIM_OC3Init(TIM3, &TIM_PWM_Options);
    TIM_OC4Init(TIM3, &TIM_PWM_Options);

    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_Cmd(TIM3, ENABLE);

	TIM3 -> CCR2 = 100; //disable all colors
	TIM3 -> CCR3 = 100;
	TIM3 -> CCR4 = 100;
}

void Button_Config(void)
{
	GPIO_InitTypeDef GPIO_Options;

	RCC_APB2PeriphClockCmd(BUTTON_CLK, ENABLE);

	GPIO_Options.GPIO_Pin = BUTTON_PIN;
	GPIO_Options.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BUTTON_PORT, &GPIO_Options);
}

void USB_Config(void)
{
	GPIO_InitTypeDef GPIO_InitOptions;
	EXTI_InitTypeDef EXTI_InitOptions;

	//USB clock, prescaller & disconnect pin clock
	RCC_APB2PeriphClockCmd(USB_DISCONNECT_CLK | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	//Configure USB pull-up pin
	GPIO_InitOptions.GPIO_Pin = USB_DISCONNECT_PIN;
	GPIO_InitOptions.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitOptions.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(USB_DISCONNECT_PORT, &GPIO_InitOptions);

	GPIO_SetBits(USB_DISCONNECT_PORT, USB_DISCONNECT_PIN); //Disable USB

	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

	//Configure the EXTI line 18 connected internally to the USB IP
	EXTI_ClearITPendingBit(EXTI_Line18);
	EXTI_InitOptions.EXTI_Line = EXTI_Line18;
	EXTI_InitOptions.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitOptions.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitOptions.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitOptions);

	MAL_Init(0);
}

void Interrupts_Config(void)
{
	NVIC_InitTypeDef NVIC_Options;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //2 bit for pre-emption priority, 2 bits for subpriority

	NVIC_Options.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
	NVIC_Options.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Options.NVIC_IRQChannelSubPriority = 0;
	NVIC_Options.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Options);

	NVIC_Options.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_Options.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Options.NVIC_IRQChannelSubPriority = 1;
	NVIC_Options.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Options);
}
