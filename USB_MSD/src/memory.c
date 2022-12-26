/**
 ******************************************************************************
 * @file    memory.c
 * @author  MCD Application Team
 * @version V4.0.0
 * @date    21-January-2013
 * @brief   Memory management layer
 ******************************************************************************
 */

#include "memory.h"

volatile u32 blockReadCount = 0;
volatile u32 blockOffset;
volatile u32 counter = 0;

u32 idx;
u32 dataBuffer[BULK_MAX_PACKET_SIZE * 8]; //2048 bytes
u8 transferState = TXFR_IDLE;

extern u8 Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  //data buffer
extern u16 Data_Len;
extern u8 Bot_State;
extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;
extern u32 MassMemorySize[1];
extern u32 MassBlockSize[1];

void Memory_Read(u8 lun, u32 memoryOffset, u32 transferLength) //Handle the Read operation from the microSD card
{
	static u32 offsetRD, lengthRD;

	if (transferState == TXFR_IDLE)
	{
		offsetRD = memoryOffset * MassBlockSize[lun];
		lengthRD = transferLength * MassBlockSize[lun];
		transferState = TXFR_ONGOING;
	}

	if (transferState == TXFR_ONGOING)
	{
		if (!blockReadCount)
		{
			MAL_Read(lun, offsetRD, dataBuffer, MassBlockSize[lun]);
			USB_SIL_Write(EP1_IN, (u8*)dataBuffer, BULK_MAX_PACKET_SIZE);
			blockReadCount = MassBlockSize[lun] - BULK_MAX_PACKET_SIZE;
			blockOffset = BULK_MAX_PACKET_SIZE;
		}
		else
		{
			USB_SIL_Write(EP1_IN, (u8*)dataBuffer + blockOffset, BULK_MAX_PACKET_SIZE);
			blockReadCount -= BULK_MAX_PACKET_SIZE;
			blockOffset += BULK_MAX_PACKET_SIZE;
		}

		SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
		SetEPTxStatus(ENDP1, EP_TX_VALID);

		offsetRD += BULK_MAX_PACKET_SIZE;
		lengthRD -= BULK_MAX_PACKET_SIZE;

		CSW.dDataResidue -= BULK_MAX_PACKET_SIZE;
	}

	if (lengthRD == 0)
	{
		blockReadCount = 0;
		blockOffset = 0;
		offsetRD = 0;
		Bot_State = BOT_DATA_IN_LAST;
		transferState = TXFR_IDLE;
	}
}

void Memory_Write(u8 lun, u32 memoryOffset, u32 transferLength) //Handle the Write operation to the microSD card
{
	static u32 offsetWR, lengthWR;

	u32 temp = counter + 64;

	if (transferState == TXFR_IDLE)
	{
		offsetWR = memoryOffset * MassBlockSize[lun];
		lengthWR = transferLength * MassBlockSize[lun];
		transferState = TXFR_ONGOING;
	}

	if (transferState == TXFR_ONGOING)
	{
		for (idx = 0; counter < temp; counter++)
		{
			*((u8*)dataBuffer + counter) = Bulk_Data_Buff[idx++];
		}

		offsetWR += Data_Len;
		lengthWR -= Data_Len;

		if (!(lengthWR % MassBlockSize[lun]))
		{
			counter = 0;
			MAL_Write(lun, offsetWR - MassBlockSize[lun], dataBuffer, MassBlockSize[lun]);
		}

		CSW.dDataResidue -= Data_Len;
		SetEPRxStatus(ENDP2, EP_RX_VALID); //enable the next transaction
	}

	if ((lengthWR == 0) || (Bot_State == BOT_CSW_Send))
	{
		counter = 0;
		Set_CSW(CSW_CMD_PASSED, SEND_CSW_ENABLE);
		transferState = TXFR_IDLE;
	}
}
