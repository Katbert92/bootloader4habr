/**
 ******************************************************************************
 * @file    usb_scsi.c
 * @author  MCD Application Team
 * @version V4.0.0
 * @date    21-January-2013
 * @brief   All processing related to the SCSI commands
 ******************************************************************************
 */

#include "usb_scsi.h"

extern u8 Bulk_Data_Buff[BULK_MAX_PACKET_SIZE]; /* data buffer*/
extern u8 Bot_State;
extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;
extern u32 MassMemorySize[1];
extern u32 MassBlockSize[1];
extern u32 MassBlockCount[1];

void SCSI_Inquiry_Cmd(u8 lun) //SCSI Inquiry Command routine
{
	u8* iInquiryData;
	u16 inquiryDataLength;

	if (CBW.CB[1] & 0x01) //Evpd is set
	{
		iInquiryData = Page00_Inquiry_Data;
		inquiryDataLength = 5;
	}
	else
	{
		if (lun == 0)
		{
			iInquiryData = Standard_Inquiry_Data;
		}

		if (CBW.CB[4] <= STANDARD_INQUIRY_DATA_LEN)
		{
			inquiryDataLength = CBW.CB[4];
		}
		else
		{
			inquiryDataLength = STANDARD_INQUIRY_DATA_LEN;
		}
	}

	Transfer_Data_Request(iInquiryData, inquiryDataLength);
}

void SCSI_ReadFormatCapacity_Cmd(u8 lun) //SCSI ReadFormatCapacity Command routine
{
	if (MAL_GetStatus(lun) != 0)
	{
		Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
		Set_CSW(CSW_CMD_FAILED, SEND_CSW_ENABLE);
		Bot_Abort(DIR_IN);
		return;
	}

	ReadFormatCapacity_Data[4] = (u8) (MassBlockCount[lun] >> 24);
	ReadFormatCapacity_Data[5] = (u8) (MassBlockCount[lun] >> 16);
	ReadFormatCapacity_Data[6] = (u8) (MassBlockCount[lun] >> 8);
	ReadFormatCapacity_Data[7] = (u8) (MassBlockCount[lun]);

	ReadFormatCapacity_Data[9] = (u8) (MassBlockSize[lun] >> 16);
	ReadFormatCapacity_Data[10] = (u8) (MassBlockSize[lun] >> 8);
	ReadFormatCapacity_Data[11] = (u8) (MassBlockSize[lun]);

	Transfer_Data_Request(ReadFormatCapacity_Data, READ_FORMAT_CAPACITY_DATA_LEN);
}

void SCSI_ReadCapacity10_Cmd(u8 lun) //SCSI ReadCapacity10 Command routine
{
	if (MAL_GetStatus(lun))
	{
		Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
		Set_CSW(CSW_CMD_FAILED, SEND_CSW_ENABLE);
		Bot_Abort(DIR_IN);
		return;
	}

	ReadCapacity10_Data[0] = (u8) ((MassBlockCount[lun] - 1) >> 24);
	ReadCapacity10_Data[1] = (u8) ((MassBlockCount[lun] - 1) >> 16);
	ReadCapacity10_Data[2] = (u8) ((MassBlockCount[lun] - 1) >> 8);
	ReadCapacity10_Data[3] = (u8) (MassBlockCount[lun] - 1);

	ReadCapacity10_Data[4] = (u8) (MassBlockSize[lun] >> 24);
	ReadCapacity10_Data[5] = (u8) (MassBlockSize[lun] >> 16);
	ReadCapacity10_Data[6] = (u8) (MassBlockSize[lun] >> 8);
	ReadCapacity10_Data[7] = (u8) (MassBlockSize[lun]);

	Transfer_Data_Request(ReadCapacity10_Data, READ_CAPACITY10_DATA_LEN);
}

void SCSI_ModeSense6_Cmd(u8 lun) //SCSI ModeSense6 Command routine
{
	Transfer_Data_Request(Mode_Sense6_data, MODE_SENSE6_DATA_LEN);
}

void SCSI_ModeSense10_Cmd(u8 lun) //SCSI ModeSense10 Command routine
{
	Transfer_Data_Request(Mode_Sense10_data, MODE_SENSE10_DATA_LEN);
}

void SCSI_RequestSense_Cmd(u8 lun) //SCSI RequestSense Command routine
{
	u8 requestSenseDataLength;

	if (CBW.CB[4] <= REQUEST_SENSE_DATA_LEN)
	{
		requestSenseDataLength = CBW.CB[4];
	}
	else
	{
		requestSenseDataLength = REQUEST_SENSE_DATA_LEN;
	}
	Transfer_Data_Request(Scsi_Sense_Data, requestSenseDataLength);
}

void Set_Scsi_Sense_Data(u8 lun, u8 sensKey, u8 asc) //Set Scsi Sense Data routine
{
	Scsi_Sense_Data[2] = sensKey;
	Scsi_Sense_Data[12] = asc;
}

void SCSI_Start_Stop_Unit_Cmd(u8 lun) //SCSI Start_Stop_Unit Command routine
{
	Set_CSW(CSW_CMD_PASSED, SEND_CSW_ENABLE);
}

void SCSI_Read10_Cmd(u8 lun, u32 LBA, u32 blockNbr) //SCSI Read10 Command routine
{
	if (Bot_State == BOT_IDLE)
	{
		if (!(SCSI_Address_Management(CBW.bLUN, SCSI_READ10, LBA, blockNbr))) //address out of range
		{
			return;
		}

		if ((CBW.bmFlags & 0x80) != 0)
		{
			Bot_State = BOT_DATA_IN;
			Memory_Read(lun, LBA, blockNbr);
		}
		else
		{
			Bot_Abort(BOTH_DIR);
			Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
			Set_CSW(CSW_CMD_FAILED, SEND_CSW_ENABLE);
		}
		return;
	}
	else if (Bot_State == BOT_DATA_IN)
	{
		Memory_Read(lun, LBA, blockNbr);
	}
}

void SCSI_Write10_Cmd(u8 lun, u32 LBA, u32 blockNbr) //SCSI Write10 Command routine
{
	if (Bot_State == BOT_IDLE)
	{
		if (!(SCSI_Address_Management(CBW.bLUN, SCSI_WRITE10, LBA, blockNbr))) //address out of range
		{
			return;
		}

		if ((CBW.bmFlags & 0x80) == 0)
		{
			Bot_State = BOT_DATA_OUT;
			SetEPRxStatus(ENDP2, EP_RX_VALID);
		}
		else
		{
			Bot_Abort(DIR_IN);
			Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
			Set_CSW(CSW_CMD_FAILED, SEND_CSW_DISABLE);
		}
		return;
	}
	else if (Bot_State == BOT_DATA_OUT)
	{
		Memory_Write(lun, LBA, blockNbr);
	}
}

void SCSI_Verify10_Cmd(u8 lun) //SCSI Verify10 Command routine
{
	if ((CBW.dDataLength == 0) && !(CBW.CB[1] & BLKVFY)) //BLKVFY not set
	{
		Set_CSW(CSW_CMD_PASSED, SEND_CSW_ENABLE);
	}
	else
	{
		Bot_Abort(BOTH_DIR);
		Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
		Set_CSW(CSW_CMD_FAILED, SEND_CSW_DISABLE);
	}
}

void SCSI_Valid_Cmd(u8 lun) //Valid Commands routine
{
	if (CBW.dDataLength != 0)
	{
		Bot_Abort(BOTH_DIR);
		Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
		Set_CSW(CSW_CMD_FAILED, SEND_CSW_DISABLE);
	}
	else
		Set_CSW(CSW_CMD_PASSED, SEND_CSW_ENABLE);
}

void SCSI_TestUnitReady_Cmd(u8 lun) //Valid Commands routine
{
	if (MAL_GetStatus(lun))
	{
		Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
		Set_CSW(CSW_CMD_FAILED, SEND_CSW_ENABLE);
		Bot_Abort(DIR_IN);
		return;
	}
	else
	{
		Set_CSW(CSW_CMD_PASSED, SEND_CSW_ENABLE);
	}
}

void SCSI_Format_Cmd(u8 lun) //Format Commands routine
{
	if (MAL_GetStatus(lun))
	{
		Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
		Set_CSW(CSW_CMD_FAILED, SEND_CSW_ENABLE);
		Bot_Abort(DIR_IN);
		return;
	}
}

void SCSI_Invalid_Cmd(u8 lun) //Invalid Commands routine
{
	if (CBW.dDataLength == 0)
	{
		Bot_Abort(DIR_IN);
	}
	else
	{
		if ((CBW.bmFlags & 0x80) != 0)
		{
			Bot_Abort(DIR_IN);
		}
		else
		{
			Bot_Abort(BOTH_DIR);
		}
	}

	Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
	Set_CSW(CSW_CMD_FAILED, SEND_CSW_DISABLE);
}

/*******************************************************************************
 * Description    : Test the received address.
 * Input          : u8 Cmd : the command can be SCSI_READ10 or SCSI_WRITE10.
 *******************************************************************************/
bool SCSI_Address_Management(u8 lun, u8 Cmd, u32 LBA, u32 blockNbr)
{
	if ((LBA + blockNbr) > MassBlockCount[lun])
	{
		if (Cmd == SCSI_WRITE10)
		{
			Bot_Abort(BOTH_DIR);
		}

		Bot_Abort(DIR_IN);
		Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
		Set_CSW(CSW_CMD_FAILED, SEND_CSW_DISABLE);

		return (FALSE);
	}

	if (CBW.dDataLength != blockNbr * MassBlockSize[lun])
	{
		if (Cmd == SCSI_WRITE10)
		{
			Bot_Abort(BOTH_DIR);
		}
		else
		{
			Bot_Abort(DIR_IN);
		}

		Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
		Set_CSW(CSW_CMD_FAILED, SEND_CSW_DISABLE);

		return (FALSE);
	}

	return (TRUE);
}
