#include<stdio.h>
#include"freertos/task.h"


TaskHandle_t th1 = NULL;
void thread1(void * p)
{
    int count = (int *)p;
    while(1)
    {
        printf("thread1 is running : %d:\n",count++);
        vTaskDelay(1000);
    }
}

int app_main()
{
    int myCount = 7;
    int xreturn=0;
    xreturn = xTaskCreate(thread1,"thread_1",2048,myCount,1,&th1);
    if(xreturn == NULL)
    {
        printf("Task creation failed\n");
        vTaskDelete(th1);
    }


    vTaskStartScheduler();
    while(1)
    {
        printf("main is running\n");
        vTaskDelay(1000);
    }
}





/* FreeRTOS Task example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


/*

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// FreeRTOS task handler 
TaskHandle_t th1=NULL;
TaskHandle_t th2 =NULL;

// FreeRTOS Thread fucntion
void Thread1(void)
{
	for(;;)
	{
		printf("FreeRTOS Thread is running...\r\n");
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}

}

void thread2(void)
{
    while(1)
    {
        printf("FreeRTOS thread 2 is running...\r\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}


void app_main(void)
{

    BaseType_t xReturn= NULL;

	// FreeRTOS task creation 

	xReturn = xTaskCreate(Thread1, //Thread Callback function 
		   "My_thread",            //Thread name for ease of debug
		    // 2048,                  //Thread allocated stack memory
		    NULL,                  //Thread argument parameter 
             1,                     //Thread priority 
		    &th1);                 //Thread handler 
         configASSERT( th1 );

	if(xReturn==NULL)
	{
		printf("Failed to create the FreeRTOS Task!\r\n");
		vTaskDelete(th1);
	}

    xReturn = xTaskCreate(thread2, //Thread Callback function 
    "my_thread2",
    2048,
    NULL,
    2,
    &th2);

    if(xReturn==NULL)
    {
        printf("Faild to create the free RTOS task 2\r\n");
        vTaskDelete(th2);
    }

}
*/