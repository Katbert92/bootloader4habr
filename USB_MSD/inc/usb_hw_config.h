#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "main.h"

#define BULK_MAX_PACKET_SIZE  0x00000040

void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_CableConfig (FunctionalState NewState);
void Get_SerialNum(void);


#endif  /*__HW_CONFIG_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
