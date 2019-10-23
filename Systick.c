/*
 * Systick.c
 *
 *  Created on: Sep 28, 2019
 *      Author: Ray
 */

#include "Systick.h"
#include "Queue.h"
#include "Uart.h"

#define CENTURY_LEAP_YEAR_PERIOD    400         // century leap year period
#define LEAP_YEAR_PERIOD            4           // leap year period
#define DEFAULT_YEAR                2019
#define DEFAULT_MON                 9
#define DEFUALT_DAY                 1
#define NULL                        0

const int days_list[NUM_TYPE_OF_MON][NUM_OF_MON] = {  // list of possible day list
                              {31,28,31,30,31,30,31,31,30,31,30,31}, // list of days in month,days_in_month_ly
                              {31,29,31,30,31,30,31,31,30,31,30,31}}; // list of days in month in leap year

volatile Systick_Clock clock = {.day         = DEFUALT_DAY,
                                .month       = DEFAULT_MON,
                                .year        = DEFAULT_YEAR,
                                .hour        = 0,
                                .min         = 0,
                                .sec         = 0,
                                .t_sec       = 0};

volatile Systick_Clock alarm = {.day         = 0,
                                .month       = 0,
                                .year        = 0,
                                .hour        = 0,
                                .min         = 0,
                                .sec         = 0,
                                .t_sec       = 0};
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
    EnQueue(INPUT, SYSTICK, NULL);
}

void SysTickInit()
{
    SysTickPeriod(MAX_WAIT);
    SysTickIntEnable();
    SysTickStart();
}

int IsDateVaild(int y/*year*/, int m/*month*/, int d/*day*/)
{
    return ((days_list[(((y%LEAP_YEAR_PERIOD)==0)&&(y%CENTURY_LEAP_YEAR_PERIOD)>0)? TRUE:FALSE][m-1])<d)? FALSE:TRUE;
}

/* Increase date by 1*/
void IncreaseDate(Systick_Clock* c)
{
    if((days_list[(c->year%LEAP_YEAR_PERIOD)>0? FALSE:TRUE][c->month-1]) == c->day) // if the last day at the month
    {
        c->day =1;
        if(c->month == NUM_OF_MON)
        {
            c->month = 1;
            c->year++;
        }
        else
            c->month++;
    }
    else
        c->day++;
}

/* Add 'hour:min:sec.t' to selected time*/
void IncreaseTime(int hour, int min, int sec, int t, Systick_Clock* c)
{
    // add tenth second
    int remainder = t+c->t_sec-MAX_T_SEC;
    if(remainder > 0) //if overflow
    {
        c->t_sec = remainder-1;
        sec++;
    }
    else c->t_sec+= t;

    // add second
    remainder = sec+c->sec-MAX_SEC;
    if (remainder > 0) //if overflow
    {
        c->sec = remainder-1;
        min++;
    }
    else c->sec+= sec;

    // add minute
    remainder = min+c->min-MAX_MIN;
    if(remainder > 0) //if overflow
    {
        c->min = remainder-1;
        hour++;
    }
    else c->min+= min;

    // add hour
    remainder = hour+c->hour-MAX_HOUR;
    if (remainder>0) //if overflow
    {
        c->hour = remainder-1;
        IncreaseDate(c);
    }
    else c->hour+=hour;
}
