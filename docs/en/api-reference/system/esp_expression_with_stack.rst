Call function with external stack
=================================

Overview
--------

A given function can be executed with a user allocated stack space
which is independent of current task stack, this mechanism can be
used to save stack space wasted by tasks which call a common function
with intensive stack usage such as `printf`. The given function can
be called inside the macro :cpp:func:`ESP_EXECUTE_EXPRESSION_WITH_STACK` 
it will redirect the target function to be executed using the space
allocated by the user.

Usage
-----

:cpp:func:`ESP_EXECUTE_EXPRESSION_WITH_STACK` takes three arguments, 
a mutex object allocated by the caller, which is used to protect if 
the same function shares its allocated stack, a pointer to the top 
of stack used to that fuction, and the function itself, note the
function is passed exactly in the same way as do when you call 
it on a regular way. 

The usage may looks like the code below:

.. code-block:: c

    //Let's suppose we wanting to call printf using a separated stack space
    //allowing app to reduce its stack size.
    void app_main()
    {
        //Allocate a stack buffer, from heap or as a static form:
        portSTACK_TYPE *shared_stack = malloc(8192 * sizeof(portSTACK_TYPE));
        assert(shared_stack != NULL);

        //Allocate a mutex to protect its usage:
        SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
        assert(printf_lock != NULL);
     
        //Call the desired function using the macro helper:
        ESP_EXECUTE_EXPRESSION_WITH_STACK(printf_lock, 
                                        shared_stack, 
                                        printf("Executing this from external stack! \n"));
        vSemaphoreDelete(printf_lock);    
        free(shared_stack); 
    }

.. _esp-call-with-stack-basic_usage:

API Reference
-------------

.. include-build-file:: inc/esp_expression_with_stack.inc


