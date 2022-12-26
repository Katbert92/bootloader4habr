/**
 ******************************************************************************
 * @file    usb_pwr.c
 * @author  MCD Application Team
 * @version V4.0.0
 * @date    21-January-2013
 * @brief   Connection/disconnection & power management
 ******************************************************************************
 */

#include "usb_pwr.h"

volatile u32 bDeviceState = UNCONNECTED; /* USB device status */
volatile bool fSuspendEnabled = TRUE; /* true when suspend is possible */
volatile u32 EP[8];

struct
{
	volatile RESUME_STATE eState;
	volatile u8 bESOFcnt;
} ResumeS;

volatile u32 remotewakeupon = 0;

RESULT PowerOn(void)
{
	u16 wRegVal;

	/*** cable plugged-in ? ***/
	USB_CableConfig(ENABLE);

	/*** CNTR_PWDN = 0 ***/
	wRegVal = CNTR_FRES;
	_SetCNTR(wRegVal);

	/*** CNTR_FRES = 0 ***/
	wInterrupt_Mask = 0;
	_SetCNTR(wInterrupt_Mask);

	/*** Clear pending interrupts ***/
	_SetISTR(0);

	/*** Set interrupt mask ***/
	wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
	_SetCNTR(wInterrupt_Mask);

	return USB_SUCCESS;
}

RESULT PowerOff() //handles switch-off conditions
{
	/* disable all interrupts and force USB reset */
	_SetCNTR(CNTR_FRES);

	/* clear interrupt status register */
	_SetISTR(0);

	/* Disable the Pull-Up*/
	USB_CableConfig(DISABLE);

	/* switch-off device */
	_SetCNTR(CNTR_FRES + CNTR_PDWN);

	/* sw variables reset */

	return USB_SUCCESS;
}

void Suspend(void) //sets suspend mode operating conditions
{
	u32 i = 0;
	u16 wCNTR;
	u32 tmpreg = 0;
	volatile u32 savePWR_CR = 0;

	/* suspend preparation */
	/* ... */

	/*Store CNTR value */
	wCNTR = _GetCNTR();

	/* This a sequence to apply a force RESET to handle a robustness case */

	/*Store endpoints registers status */
	for (i = 0; i < 8; i++)
	{
		EP[i] = _GetENDPOINT(i);
	}

	/* unmask RESET flag */
	wCNTR |= CNTR_RESETM;
	_SetCNTR(wCNTR);

	/*apply FRES */
	wCNTR |= CNTR_FRES;
	_SetCNTR(wCNTR);

	/*clear FRES*/
	wCNTR &= ~CNTR_FRES;
	_SetCNTR(wCNTR);

	/*poll for RESET flag in ISTR*/
	while ((_GetISTR() & ISTR_RESET) == 0);

	/* clear RESET flag in ISTR */
	_SetISTR((u16)CLR_RESET);

	/*restore Enpoints*/
	for (i = 0; i < 8; i++)
	{
		_SetENDPOINT(i, EP[i]);
	}

	/* Now it is safe to enter macrocell in suspend mode */
	wCNTR |= CNTR_FSUSP;
	_SetCNTR(wCNTR);

	/* force low-power mode in the macrocell */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_LPMODE;
	_SetCNTR(wCNTR);

	/*prepare entry in low power mode (STOP mode)*/
	/* Select the regulator state in STOP mode*/
	savePWR_CR = PWR->CR;
	tmpreg = PWR->CR;

	/* Clear PDDS and LPDS bits */
	tmpreg &= ((u32) 0xFFFFFFFC);

	/* Set LPDS bit according to PWR_Regulator value */
	tmpreg |= PWR_Regulator_LowPower;

	/* Store the new value */
	PWR->CR = tmpreg;

	/* Set SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR |= SCB_SCR_SLEEPDEEP;

	/* enter system in STOP mode, only when wakeup flag in not set */
	if ((_GetISTR() & ISTR_WKUP) == 0)
	{
		__WFI();

		/* Reset SLEEPDEEP bit of Cortex System Control Register */
		SCB->SCR &= (u32) ~((u32) SCB_SCR_SLEEPDEEP);
	}
	else
	{
		/* Clear Wakeup flag */
		_SetISTR(CLR_WKUP);

		/* clear FSUSP to abort entry in suspend mode  */
		wCNTR = _GetCNTR();
		wCNTR &= ~CNTR_FSUSP;
		_SetCNTR(wCNTR);

		/*restore sleep mode configuration */
		/* restore Power regulator config in sleep mode*/
		PWR->CR = savePWR_CR;

		/* Reset SLEEPDEEP bit of Cortex System Control Register */
		SCB->SCR &= (u32) ~((u32) SCB_SCR_SLEEPDEEP);
	}
}

void Resume_Init(void) //Handles wake-up restoring normal operations
{
	u16 wCNTR;

	/* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
	/* restart the clocks */
	/* ...	*/

	/* CNTR_LPMODE = 0 */
	wCNTR = _GetCNTR();
	wCNTR &= (~CNTR_LPMODE);
	_SetCNTR(wCNTR);

	/* restore full power */
	/* ... on connected devices */
	Leave_LowPowerMode();

	/* reset FSUSP bit */
	_SetCNTR(IMR_MSK);

	/* reverse suspend preparation */
}

/*******************************************************************************
 * Description    : This is the state machine handling resume operations and
 *                 timing sequence. The control is based on the Resume structure
 *                 variables and on the ESOF interrupt calling this subroutine
 *                 without changing machine state.
 * Input          : a state machine value (RESUME_STATE)
 *                  RESUME_ESOF doesn't change ResumeS.eState allowing
 *                  decrementing of the ESOF counter in different states.
 *******************************************************************************/
void Resume(RESUME_STATE eResumeSetVal)
{
	u16 wCNTR;

	if (eResumeSetVal != RESUME_ESOF)
	{
		ResumeS.eState = eResumeSetVal;
	}

	switch (ResumeS.eState)
	{
		case RESUME_EXTERNAL:
			if (remotewakeupon == 0)
			{
				Resume_Init();
				ResumeS.eState = RESUME_OFF;
			}
			else //RESUME detected during the RemoteWAkeup signalling => keep RemoteWakeup handling
			{
				ResumeS.eState = RESUME_ON;
			}
			break;

		case RESUME_INTERNAL:
			Resume_Init();
			ResumeS.eState = RESUME_START;
			remotewakeupon = 1;
			break;

		case RESUME_LATER:
			ResumeS.bESOFcnt = 2;
			ResumeS.eState = RESUME_WAIT;
			break;

		case RESUME_WAIT:
			ResumeS.bESOFcnt--;
			if (ResumeS.bESOFcnt == 0)
			{
				ResumeS.eState = RESUME_START;
			}
			break;

		case RESUME_START:
			wCNTR = _GetCNTR();
			wCNTR |= CNTR_RESUME;
			_SetCNTR(wCNTR);
			ResumeS.eState = RESUME_ON;
			ResumeS.bESOFcnt = 10;
			break;

		case RESUME_ON:
			ResumeS.bESOFcnt--;
			if (ResumeS.bESOFcnt == 0)
			{
				wCNTR = _GetCNTR();
				wCNTR &= (~CNTR_RESUME);
				_SetCNTR(wCNTR);
				ResumeS.eState = RESUME_OFF;
				remotewakeupon = 0;
			}
			break;

		case RESUME_OFF:

		case RESUME_ESOF:

		default:
			ResumeS.eState = RESUME_OFF;
			break;
	}
}
