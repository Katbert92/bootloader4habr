/**
  ******************************************************************************
  * @file    usb_prop.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   All processing related to Mass Storage Demo (Endpoint 0)
  ******************************************************************************
  */

#ifndef __USB_PROP_H
#define __USB_PROP_H

#include "main.h"

#define Mass_Storage_GetConfiguration          NOP_Process
/* #define Mass_Storage_SetConfiguration          NOP_Process*/
#define Mass_Storage_GetInterface              NOP_Process
#define Mass_Storage_SetInterface              NOP_Process
#define Mass_Storage_GetStatus                 NOP_Process
/* #define Mass_Storage_ClearFeature              NOP_Process*/
#define Mass_Storage_SetEndPointFeature        NOP_Process
#define Mass_Storage_SetDeviceFeature          NOP_Process
/*#define Mass_Storage_SetDeviceAddress          NOP_Process*/

/* MASS Storage Requests*/
#define GET_MAX_LUN                0xFE
#define MASS_STORAGE_RESET         0xFF
#define LUN_DATA_LENGTH            1

void MASS_Init(void);
void MASS_Reset(void);
void Mass_Storage_SetConfiguration(void);
void Mass_Storage_ClearFeature(void);
void Mass_Storage_SetDeviceAddress(void);
void MASS_Status_In(void);
void MASS_Status_Out(void);
RESULT MASS_Data_Setup(u8);
RESULT MASS_NoData_Setup(u8);
RESULT MASS_Get_Interface_Setting(u8 interface, u8 alternateSetting);
u8 *MASS_GetDeviceDescriptor(u16);
u8 *MASS_GetConfigDescriptor(u16);
u8 *MASS_GetStringDescriptor(u16);
u8 *Get_Max_Lun(u16 length);

#endif /* __USB_PROP_H */
