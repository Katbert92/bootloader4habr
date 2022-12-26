/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA hard disk control module */
#include "mass_mal.h"		/* Example: Header file of existing MMC/SDC control module */

/* Definitions of physical drive number for each drive */
#define ATA			0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC			1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB			2	/* Example: Map USB MSD to physical drive 2 */
#define SECTOR_SIZE 2048U

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	MAL_Init(0);

	return 0;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u32 i;

	for(i = 0; i < count * SECTOR_SIZE; i++)
	{
		buff[i] = *((volatile u8*)(FLASH_MSD_START_ADDR + sector * SECTOR_SIZE + i));
	}

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	LED_RGB_SetBrightness(LED_BLUE, 100);

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	return RES_OK;
}

DWORD get_fattime(void)
{

    return (	(DWORD)(2016 - 1980) << 25) |
             	((DWORD)12 << 21) |
             	((DWORD)16 << 16) |
             	((DWORD)16 << 11) |
             	((DWORD)16 << 5) |
             	((DWORD)16 >> 1);
}
