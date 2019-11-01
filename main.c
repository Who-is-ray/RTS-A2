/**
 * Real Time System Assignment 2
 *
 * This program is a
 *
 * Author: Ray Su B00634512
 * Fall 2019
 */

//*********To Remove**********//
#include "Queue.h"
#include "Systick.h"
//*********To Remove**********//

#include <stdio.h>
#include "Uart.h"

/* Initialization Uart, Systick and Queues
 * Enable interrupts*/
void Initialization()
{
    /* Initialize UART */
    UART0_Init();           // Initialize UART0

    // To update
    Queue_Init();           // Initialize Queues
    InterruptEnable(INT_VEC_UART0);       // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts
    //SysTickInit();  // Enable Systick
    InterruptMasterEnable();    // Enable Master (CPU) Interrupts
    //OutputString("> ");     // Output first pre-fix


}

void main (void)
{
    Initialization();

    /* From A1
    while(1)
        CheckInputQueue();
    */
}
