/* FreeRTOS Task example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/***********************************
 *FreeRTOS First task example 
 *Author: Mahendra Sondagar
 *mahendrasondagar08@gmail.com
 * ********************************/

/* FreeRTOS task handler */
TaskHandle_t th1=NULL;

/*FreeRTOS Thread fucntion*/
void Thread1(void)
{
	for(;;)
	{
		printf("FreeRTOS Thread is running...\r\n");
		vTaskDelay(250/portTICK_PERIOD_MS);
	}

}


void app_main(void)
{

    BaseType_t xReturn= NULL;
	/* FreeRTOS task creation */

	xReturn = xTaskCreate(Thread1, /*Thread Callback function */
		   "My_thread",            /*Thread name for ease of debug */
		    2048,                  /*Thread allocated stack memory */
		    NULL,                  /*Thread argument parameter */
		    1,                     /*Thread priority */
		    &th1);                 /*Thread handler */
         configASSERT( th1 );

	if(xReturn==NULL)
	{
		printf("Failed to create the FreeRTOS Task!\r\n");
		vTaskDelete(th1);
	}

}