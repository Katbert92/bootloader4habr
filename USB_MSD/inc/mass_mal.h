/**
  ******************************************************************************
  * @file    mass_mal.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Header for mass_mal.c file.
  ******************************************************************************
  */

#ifndef __MASS_MAL_H
#define __MASS_MAL_H

#include "main.h"

#define MAL_OK		0
#define MAL_FAIL	1
#define MAX_LUN		1

#define U32_LEN					4
#define FLASH_PAGE_SIZE			2048		//2K per page
#define WAIT_TIMEOUT			100000

#define FLASH_START_ADDR		0x08000000									//Origin
#define FLASH_MAX_SIZE			0x00080000									//Max FLASH size - 512 kByte
#define	FLASH_END_ADDR			(FLASH_START_ADDR + FLASH_MAX_SIZE)			//FLASH end address

#define FLASH_BOOT_START_ADDR	FLASH_START_ADDR							//Bootloader start address
#define FLASH_BOOT_SIZE			0x00010000 									//64 kByte for bootloader
#define FLASH_USER_START_ADDR	(FLASH_BOOT_START_ADDR + FLASH_BOOT_SIZE)	//User application start address
#define FLASH_USER_SIZE			0x00032000									//256 kByte for user application
#define FLASH_MSD_START_ADDR	(FLASH_USER_START_ADDR + FLASH_USER_SIZE)	//USB MSD start address
#define FLASH_MSD_SIZE			0x00032000									//128 kByte for USB MASS Storage
#define FLASH_OTHER_START_ADDR	(FLASH_MSD_START_ADDR + FLASH_MSD_SIZE)		//Other free memory start address
#define FLASH_OTHER_SIZE		(FLASH_END_ADDR - FLASH_OTHER_START_ADDR)	//Free space size

u16 MAL_Init (u8 lun);
u16 MAL_Read(u8 lun, u32 memOffset, u32 *readBuff, u16 transferLen);
u16 MAL_Write(u8 lun, u32 memOffset, u32 *writeBuff, u16 transferLen);
u16 MAL_GetStatus (u8 lun);

#endif /* __MASS_MAL_H */
