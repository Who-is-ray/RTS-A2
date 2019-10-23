/*
 * Application.c
 *
 *  Created on: Sep 26, 2019
 *      Author: Ray
 */

#include <string.h>
#include "Application.h"
#include "Queue.h"
#include "Uart.h"
#include "Systick.h"

#define STRING_SIZE             64          // size of string to store
#define ALARM_CMD_SIZE          5           // size of alarm cmd
#define ALARM_PARA_CMD_SIZE     16          // size of alarm cmd with parameter
#define TIME_CMD_SIZE           4           // size of time cmd
#define TIME_PARA_CMD_SIZE      15          // size of time cmd with parameter
#define DATE_CMD_SIZE           4           // size of date cmd
#define DATE_PARA_CMD_SIZE1     15          // size of date cmd with parameter of 1 digit date
#define DATE_PARA_CMD_SIZE2     16          // size of date cmd with parameter of 2 digits date
#define NUM_OF_CHAR_IN_MON      3           // number of letter in month
#define NUM_OF_TIME_ELEMENT     4           // number of time elements
#define NUM_OF_DATE_ELEMENT     3           // number of date elements
#define DATA_COUNT_INIT_VAL     -1          // data_count initial value
#define TIME_HOUR               0           // position of hour in time array
#define TIME_MIN                1           // position of minute in time array
#define TIME_SEC                2           // position of second in time array
#define TIME_T_SEC              3           // position of tenths second in time array
#define SCREEN_BOTTOM           24          // the line of screen bottom

// ASCII Table Define
#define COMMON_CHAR_START       32          // char can direct echo start from 32(' ')
#define COMMON_CHAR_END         126         // char can direct echo end at 126('~')
#define NUMBER_START            48          // number start from 47(0)
#define NUMBER_END              57          // number start from 58(0)
#define ALPHABET_UC_START       65          // alphabet upper case character start from 122('A')
#define ALPHABET_UC_END         90          // alphabet lower case character end at 122('Z')
#define ALPHABET_LC_START       97          // alphabet lower case character start from 97('a')
#define ALPHABET_LC_END         122         // alphabet lower case character end at 122('z')
#define ALPHABET_CASE_OFFSET    0b11011111  // and value upper case
#define LINE_FEED               10
#define ENTER                   13
#define SPACE                   32
#define DASH                    45
#define PERIOD                  46
#define COLON                   58
#define BACKSPACE               127
#define ESC                     27
#define BEL                     7
#define EQUAL                   0

extern Systick_Clock clock;
extern Systick_Clock alarm;

char str[STRING_SIZE];      // command string
int str_counter = 0;        // command string letter counter
int cursor_line = 1;        // the line of cursor
int is_ESC_seq = FALSE;     // flag to indicate if is ESC sequences
int is_alarm_active = FALSE;// is alarm turned on
int day,                    // day
    mon_int,                // month integer
    year = 0;               // year
int time[NUM_OF_TIME_ELEMENT]; // time array: {hour,min,sec,t_sec}
char mon_str[NUM_OF_CHAR_IN_MON]; //month string
const char time_symbol[]={SPACE,COLON,COLON,PERIOD}; // time symbol array: {' ',':',':','.'}
const char mon_list[NUM_OF_MON][NUM_OF_CHAR_IN_MON]={"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"}; // list of month

//****************************************************************************
//
// Local functions
//
//*****************************************************************************

/* Transmit a character*/
void TransChar(char c)
{
    while(EnQueue(OUTPUT, UART, c)==FALSE); // wait until it is enqueued
}

/* Output a string*/
void OutputString(const char* s)
{
    int i;
    for(i = 0; i <= strlen(s); i++) // output each character in string
        TransChar(s[i]);
}

/* Move cursor to new line*/
void OutputNewLine()
{
    TransChar(ENTER);
    TransChar(LINE_FEED);
}

/* Move cursor to new line and output prefix*/
void OutputNewLinePrefix()
{
    OutputNewLine();
    OutputString("> ");
}

/* Output current time*/
void OutputTime(Systick_Clock c)
{
    TransChar(c.hour/10+NUMBER_START);
    TransChar(c.hour%10+NUMBER_START);
    OutputString(":");
    TransChar(c.min/10+NUMBER_START);
    TransChar(c.min%10+NUMBER_START);
    OutputString(":");
    TransChar(c.sec/10+NUMBER_START);
    TransChar(c.sec%10+NUMBER_START);
    OutputString(".");
    TransChar(c.t_sec+NUMBER_START);
}

/* Output current date*/
void OutputCurrentDate()
{
    if(clock.day/10)// if two digit date
        TransChar(clock.day/10+NUMBER_START);
    TransChar(clock.day%10+NUMBER_START); // output ones digit date
    OutputString("-");

    //Output month string
    char month_str[3];
    strncpy(month_str, mon_list[clock.month-1],3);
    OutputString(month_str);

    OutputString("-");
    TransChar(clock.year/1000+NUMBER_START);
    TransChar((clock.year/100)%10+NUMBER_START);
    TransChar((clock.year/10)%100+NUMBER_START);
    TransChar(((clock.year%1000)%100)%10+NUMBER_START);
}

/* Indicate if the character is a number*/
int IsNumber(char c)
{
    if(c>=NUMBER_START && c<=NUMBER_END)
        return TRUE;
    return FALSE;
}

/* Indicate if the character is an upper case letter*/
int IsUCLetter(char c)
{
    if(c>=ALPHABET_UC_START && c<=ALPHABET_UC_END)
        return TRUE;
    return FALSE;
}

/* Decoding the time from command
 * return true if invalid*/
int DecodeTime(char str[], int char_count, int str_len)
{
    int data_count = DATA_COUNT_INIT_VAL;
    int is_data_updated = FALSE;

    // clear read time
    int i;
    for(i = 0; i<NUM_OF_TIME_ELEMENT; i++)
        time[i] = 0;

    while(char_count<str_len) // decode string after "time" or "alarm"
    {
        if(str[char_count] != time_symbol[data_count+1]) // test symbol
            return TRUE ;
        else
            data_count++;

        char_count++;
        if(char_count>=str_len)
            break;

        if(IsNumber(str[char_count])) // if next bit is a number
        {
            time[data_count] = str[char_count] - NUMBER_START;
            char_count++;
            if((char_count<str_len) && IsNumber(str[char_count])) // test if next bit exist and is a number
            {
                time[data_count] *= 10; // set to value of tens digit
                time[data_count] += str[char_count] - NUMBER_START;
                char_count++;
            }
            is_data_updated=TRUE;
        }
        else //not a number
            time[data_count] = 0;
    }

    if(is_data_updated) // if data updated
        // if data valid
        if((time[TIME_HOUR]>MAX_HOUR)   ||
           (time[TIME_MIN]>MAX_MIN)     ||
           (time[TIME_SEC]>MAX_SEC)     ||
           (time[TIME_T_SEC]>MAX_T_SEC))
            return TRUE;

    return FALSE;
}

/* Decoding the time from command
 * return true if invalid*/
int DecodeDate(char str[], int count)
{
    if(IsNumber(str[count-1])) // test last bit: ones digit of year
    {
        year = str[count-1] - NUMBER_START;
        count--;
    }
    else // if invalid
        return TRUE;

    if(IsNumber(str[count-1])) // test last second bit: tens digit of year
    {
        year += (str[count-1] - NUMBER_START)*10;
        count--;
    }
    else // if invalid
        return TRUE;

    if(IsNumber(str[count-1])) // test last thrid bit: hundreds digit of year
    {
        year += (str[count-1] - NUMBER_START)*100;
        count--;
    }
    else // if invalid
        return TRUE;

    if(IsNumber(str[count-1])) // test last forth bit: thousands digit of year
    {
        year += (str[count-1] - NUMBER_START)*1000;
        count--;
    }
    else // if invalid
        return TRUE;

    if(str[count-1] == DASH) // test last fifth bit: dash
        count--;
    else // if invalid
        return TRUE;

    int i;
    for(i=NUM_OF_CHAR_IN_MON-1; i>=0; i--) // read month
    {
        mon_str[i] = str[count-1];
        count--;
    }
    int not_match = TRUE; // flag to indicate if month exist
    for(mon_int=1; mon_int<=NUM_OF_MON; mon_int++) // find month in month list
    {
        if(strncmp(mon_str, mon_list[mon_int-1],NUM_OF_CHAR_IN_MON) == EQUAL) // if find in month list
        {
            not_match = FALSE;
            break;
        }
    }
    if(not_match) // if not a valid month
        return TRUE;

    if(str[count-1] == DASH) // test last ninth bit: dash
        count--;
    else // if invalid
        return TRUE;

    if(IsNumber(str[count-1])) // test last tenth bit: last digit of date
    {
        day = str[count-1] - NUMBER_START;
        count--;
    }
    else // if invalid
        return TRUE;

    if(IsNumber(str[count-1])) // test last eleventh bit: first digit of date
    {
        day += (str[count-1] - NUMBER_START)*10;

        if(IsDateVaild(year,mon_int,day)==FALSE) // check if date is invalid
            return TRUE;

        count--;
        if(str[count-1] == SPACE) // test last twelfth bit: space
            return FALSE;
    }
    else if(str[count-1] == SPACE || strlen(str)==DATE_PARA_CMD_SIZE1) // test last eleventh bit: space
        return FALSE;
    return TRUE;
}

/* check is alarm triggered*/
void CheckAlarm()
{
    if( (is_alarm_active==TRUE)     &&
        (alarm.hour==clock.hour)    &&
        (alarm.min==clock.min)      &&
        (alarm.sec==clock.sec)      &&
        (alarm.t_sec==clock.t_sec)  &&
        (alarm.day==clock.day)      &&
        (alarm.month==clock.month)  &&
        (alarm.year==clock.year)) // if should triggered
    {
        OutputNewLine();
        OutputString("* ALARM * ");
        OutputTime(clock);
        OutputString(" *");
        OutputNewLinePrefix();
        TransChar(BEL); // output audible beep
        is_alarm_active = FALSE; // turn off alarm
    }
}

/* Initialization Uart, Systick and Queues
 * Enable interrupts*/
void Initialization()
{
    /* Initialize UART */
    UART0_Init();           // Initialize UART0
    Queue_Init();           // Initialize Queues
    InterruptEnable(INT_VEC_UART0);       // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts
    SysTickInit();  // Enable Systick
    InterruptMasterEnable();    // Enable Master (CPU) Interrupts
    OutputString("> ");     // Output first pre-fix
}

/* check if input queue has data to process
 * process if has*/
void CheckInputQueue()
{
    int need_echo = FALSE;    // flag to indicate if ehco back

    /* Input data - xmit directly */
    QueueData data;
    if(DeQueue(INPUT,&data.source,&data.value) == TRUE) // If input is not empty
    {
        switch(data.source)
        {
            case UART:// if is UART
            {
                char data_val=data.value;
                int has_error = FALSE; // flag indicate if cmd has error

                /*process the input*/
                if(is_ESC_seq == TRUE)   // if is a part of ESC sequence
                {
                   char data_val_uc = data_val & ALPHABET_CASE_OFFSET;    // convert to upper case letter
                   if(IsUCLetter(data_val_uc))
                       is_ESC_seq = FALSE;  // most case ESC sequences ended with a letter
                   need_echo = TRUE;
                }
                else if(data_val>=COMMON_CHAR_START && data_val<=COMMON_CHAR_END) // if data is common char
                {
                   if(str_counter<STRING_SIZE) // if string not full
                   {
                       // store to string and echo back
                       if((data_val>=ALPHABET_LC_START) && (data_val<=ALPHABET_LC_END)) // if is a lower case letter
                           data_val &= ALPHABET_CASE_OFFSET;    // convert to upper case letter

                       str[str_counter]=data_val;
                       str_counter++;
                       need_echo = TRUE;
                   }
                }
                else if(data_val == BACKSPACE)   // if data is backspace
                {
                   if(str_counter >0)    // if string not empty
                   {
                       str_counter--;   // remove last bit from string
                       need_echo = TRUE;
                   }
                }
                else if(data_val == ENTER)   // if data is enter
                {
                    OutputNewLine(); // change to new line
                    int count = str_counter; // save string count to local
                    str_counter = 0; // reset the string counter

                    /* Process the string */
                    if (strncmp(str, "TIME", TIME_CMD_SIZE)==EQUAL) // if start with 'TIME'
                    {
                       if(count == TIME_CMD_SIZE)// if has no parameter
                       {
                           OutputTime(clock);
                           OutputNewLinePrefix();
                       }
                       else if (count > TIME_CMD_SIZE && count <= TIME_PARA_CMD_SIZE)// if has parameters
                       {
                           has_error = DecodeTime(str,TIME_CMD_SIZE,count);
                           if(has_error == FALSE) // if time valid
                           {
                               // set to clock
                               clock.t_sec  = time[TIME_T_SEC];
                               clock.sec    = time[TIME_SEC];
                               clock.min    = time[TIME_MIN];
                               clock.hour   = time[TIME_HOUR];

                               // Output set time
                               OutputTime(clock);
                               OutputNewLinePrefix();
                               return;
                           }
                       }
                       else
                           has_error = TRUE;
                    }
                    else if (strncmp(str, "DATE", DATE_CMD_SIZE)==EQUAL) // if start with 'DATE'
                    {
                       if(count == DATE_CMD_SIZE)// if has no parameter
                       {
                           OutputCurrentDate();
                           OutputNewLinePrefix();
                       }
                       else if ((count == DATE_PARA_CMD_SIZE1) || (count == DATE_PARA_CMD_SIZE2)) // if has parameters
                       {
                           has_error = DecodeDate(str,count);
                           if(has_error == FALSE) // if date valid
                           {
                               // set to clock
                               clock.day = day;
                               clock.month = mon_int;
                               clock.year = year;

                               // Output set time
                               OutputCurrentDate();
                               OutputNewLinePrefix();
                               return;
                           }
                       }
                       else // error
                           has_error = TRUE;
                    }
                    else if (strncmp(str, "ALARM", ALARM_CMD_SIZE)==EQUAL) // if start with 'ALARM'
                    {
                       if(count == ALARM_CMD_SIZE)// if has no parameter
                       {
                           // clear alarm and output msg
                           is_alarm_active = FALSE;
                           OutputString("Alarm cleared");
                           OutputNewLinePrefix();
                       }
                       else  if (count > ALARM_CMD_SIZE && count <= ALARM_PARA_CMD_SIZE)// if has parameters
                       {
                           has_error = DecodeTime(str,ALARM_CMD_SIZE,count);
                           if(has_error == FALSE) // if time valid
                           {
                               // add to alarm
                               alarm = clock;
                               IncreaseTime(time[TIME_HOUR],time[TIME_MIN],time[TIME_SEC],time[TIME_T_SEC],&alarm);
                               is_alarm_active = TRUE;

                               // Output set time
                               OutputString("Alarm at ");
                               OutputTime(alarm);
                               OutputNewLinePrefix();
                               return;
                           }
                       }
                       else
                           has_error = TRUE;
                    }
                    else // error
                       has_error = TRUE;
                }
                else if(data_val == ESC) // if data is ESC
                {
                   is_ESC_seq = TRUE;
                   need_echo = TRUE;
                }

                /*After processed command*/
                if(need_echo) // echo if need to
                {
                   TransChar(data.value); // echo back
                   need_echo = FALSE;
                }
                else if(has_error) // report error
                {
                   OutputString("?");
                   OutputNewLinePrefix();
                }
                break;
            }
            case SYSTICK: // if is systick
            {
                IncreaseTime(0,0,0,1,&clock);
                CheckAlarm();
                break;
            }
        }
    }
}


//****************************************************************************
//
// Public functions
//
//*****************************************************************************

/* Run application*/
void Run()
{
    Initialization();

    while(1)
        CheckInputQueue();
}
