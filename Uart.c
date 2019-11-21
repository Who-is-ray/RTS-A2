/**
 *File name: Uart.c
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: this file includes all the settings of UART, to enable the interrupt and build the handler.
 *          this is kind of upversion of assignment 2.
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.04
 *  From Assignment 1
 */

#include "Uart.h"
#include "Queue.h"

volatile int UART_STATUS = IDLE;

void UART0_Init(void)
{
	volatile int wait;

	/* Initialize UART0 */
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;   // Enable Clock Gating for UART0
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;   // Enable Clock Gating for PORTA
	wait = 0; // give time for the clocks to activate

	UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
	wait = 0;   // wait required before accessing the UART config regs

	// Setup the BAUD rate
	UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
	UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

	UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

	GPIO_PORTA_AFSEL_R = 0x3;        // Enable Receive and Transmit on PA1-0
	GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);         // Enable UART RX/TX pins on PA1-0
	GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;        // Enable Digital I/O on PA1-0

	UART0_CTL_R = UART_CTL_UARTEN;        // Enable the UART
	wait = 0; // wait; give UART time to enable itself.
}

void InterruptEnable(unsigned long InterruptIndex)
{
	/* Indicate to CPU which device is to interrupt */
	if (InterruptIndex < 32)
		NVIC_EN0_R = 1 << InterruptIndex;       // Enable the interrupt in the EN0 Register
	else
		NVIC_EN1_R = 1 << (InterruptIndex - 32);    // Enable the interrupt in the EN1 Register
}

void UART0_IntEnable(unsigned long flags)
{
	/* Set specified bits for interrupt */
	UART0_IM_R |= flags;
}

void UART0_IntDisable(unsigned long flags)
{
	/* Clear specified bits for interrupt */
	UART0_IM_R &= ~flags;
}

void UART0_IntHandler(void)
{
	/*
	 * Simplified UART ISR - handles receive and xmit interrupts
	 * Application signalled when data received
	 */
	if (UART0_MIS_R & UART_INT_RX)
	{
		/* RECV done - clear interrupt and make char available to application */
		UART0_ICR_R |= UART_INT_RX;
		EnQueueIO(INPUT, UART, UART0_DR_R);
	}

	if (UART0_MIS_R & UART_INT_TX)
	{
		/* XMIT done - clear interrupt */
		UART0_ICR_R |= UART_INT_TX;

		char data;
		if (DeQueueIO(OUTPUT, UART, &data)) // if output queue is not empty
			UART0_DR_R = data;  // transmit next data
		else // if output queue is empty
			UART_STATUS = IDLE; // idle
	}
}

void InterruptMasterEnable(void)
{
	/* enable CPU interrupts */
	__asm(" cpsie   i");
}

void InterruptMasterDisable(void)
{
	/* disable CPU interrupts */
	__asm(" cpsid   i");
}

