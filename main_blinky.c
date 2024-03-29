/*
 * FreeRTOS V202107.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/******************************************************************************
 * NOTE 1: The FreeRTOS demo threads will not be running continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Linux port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Linux
 * port for further information:
 * https://freertos.org/FreeRTOS-simulator-for-Linux.html
 *
 * NOTE 2:  This project provides two demo applications.  A simple blinky style
 * project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky version.  Console output
 * is used in place of the normal LED toggling.
 *
 * NOTE 3:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, are defined
 * in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, one software timer, and two tasks.  It then
 * starts the scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  It uses vTaskDelayUntil() to create a periodic task that sends
 * the value 100 to the queue every 200 milliseconds (please read the notes
 * above regarding the accuracy of timing under Linux).
 *
 * The Queue Send Software Timer:
 * The timer is an auto-reload timer with a period of two seconds.  The timer's
 * callback function writes the value 200 to the queue.  The callback function
 * is implemented by prvQueueSendTimerCallback() within this file.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() waits for data to arrive on the queue.
 * When data is received, the task checks the value of the data, then outputs a
 * message to indicate if the data came from the queue send task or the queue
 * send software timer.
 *
 * Expected Behaviour:
 * - The queue send task writes to the queue every 200ms, so every 200ms the
 *   queue receive task will output a message indicating that data was received
 *   on the queue from the queue send task.
 * - The queue send software timer has a period of two seconds, and is reset
 *   each time a key is pressed.  So if two seconds expire without a key being
 *   pressed then the queue receive task will output a message indicating that
 *   data was received on the queue from the queue send software timer.
 *
 * NOTE:  Console input and output relies on Linux system calls, which can
 * interfere with the execution of the FreeRTOS Linux port. This demo only
 * uses Linux system call occasionally. Heavier use of Linux system calls
 * may crash the port.
 */

#include <stdio.h>
#include <pthread.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"

/* Priorities at which the tasks are created. */
#define QUEUE_TASK_PRIORITY1  (tskIDLE_PRIORITY + 1)
#define QUEUE_TASK_PRIORITY2  (tskIDLE_PRIORITY + 2)
#define QUEUE_TASK_PRIORITY3  (tskIDLE_PRIORITY + 3)
#define QUEUE_TASK_PRIORITY4  (tskIDLE_PRIORITY + 4)

/* Periods for the tasks. */
#define TASK1_PERIOD    pdMS_TO_TICKS(200)
#define TASK2_PERIOD    pdMS_TO_TICKS(500)
#define TASK3_PERIOD    pdMS_TO_TICKS(1000)
#define TASK4_PERIOD    pdMS_TO_TICKS(100)



/* The number of items the queue can hold at once. */


static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);
static void vTask3(void *pvParameters);
static void vTask4(void *pvParameters);

void main_blinky(void);

void main_blinky(void) {

    /* Create the tasks as described in the comments. */
    xTaskCreate(vTask1,
                "Task 1",
                configMINIMAL_STACK_SIZE,
                NULL, 
                QUEUE_TASK_PRIORITY1,
                NULL);
    xTaskCreate(vTask2,
                "Task 2", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                QUEUE_TASK_PRIORITY2, 
                NULL);
    xTaskCreate(vTask3,
                "Task 3", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                QUEUE_TASK_PRIORITY3, 
                NULL);
    xTaskCreate(vTask4, 
                "Task 4", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                QUEUE_TASK_PRIORITY4, 
                NULL);

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* The scheduler should never return. If it does, there was not enough
     * heap available for the idle and/or timer tasks to be created. See
     * the memory management section on the FreeRTOS website. */
    for (;;) {
    }
}

static void vTask1(void *pvParameters) {
    for (;;) {
        printf("Task 1 : Completed. \n");
        vTaskDelay(TASK1_PERIOD);
    }
}

static void vTask2(void *pvParameters) {
    const float fVal = 9120;

    for (;;) {
        float cVal = (fVal - 32) * 5 / 9.0;
        printf("The temperature %.2f in Fahrenheit is equivalent to  %.2f in Celsius\n", fVal, cVal);
        vTaskDelay(TASK2_PERIOD);
    }
}

static void vTask3(void *pvParameters) {
    const long int firstNum = 1000000000;
    const long int secondNum = 2564851111;

    for (;;) {
        long int resultMulti = firstNum * secondNum;
        printf("The result of the multiplication is : %ld\n", secondNum);
        vTaskDelay(TASK3_PERIOD);
    }
}

static void vTask4(void *pvParameters) {
    for (;;){
    int arr[50];
    int i = 0;

    for (i; i < 50; i++) {
        arr[i] =  i;
    }

    int targetNumber = 36;
    int lowBound = 0;
    int highBound = 49;
    int result = -1;

    for (;;) {
        int mid = lowBound + (highBound - lowBound) / 2;

        if (arr[mid] == targetNumber) {
            result = mid;
            break;
        }

        if (arr[mid] < targetNumber)
            lowBound = mid + 1;
        else
            highBound = mid - 1;
    }

    if (result != -1)
        printf("The element %d is found at the index %d.\n", targetNumber, result);
    else
        printf("The element %d is not found in the list.\n", targetNumber);
    
    vTaskDelay(TASK4_PERIOD);
    }
}
