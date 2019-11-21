/*
 *File name: Systick.c
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: Initialization the systick, and provide the systick to support the switch process.
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Sep 28
 *From Assignment 1
 */

#include "Systick.h"
#include "Process.h"
#include "Queue.h"
#include "Uart.h"

#define NVIC_INT_CTRL_R (*((volatile unsigned long *) 0xE000ED04))
#define TRIGGER_PENDSV 0x10000000

volatile int PENDSV_ON;

void SysTickStart(void)
{
    // Set the clock source to internal and enable the counter to interrupt
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

void SysTickStop(void)
{
    // Clear the enable bit to stop the counter
    ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

void SysTickPeriod(unsigned long Period)
{
    /*
     For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
    */
    ST_RELOAD_R = Period - 1;  /* 1 to 0xff.ffff */
}

void SysTickIntEnable(void)
{
    // Set the interrupt bit in STCTRL
    ST_CTRL_R |= ST_CTRL_INTEN;
}

void SysTickIntDisable(void)
{
    // Clear the interrupt bit in STCTRL
    ST_CTRL_R &= ~(ST_CTRL_INTEN);
}

void SysTickHandler(void)
{
    if (PENDSV_ON == FALSE)
        NVIC_INT_CTRL_R |= TRIGGER_PENDSV;// Process switch, trigger PendSV after this function
}

void SysTickInit()
{
    SysTickPeriod(MAX_WAIT);
    SysTickIntEnable();
    SysTickStart();
}
