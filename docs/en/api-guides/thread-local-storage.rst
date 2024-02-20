Thread Local Storage
====================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

Thread-local storage (TLS) is a mechanism by which variables are allocated such that there is one instance of the variable per extant thread. ESP-IDF provides three ways to make use of such variables:

- :ref:`freertos-native`: ESP-IDF FreeRTOS native APIs.
- :ref:`pthread-api`: ESP-IDF pthread APIs.
- :ref:`c11-std`: C11 standard introduces special keywords to declare variables as thread local.

.. _freertos-native:

FreeRTOS Native APIs
--------------------

The ESP-IDF FreeRTOS provides the following APIs to manage thread local variables:

- :cpp:func:`vTaskSetThreadLocalStoragePointer`
- :cpp:func:`pvTaskGetThreadLocalStoragePointer`
- :cpp:func:`vTaskSetThreadLocalStoragePointerAndDelCallback`

In this case, the maximum number of variables that can be allocated is limited by :ref:`CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS`. Variables are kept in the task control block (TCB) and accessed by their index. Note that index 0 is reserved for ESP-IDF internal uses.

Using the APIs above, you can allocate thread local variables of an arbitrary size, and assign them to any number of tasks. Different tasks can have different sets of TLS variables.

If size of the variable is more than 4 bytes, then you need to allocate/deallocate memory for it. Variable's deallocation is initiated by FreeRTOS when task is deleted, but user must provide callback function to do proper cleanup.

.. _pthread-api:

Pthread APIs
----------------

The ESP-IDF provides the following :doc:`/api-reference/system/pthread` to manage thread local variables:

- :cpp:func:`pthread_key_create`
- :cpp:func:`pthread_key_delete`
- :cpp:func:`pthread_getspecific`
- :cpp:func:`pthread_setspecific`

These APIs have all benefits of the ones above, but eliminates some their limits. The number of variables is limited only by size of available memory on the heap. Due to the dynamic nature, this API introduces additional performance overhead compared to the native one.

.. _c11-std:

C11 Standard
------------

The ESP-IDF FreeRTOS supports thread local variables according to C11 standard, ones specified with ``__thread`` keyword. For details on this feature, please refer to the `GCC documentation <https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gcc/Thread-Local.html#Thread-Local>`_.

Storage for that kind of variables is allocated on the task stack. Note that area for all such variables in the program is allocated on the stack of every task in the system even if that task does not use such variables at all. For example, ESP-IDF system tasks (e.g., ``ipc``, ``timer`` tasks etc.) will also have that extra stack space allocated. Thus feature should be used with care.

Using C11 thread local variables comes at a trade-off. On one hand, they are quite handy to use in programming and can be accessed using minimal CPU instructions. However, this benefit comes at the cost of additional stack usage for all tasks in the system. Due to static nature of variables allocation, all tasks in the system have the same sets of C11 thread local variables.
