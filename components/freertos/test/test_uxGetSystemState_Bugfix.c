/*
 Test Bugfix for uxTaskGetSystemState where getting system state immediately after creating 
 new tasks would lead them being include twice in the TaskStatusArray. Changed suspendScheduler
 in function to taskENTER_CRITICAL
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

#define CreatedTaskDelayTicks 1
#define NumberOfTasksToCreate 4
#define CreatedTaskPriority 10
#define CoreToUse	1

#if (configUSE_TRACE_FACILITY)

void TaskCallback (void* pxParam){
	int counter = 0;
	while(1){
		counter++;
		vTaskDelay(CreatedTaskDelayTicks);
	}
}

TEST_CASE("uxTaskGetSystemState Bugfix Test", "[freertos]")
{
	TaskStatus_t *TaskStatusArray;
	TaskHandle_t *TaskHandles;
	UBaseType_t NumberOfTasks = 0;
	UBaseType_t StartingNumberOfTasks = 0;
	
	//Give Time for OS to remove dport tasks
	vTaskDelay(1000);
	
	//Allocate TaskStatusArray
	StartingNumberOfTasks = uxTaskGetNumberOfTasks();
	
	
	TaskStatusArray = pvPortMalloc((StartingNumberOfTasks+NumberOfTasksToCreate) * sizeof(TaskStatus_t));
	TaskHandles = pvPortMalloc((StartingNumberOfTasks+NumberOfTasksToCreate) * sizeof(TaskHandle_t));

	
	
	//Create Tasks
	for(int i = 0; i < NumberOfTasksToCreate; i++){
		xTaskCreatePinnedToCore(&TaskCallback, "Task" , 2048, NULL, CreatedTaskPriority, (TaskHandle_t*) &TaskHandles[i], CoreToUse);
	}
	NumberOfTasks = uxTaskGetSystemState(TaskStatusArray, (StartingNumberOfTasks+NumberOfTasksToCreate), NULL);
	
	//Check if any taska have been repeated in TaskStatusArray
	if(NumberOfTasks != 0){
		printf("Tasks Created, Checking for Repeated Additions \n");
		for(int i = 0; i <NumberOfTasks; i++){
			for(int j = i + 1;  j< NumberOfTasks; j++){
				//Give error task handle matches another
				configASSERT(!(TaskStatusArray[i].xHandle == TaskStatusArray[j].xHandle));
			}
				
		}
	}
	
	
}
#endif