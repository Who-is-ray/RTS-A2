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
#include "Process.h"

/* Initialization Uart, Systick and Queues
 * Enable interrupts*/
void Initialization()
{


    // To update
    Queue_Init();           // Initialize Queues

	enable();    // Enable Master (CPU) Interrupts
    //OutputString("> ");     // Output first pre-fix

	Initialize_Process();
}

void main (void)
{
    Initialization();

    /* From A1
    while(1)
        CheckInputQueue();
    */
}
