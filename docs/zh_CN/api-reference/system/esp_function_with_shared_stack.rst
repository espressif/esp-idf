使用外部堆栈调用函数
=================================

:link_to_translation:`en:[English]`

概述
--------

执行某个给定函数时，可以使用用户分配的堆栈空间，且该堆栈空间独立于当前任务的堆栈。这一机制能够节省在调用常用函数时浪费大量堆栈空间，例如 ``printf`` 函数。具体而言，将给定函数作为参数传入 :cpp:func:`esp_execute_shared_stack_function` 中，给定函数便会在共享堆栈空间内作为回调函数延迟执行。


使用方法
--------

:cpp:func:`esp_execute_shared_stack_function` 需要四个参数：

- 由调用者分配的互斥锁，防止同一函数共享分配的堆栈
- 指向分配的堆栈顶部的指针
- 以字节为单位的堆栈的大小
- 指向需要共享堆栈的函数的指针

通过这一功能，用户指定的函数被作为回调函数延迟执行，并在用户分配的空间中调用，无需从当前任务堆栈中获取空间。

该函数的使用方式如下所示：

.. code-block:: c

    void external_stack_function(void)
    {
        printf("Executing this printf from external stack! \n");
    }

    //假设要使用一个单独的堆栈空间来调用 printf 函数
    //允许应用程序减小其堆栈大小
    void app_main()
    {
        //从堆中或以静态方式分配一个堆栈 buffer：
        StackType_t *shared_stack = malloc(8192 * sizeof(StackType_t));
        assert(shared_stack != NULL);

        //分配一个互斥锁：
        SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
        assert(printf_lock != NULL);

        //使用宏助手调用所需函数：
        esp_execute_shared_stack_function(printf_lock,
                                        shared_stack,
                                        8192,
                                        external_stack_function);

        vSemaphoreDelete(printf_lock);
        free(shared_stack);
    }


.. _esp-call-with-stack-basic_usage:

API 参考
-------------

.. include-build-file:: inc/esp_expression_with_stack.inc
