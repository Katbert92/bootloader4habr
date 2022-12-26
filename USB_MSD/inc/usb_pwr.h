/**
  ******************************************************************************
  * @file    usb_pwr.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Connection/disconnection & power management header
  ******************************************************************************
  */

#ifndef __USB_PWR_H
#define __USB_PWR_H

#include "main.h"

typedef enum _RESUME_STATE
{
	RESUME_EXTERNAL,
	RESUME_INTERNAL,
	RESUME_LATER,
	RESUME_WAIT,
	RESUME_START,
	RESUME_ON,
	RESUME_OFF,
	RESUME_ESOF
} RESUME_STATE;

typedef enum _DEVICE_STATE
{
	UNCONNECTED,
	ATTACHED,
	POWERED,
	SUSPENDED,
	ADDRESSED,
	CONFIGURED
} DEVICE_STATE;

void Suspend(void);
void Resume_Init(void);
void Resume(RESUME_STATE eResumeSetVal);
RESULT PowerOn(void);
RESULT PowerOff(void);

extern volatile u32 bDeviceState; 		// USB device status
extern volatile bool fSuspendEnabled;	// true when suspend is possible

#endif  /*__USB_PWR_H*/
