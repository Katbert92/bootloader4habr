/**
 ******************************************************************************
 * @file    mass_mal.c
 * @author  MCD Application Team
 * @version V4.0.0
 * @date    21-January-2013
 * @brief   Medium Access Layer interface
 ******************************************************************************
 */

#include "mass_mal.h"

u32 MassMemorySize[1];
u32 MassBlockSize[1];
u32 MassBlockCount[1];

u16 MAL_Init(u8 lun) //Initializes the Media on the STM32
{
	switch (lun)
	{
		case 0:

			FLASH_Unlock();

			break;

		default:

			return MAL_FAIL;
	}

	return MAL_OK;
}

u16 MAL_Read(u8 lun, u32 memOffset, u32 *readBuff, u16 transferLen) //Read sectors
{
	u32 i;

	switch (lun)
	{
		case 0:

			LED_RGB_SetBrightness(LED_GREEN, 100);

			for(i = 0; i < transferLen / U32_LEN; i++)
			{
				readBuff[i] = *((volatile u32*)(FLASH_MSD_START_ADDR + memOffset) + i);
			}

			LED_RGB_SetBrightness(LED_GREEN, 0);

			break;

		default:

			return MAL_FAIL;
	}

	return MAL_OK;
}

u16 MAL_Write(u8 lun, u32 memOffset, u32 *writeBuff, u16 transferLen) //Write sectors
{
	u32 i;

	switch (lun)
	{
		case 0:

			LED_RGB_SetBrightness(LED_RED, 100);

			for(i = 0; i < transferLen; i += FLASH_PAGE_SIZE)
			{
				while(FLASH_GetStatus() != FLASH_COMPLETE);
				FLASH_ErasePage(FLASH_MSD_START_ADDR + memOffset + i);
			}

			for(i = 0; i < transferLen; i += U32_LEN)
			{
				while(FLASH_GetStatus() != FLASH_COMPLETE);
				FLASH_ProgramWord(FLASH_MSD_START_ADDR + memOffset + i, writeBuff[i / U32_LEN]);
			}

			LED_RGB_SetBrightness(LED_RED, 0);

			break;

		default:

			return MAL_FAIL;
	}

	return MAL_OK;
}

u16 MAL_GetStatus(u8 lun) //Get status
{
	switch(lun)
	{
		case 0:

			MassBlockCount[0] = FLASH_MSD_SIZE / FLASH_PAGE_SIZE;
			MassBlockSize[0] =  FLASH_PAGE_SIZE;
			MassMemorySize[0] = FLASH_MSD_SIZE;

			break;


		default:

			return MAL_FAIL;
	}

	return MAL_OK;
}
