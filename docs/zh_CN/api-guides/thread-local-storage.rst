线程局部存储
====================

:link_to_translation:`en:[English]`

概述
--------

线程局部存储 (TLS) 机制可以分配变量，使每个现有的线程都有相应变量实例。ESP-IDF 提供了以下三种方法，支持使用此类变量：

- :ref:`freertos-native`：ESP-IDF FreeRTOS 原生 API。
- :ref:`pthread-api`：ESP-IDF pthread API。
- :ref:`c11-std`：C11 标准引入了特殊关键字，将变量声明为线程局部变量。

.. _freertos-native:

FreeRTOS 原生 API
--------------------

ESP-IDF FreeRTOS 提供以下 API，用于管理线程局部变量：

- :cpp:func:`vTaskSetThreadLocalStoragePointer`
- :cpp:func:`pvTaskGetThreadLocalStoragePointer`
- :cpp:func:`vTaskSetThreadLocalStoragePointerAndDelCallback`

此时，可以分配的最大变量数量受 :ref:`CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS` 限制。变量保存在任务控制块 (TCB) 中，并由其索引访问。注意，索引 0 保留，供 ESP-IDF 内部使用。

通过使用上述 API，你可以分配任意大小的线程局部变量，并将其分配给任意数量的任务。不同任务可以拥有不同的 TLS 变量集。

如果变量大小超过 4 个字节，你需要负责为其分配/释放内存。在任务删除时，FreeRTOS 会释放变量，但必须提供回调函数来适当清理。

.. _pthread-api:

Pthread API
----------------

ESP-IDF 提供以下 :doc:`/api-reference/system/pthread`，用于管理线程局部变量：

- :cpp:func:`pthread_key_create`
- :cpp:func:`pthread_key_delete`
- :cpp:func:`pthread_getspecific`
- :cpp:func:`pthread_setspecific`

Pthread API 具备 FreeRTOS 原生 API 的所有优点，并突破了 FreeRTOS 原生 API 的部分限制，如变量数量仅受堆上可用内存大小的限制。然而由于 Pthread API 具备动态性质，与原生 API 相比，这个 API 引入了额外的性能开销。

.. _c11-std:

C11 标准
------------

ESP-IDF FreeRTOS 支持基于 C11 标准的线程局部变量，使用 ``__thread`` 关键字指定。要了解 GCC 功能详情，请参阅 `GCC 文档 <https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gcc/Thread-Local.html#Thread-Local>`_。

这类变量的存储空间分配在任务栈上。请注意，即使任务根本不使用这些变量，程序中所有这类变量的存储区域都会在系统中每个任务的栈上分配。例如，ESP-IDF 系统任务（如 ``ipc``、``timer`` 任务等）也有额外的栈空间分配。因此，使用这个特性时，需要谨慎考虑。

在使用 C11 线程局部变量时，可以针对以下特点进行权衡：在编程中使用 C11 线程局部变量非常方便，使用最少的 CPU 指令即可访问，但这一优势的代价是系统中所有任务都会额外使用栈。由于变量分配的静态性质，系统中的所有任务具有一组相同的 C11 线程局部变量。
