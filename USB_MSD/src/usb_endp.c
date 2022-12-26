/**
 ******************************************************************************
 * @file    usb_endp.c
 * @author  MCD Application Team
 * @version V4.0.0
 * @date    21-January-2013
 * @brief   Endpoint routines
 ******************************************************************************
 */

#include "main.h"

void EP1_IN_Callback(void) //EP1 IN Callback Routine
{
	Mass_Storage_In();
}

void EP2_OUT_Callback(void) //EP2 OUT Callback Routine
{
	Mass_Storage_Out();
}
