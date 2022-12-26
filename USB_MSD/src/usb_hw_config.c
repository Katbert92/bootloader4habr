#include "usb_hw_config.h"

static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);

void Enter_LowPowerMode(void) //Power-off system clocks and power while entering suspend mode
{
	bDeviceState = SUSPENDED;
}

void Leave_LowPowerMode(void) //Restores system clocks and power while exiting suspend mode
{
	DEVICE_INFO *pInfo = &Device_Info;

	if (pInfo->Current_Configuration != 0) //Set the device state to the correct state
	{
		bDeviceState = CONFIGURED;
	}
	else
	{
		bDeviceState = ATTACHED;
	}

	SystemInit(); //Enable SystemCoreClock
}

void USB_CableConfig(FunctionalState NewState) //Software Connection/Disconnection of USB Cable
{
	if (NewState != DISABLE)
	{
		USB_DISCONNECT_PORT->BRR = USB_DISCONNECT_PIN;
	}
	else
	{
		USB_DISCONNECT_PORT->BSRR = USB_DISCONNECT_PIN;
	}
}

void Get_SerialNum(void) //Create the serial number string descriptor
{
	u32 CPU_SerialNumber[3];

	CPU_SerialNumber[0] = *(volatile u32 *)(0x1FFFF7E8);
	CPU_SerialNumber[1] = *(volatile u32 *)(0x1FFFF7EC);
	CPU_SerialNumber[2] = *(volatile u32 *)(0x1FFFF7F0);

	CPU_SerialNumber[0] += CPU_SerialNumber[2];

	IntToUnicode(CPU_SerialNumber[0], &MASS_StringSerial[2], 8);
	IntToUnicode(CPU_SerialNumber[1], &MASS_StringSerial[18], 4);
}

static void IntToUnicode (u32 value, u8 *pbuf, u8 len) //Convert Hex 32Bits value into char
{
	u8 idx;

	for(idx = 0; idx < len ; idx++)
	{
		if((value >> 28) < 0xA)
		{
			pbuf[2 * idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2 * idx] = (value >> 28) + 'A' - 10;
		}

		value = value << 4;

		pbuf[2 * idx + 1] = 0;
	}
}
