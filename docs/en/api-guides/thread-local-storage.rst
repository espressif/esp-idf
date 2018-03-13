Thread Local Storage
====================

Overview
--------

Thread-local storage (TLS) is a mechanism by which variables are allocated such that there 
is one instance of the variable per extant thread. ESP-IDF provides three ways to make use 
of such variables:

 - :ref:`freertos-native`: ESP-IDF FreeRTOS native API.
 - :ref:`pthread-api`: ESP-IDF's pthread API.
 - :ref:`c11-std`: C11 standard introduces special keyword to declare variables as thread local.

.. _freertos-native:

FreeRTOS Native API
--------------------

The ESP-IDF FreeRTOS provides the following API to manage thread local variables:

 - :cpp:func:`vTaskSetThreadLocalStoragePointer`
 - :cpp:func:`pvTaskGetThreadLocalStoragePointer`
 - :cpp:func:`vTaskSetThreadLocalStoragePointerAndDelCallback`

In this case maximum number of variables that can be allocated is limited by
``configNUM_THREAD_LOCAL_STORAGE_POINTERS`` macro. Variables are kept in the task control block (TCB)
and accessed by their index. Note that index 0 is reserved for ESP-IDF internal uses.
Using that API user can allocate thread local variables of an arbitrary size and assign them to any number of tasks.
Different tasks can have different sets of TLS variables.
If size of the variable is more then 4 bytes then user is responsible for allocating/deallocating memory for it. 
Variable's deallocation is initiated by FreeRTOS when task is deleted, but user must provide function (callback) 
to do proper cleanup.

.. _pthread-api:

Pthread API
----------------

The ESP-IDF provides the following pthread API to manage thtread local variables:

 - :cpp:func:`pthread_key_create`
 - :cpp:func:`pthread_key_delete`
 - :cpp:func:`pthread_getspecific`
 - :cpp:func:`pthread_setspecific`

This API has all benefits of the one above, but eliminates some its limits. The number of variables is
limited only by size of available memory on the heap.
Due to the dynamic nature this API introduces additional performance overhead compared to the native one.

.. _c11-std:

C11 Standard
------------

The ESP-IDF FreeRTOS supports thread local variables according to C11 standard (ones specified with ``__thread`` keyword).
For details on this GCC feature please see https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gcc/Thread-Local.html#Thread-Local.
Storage for that kind of variables is allocated on the task's stack.
Note that area for all such variables in the program will be allocated on the stack of
every task in the system even if that task does not use such variables at all. For example
ESP-IDF system tasks (like ``ipc``, ``timer`` tasks etc.) will also have that extra stack space allocated.
So this feature should be used with care. There is a tradeoff: C11 thread local variables are quite handy
to use in programming and can be accessed using just a few Xtensa instructions, but this benefit goes
with the cost of additional stack usage for all tasks in the system.
Due to static nature of variables allocation all tasks in the system have the same sets of C11 thread local variables.
