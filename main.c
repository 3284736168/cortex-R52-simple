/*
** Copyright (c) 2016-2023 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such license agreement.
*/

/* This file contains the main() program that displays a welcome message, enables the caches,
   performs a float calculation to demonstrate floating point, then runs the main application (sorts) */

#include <stdio.h>
#include <arm_acle.h>
#include <stdint.h>
//#include "generictimer.h"
#include "v8rgicv3cpuif.h"

#define MILLISECOND 0x186A0		//100000 assuming 100MHz
#define DUALTIMER0BASE 0x9C110000
#define SYSLEDBASE 0x9C010000

extern void GIC_configure(void);
extern void GIC_enableDualTimer0Interrupt(void);
void enableDualTimer0(unsigned int);


int main(void)
{


	int a=10;
	int b=20;
	int c=a+b;
    /* Configure GIC */
	GIC_configure();

	enableDualTimer0(0x29aa);
	GIC_enableDualTimer0Interrupt();

	
	/* Enable IRQ and FIQ in SVC mode */
	__asm volatile ("CPSIE if");

	while (1)
	{
		__asm volatile ("WFI");
	}
	
    return 0;
}

// ARM Dual-Timer Module (SP804)
// See https://developer.arm.com/documentation/ddi0271/d/programmer-s-model/summary-of-registers
void enableDualTimer0(unsigned int period)
{
	volatile uint32_t* DualTimer0 = (uint32_t*)DUALTIMER0BASE;
	*(DualTimer0+0x2) = 0x0;	// Disable timer
	*(DualTimer0+0x0) = period;
	*(DualTimer0+0x2) = 0xe2;	// Enable timer, periodic mode, enable interrupts, 32-bit counter
}

void DualTimer0IRQhandler(void)
{
	volatile uint32_t* DualTimer0 = (uint32_t*)DUALTIMER0BASE;
	volatile uint32_t* SysLed = (uint32_t*)SYSLEDBASE;
	*(DualTimer0+0x3) = 0x0;	// Clear timer interrupt

    // Advance the LEDs
	if (*(SysLed+0x2)!= 0x80)
		*(SysLed+0x2) = *(SysLed+0x2)<<1;
	else
		*(SysLed+0x2) = 0x1;
}
