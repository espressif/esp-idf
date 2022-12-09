# Component: Button

* This component defines a button as a well encapsulated object.
* A button device is defined by:
	* GPIO number on which the button is attached.
	* Active level which decided by peripheral hardware.
    * Trigger mode which decides whether to call serial trigger callback during pressing
    * Serial threshold seconds which decides that serial trigger callback will be called after how many seconds' pressing
* A button device can provide:
    * One push event callback
    * One release event callback
    * One short-time tap event callback
    * One serial trigger event callback
    * Several long-time press event callback
    We can set different jitter filters for all the events.
    Once any of the long press callback is triggered, the short tap event will not be triggered.
    This components are based on GPIO provided by idf and soft timer provided by FreeRTOS.
    
* To use the button device, you need to :
	* create a button object returned by iot_button_create().
	* Then hook different event callbacks to the button object.
	* To free the object, you can call iot_button_delete to delete the button object and free the memory that used.
	
* Todo:
    * Add hardware timer mode(because sometimes soft-timer callback function is limited)
    
### NOTE:
> All the event callback function are realized by FreeRTOS soft timer APIs, the callback must follow the rule: 



```
  Button callback functions execute in the context of the timer service task.
  It is therefore essential that button callback functions never attempt to block.
  For example, a button callback function must not call vTaskDelay(), vTaskDelayUntil(), or specify a non zero block time when accessing a queue or a semaphore.
```

> In addition:
> You can adjust the following macros within FreeRTOS to adjust the stack depth/queue length/task priority of the timer service.


```
#define configUSE_TIMERS //enable soft-timer
#define configTIMER_TASK_PRIORITY // priority of the timers service task
#define configQueue_LENGTH // length of timer command queue
#define configTIMER_TASK_STACK_DEPTH // stack depth of the soft-timer
```