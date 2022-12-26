/**
  ******************************************************************************
  * @file    memory.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Memory management layer
  ******************************************************************************
  */

#ifndef __MEMORY_H
#define __MEMORY_H

#include "main.h"

#define TXFR_IDLE     0
#define TXFR_ONGOING  1

void Memory_Read(u8 lun, u32 memoryOffset, u32 transferLength);
void Memory_Write(u8 lun, u32 memoryOffset, u32 transferLength);

#endif /* __MEMORY_H */
