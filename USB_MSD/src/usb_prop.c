/**
  ******************************************************************************
  * @file    usb_prop.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   All processing related to Mass Storage Demo
  ******************************************************************************
  */

#include "usb_prop.h"

u32 Max_Lun = 0;

DEVICE Device_Table =
{
	EP_NUM,
	1
};

DEVICE_PROP Device_Property =
{
	MASS_Init,
	MASS_Reset,
	MASS_Status_In,
	MASS_Status_Out,
	MASS_Data_Setup,
	MASS_NoData_Setup,
	MASS_Get_Interface_Setting,
	MASS_GetDeviceDescriptor,
	MASS_GetConfigDescriptor,
	MASS_GetStringDescriptor,
	0,
	0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
	Mass_Storage_GetConfiguration,
	Mass_Storage_SetConfiguration,
	Mass_Storage_GetInterface,
	Mass_Storage_SetInterface,
	Mass_Storage_GetStatus,
	Mass_Storage_ClearFeature,
	Mass_Storage_SetEndPointFeature,
	Mass_Storage_SetDeviceFeature,
	Mass_Storage_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor =
{
	(u8*)MASS_DeviceDescriptor,
	MASS_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor =
{
	(u8*)MASS_ConfigDescriptor,
	MASS_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[5] =
{
	{(u8*)MASS_StringLangID, MASS_SIZ_STRING_LANGID},
	{(u8*)MASS_StringVendor, MASS_SIZ_STRING_VENDOR},
	{(u8*)MASS_StringProduct, MASS_SIZ_STRING_PRODUCT},
	{(u8*)MASS_StringSerial, MASS_SIZ_STRING_SERIAL},
	{(u8*)MASS_StringInterface, MASS_SIZ_STRING_INTERFACE},
};

extern unsigned char Bot_State;
extern Bulk_Only_CBW CBW;

void MASS_Init(void) //Mass Storage init routine
{
	/* Update the serial number string descriptor with the data from the unique ID*/
	Get_SerialNum();

	pInformation->Current_Configuration = 0;

	/* Connect the device */
	PowerOn();

	/* Perform basic device initialization operations */
	USB_SIL_Init();

	bDeviceState = UNCONNECTED;
}

void MASS_Reset(void) //Mass Storage reset routine
{
	/* Set the device as not configured */
	Device_Info.Current_Configuration = 0;

	/* Current Feature initialization */
	pInformation->Current_Feature = MASS_ConfigDescriptor[7];

	SetBTABLE(BTABLE_ADDRESS);

	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_NAK);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxValid(ENDP0);

	/* Initialize Endpoint 1 */
	SetEPType(ENDP1, EP_BULK);
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPTxStatus(ENDP1, EP_TX_NAK);
	SetEPRxStatus(ENDP1, EP_RX_DIS);

	/* Initialize Endpoint 2 */
	SetEPType(ENDP2, EP_BULK);
	SetEPRxAddr(ENDP2, ENDP2_RXADDR);
	SetEPRxCount(ENDP2, Device_Property.MaxPacketSize);
	SetEPRxStatus(ENDP2, EP_RX_VALID);
	SetEPTxStatus(ENDP2, EP_TX_DIS);

	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPRxValid(ENDP0);

	/* Set the device to response on default address */
	SetDeviceAddress(0);

	bDeviceState = ATTACHED;

	CBW.dSignature = BOT_CBW_SIGNATURE;
	Bot_State = BOT_IDLE;

}

void Mass_Storage_SetConfiguration(void) //Handle the SetConfiguration request
{
	if (pInformation->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;

		ClearDTOG_TX(ENDP1);
		ClearDTOG_RX(ENDP2);

		Bot_State = BOT_IDLE; /* set the Bot state machine to the IDLE state */
	}
}

void Mass_Storage_ClearFeature(void) //Handle the ClearFeature request
{
	/* when the host send a CBW with invalid signature or invalid length the two
	 Endpoints (IN & OUT) shall stall until receiving a Mass Storage Reset     */
	if (CBW.dSignature != BOT_CBW_SIGNATURE)
	{
		Bot_Abort(BOTH_DIR);
	}
}

void Mass_Storage_SetDeviceAddress(void) //Update the device state to addressed
{
	bDeviceState = ADDRESSED;
}

void MASS_Status_In(void) //Mass Storage Status IN routine
{
	return;
}

void MASS_Status_Out(void) //Mass Storage Status OUT routine
{
	return;
}

RESULT MASS_Data_Setup(u8 RequestNo) //Handle the data class specific requests
{
	u8 *(*CopyRoutine)(u16);

	CopyRoutine = NULL;
	if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
			&& (RequestNo == GET_MAX_LUN) && (pInformation->USBwValue == 0)
			&& (pInformation->USBwIndex == 0)
			&& (pInformation->USBwLength == 0x01))
	{
		CopyRoutine = Get_Max_Lun;
	}
	else
	{
		return USB_UNSUPPORT;
	}

	if (CopyRoutine == NULL)
	{
		return USB_UNSUPPORT;
	}

	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);

	return USB_SUCCESS;
}

RESULT MASS_NoData_Setup(u8 RequestNo) //Handle the no data class specific requests
{
	if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
			&& (RequestNo == MASS_STORAGE_RESET)
			&& (pInformation->USBwValue == 0) && (pInformation->USBwIndex == 0)
			&& (pInformation->USBwLength == 0x00))
	{
		/* Initialize Endpoint 1 */
		ClearDTOG_TX(ENDP1);

		/* Initialize Endpoint 2 */
		ClearDTOG_RX(ENDP2);

		/*initialize the CBW signature to enable the clear feature*/
		CBW.dSignature = BOT_CBW_SIGNATURE;
		Bot_State = BOT_IDLE;

		return USB_SUCCESS;
	}

	return USB_UNSUPPORT;
}

/*******************************************************************************
 * Description    : Test the interface and the alternate setting according to the
 *                  supported one.
 *******************************************************************************/
RESULT MASS_Get_Interface_Setting(u8 interface, u8 alternateSetting)
{
	if (alternateSetting > 0)
	{
		return USB_UNSUPPORT;/* in this application we don't have AlternateSetting*/
	}
	else if (interface > 0)
	{
		return USB_UNSUPPORT;/*in this application we have only 1 interfaces*/
	}
	return USB_SUCCESS;
}

u8 *MASS_GetDeviceDescriptor(u16 length) //Get the device descriptor
{
	return Standard_GetDescriptorData(length, &Device_Descriptor);
}

u8 *MASS_GetConfigDescriptor(u16 length) //Get the configuration descriptor
{
	return Standard_GetDescriptorData(length, &Config_Descriptor);
}

u8 *MASS_GetStringDescriptor(u16 length) //Get the string descriptors according to the needed index
{
	u8 wValue0 = pInformation->USBwValue0;

	if (wValue0 > 5)
	{
		return NULL;
	}
	else
	{
		return Standard_GetDescriptorData(length, &String_Descriptor[wValue0]);
	}
}

u8 *Get_Max_Lun(u16 length) //Handle the Get Max Lun request
{
	if (length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = LUN_DATA_LENGTH;
		return 0;
	}
	else
	{
		return ((u8*)(&Max_Lun));
	}
}
