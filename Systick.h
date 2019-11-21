/*
 *File name: Systick.h
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: header file for systick.c
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.04
 *From Assignment 1
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

// SysTick Registers
// SysTick Control and Status Register (STCTRL)
#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// Systick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

// SysTick defines
#define ST_CTRL_COUNT       0x00010000  // Count Flag for STCTRL
#define ST_CTRL_CLK_SRC     0x00000004  // Clock Source for STCTRL
#define ST_CTRL_INTEN       0x00000002  // Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE      0x00000001  // Enable for STCTRL

// Maximum period
#define MAX_WAIT            0x27100     // 1/100 second

#define MAX_T_SEC           9           // max value of 1/10 second
#define MAX_SEC             59          // max value of second
#define MAX_MIN             59          // max value of minute
#define MAX_HOUR            23          // max value of hour
#define NUM_OF_MON          12          // number of month
#define NUM_TYPE_OF_MON     2           // leap year month and normal year month

typedef struct {
    int t_sec,  //1/10 of second
        sec,    // second
        min,    // minute
        hour,   // hour
        day,    // day
        month,  // month integer
        year;   // year
} Systick_Clock;// Clock struct

void SysTickInit(); // initialization
void IncreaseTime(int hour, int min, int sec, int t, Systick_Clock* c);
int  IsDateVaild(int y/*year*/, int m/*month*/, int d/*day*/); //Check if date valid

#endif /* SYSTICK_H_ */
