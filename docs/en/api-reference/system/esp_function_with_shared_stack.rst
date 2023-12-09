Call Function with External Stack
=================================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

A given function can be executed with a user-allocated stack space which is independent of current task stack. This mechanism can be used to save stack space wasted by tasks which call a common function with intensive stack usage such as ``printf``. The given function can be called inside the shared stack space, which is a callback function deferred by calling :cpp:func:`esp_execute_shared_stack_function`, passing that function as a parameter.


Usage
-----

:cpp:func:`esp_execute_shared_stack_function` takes four arguments:

- a mutex object allocated by the caller, which is used to protect if the same function shares its allocated stack
- a pointer to the top of stack used for that function
- the size of stack in bytes
- a pointer to the shared stack function

The user-defined function is deferred as a callback and can be called using the user-allocated space without taking space from current task stack.

The usage may look like the code below:

.. code-block:: c

    void external_stack_function(void)
    {
        printf("Executing this printf from external stack! \n");
    }

    //Let us suppose we want to call printf using a separated stack space
    //allowing the app to reduce its stack size.
    void app_main()
    {
        //Allocate a stack buffer, from heap or as a static form:
        StackType_t *shared_stack = malloc(8192 * sizeof(StackType_t));
        assert(shared_stack != NULL);

        //Allocate a mutex to protect its usage:
        SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
        assert(printf_lock != NULL);

        //Call the desired function using the macro helper:
        esp_execute_shared_stack_function(printf_lock,
                                        shared_stack,
                                        8192,
                                        external_stack_function);

        vSemaphoreDelete(printf_lock);
        free(shared_stack);
    }


.. _esp-call-with-stack-basic_usage:

API Reference
-------------

.. include-build-file:: inc/esp_expression_with_stack.inc
