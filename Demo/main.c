// This file is modified by Wan-Ting CHEN (2016)
// for the Peer Graded Assignment: Assignment 1
// of the MOOC on Coursera : Development of Real-Time Systems

/*
Copyright 2013, Jernej Kovacic

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stddef.h>

#include <FreeRTOS.h>
#include <task.h>

#include "app_config.h"
#include "print.h"

#define     STRING_LENGTH   32

// The type funcVariable is used to provide the string which should be printed and the delay time
typedef struct {
    char str[STRING_LENGTH]; // string sent to UART (shown on screen if using QEMU)
    unsigned int sleepTime;  // delay time (ms)
} funcVariable;

// The type taskVariable is used to create the task as well as send the funcVariable to the function called by task.

typedef struct {
    char name[STRING_LENGTH];   // Task name
    unsigned int stackSize;     // Stack size (byte)
    unsigned int priority;      // Priority
    funcVariable fVar;          // function Variable
} taskVariable;


// to create the task with the pointer of task handle, pointer to array of task Variable, and number of task
void createTask(xTaskHandle*, taskVariable*, int); 
// The function used by the tasks created by createTask.
void printTask(void*);

/*
 * A convenience function that is called when a FreeRTOS API call fails
 * and a program cannot continue. It prints a message (if provided) and
 * ends in an infinite loop.
 */
static void FreeRTOS_Error(const portCHAR* msg)
{
    if ( NULL != msg )
    {
        vDirectPrintMsg(msg);
    }

    for ( ; ; );
}

/* Startup function that creates and runs two FreeRTOS tasks */
int main(void)
{
    /* Init of print related tasks: */
    if ( pdFAIL == printInit(PRINT_UART_NR) )
    {
        FreeRTOS_Error("Initialization of print failed\r\n");
    }

    /*
     * I M P O R T A N T :
     * Make sure (in startup.s) that main is entered in Supervisor mode.
     * When vTaskStartScheduler launches the first task, it will switch
     * to System mode and enable interrupt exceptions.
     */
    vDirectPrintMsg("= = = T E S T   S T A R T E D = = =\r\n\r\n");

    /* Init of receiver related tasks: */
    /*
    if ( pdFAIL == recvInit(RECV_UART_NR) )
    {
        FreeRTOS_Error("Initialization of receiver failed\r\n");
    }*/


    xTaskHandle myTaskHandle;

    taskVariable a[] = {{"Task1", 1000, 3, {"This is task 1\n", 100}},
                        {"Task2", 100, 1, {"This is task 2\n", 500}}};

    
    createTask(&myTaskHandle, a, sizeof(a)/sizeof(taskVariable));

    vTaskStartScheduler();

    /*
     * If all goes well, vTaskStartScheduler should never return.
     * If it does return, typically not enough heap memory is reserved.
     */

    FreeRTOS_Error("Could not start the scheduler!!!\r\n");

    /* just in case if an infinite loop is somehow omitted in FreeRTOS_Error */
    for ( ; ; );

    return 0;
}

void createTask(xTaskHandle* myTaskHandle, taskVariable* task, int N){
    BaseType_t xReturned;
    for (int i=0; i<N; ++i) {
        xReturned = xTaskCreate(printTask,                      // Pointer to the task entry function 
                                task[i].name,                   // name for the task
                                task[i].stackSize/sizeof(void*),// StackDepth, sizeof(void*) indicates if it is 2/4/8 byte
                                (void *) &task[i].fVar,         // variable needed by the created task.
                                task[i].priority,               // Task's priority
                                myTaskHandle);                  // Task handle

        if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {  // Print if tasks cannot be created.
            vDirectPrintMsg("errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY for task\n");
        }
    }
}

void printTask(void* fVar) {
    funcVariable* var = (funcVariable*) fVar;
    for( ;; ){
	    vDirectPrintMsg(var->str);
        vTaskDelay(var->sleepTime/portTICK_RATE_MS);            // delay 
    }
}

