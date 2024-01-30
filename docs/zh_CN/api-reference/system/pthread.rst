POSIX Thread
============

:link_to_translation:`en:[English]`

概述
--------

ESP-IDF 基于 FreeRTOS，但提供了一系列与 POSIX 兼容的 API，以便轻松移植第三方代码，例如支持 pthread 常用的 ``pthread`` API。

在 ESP-IDF 中，pthread 对 FreeRTOS 中的等效功能进行了包装。pthread API 所需的运行内存或性能开销很低，但 pthread 或 FreeRTOS 的可用功能并非都可以通过 ESP-IDF 的 pthread 支持来实现。

添加标准 ``pthread.h`` 头文件后可以在 ESP-IDF 中使用 pthread，该头文件已包含在工具链 libc 中。还有另一个专用于 ESP-IDF 的头文件 ``esp_pthread.h``，其中提供了一些额外的非 POSIX API，以便通过 pthread 使用一些 ESP-IDF 功能。

C++ 标准库中的 ``std::thread``、``std::mutex``、``std::condition_variable`` 等功能也是通过 pthread（利用 GCC libstdc++）实现的。因此，本文档提到的限制条件也同样适用于 C++ 标准库中等效功能。

RTOS 集成
----------------

与许多使用 pthread 的操作系统不同，ESP-IDF 是一个实时操作系统，具有实时调度程序。这意味着只有当一个更高优先级的任务准备就绪、线程在 OS 同步结构（如 mutex）上发生阻塞、或者线程调用 ``sleep``、:cpp:func:`vTaskDelay`、``usleep`` 等函数时，线程才会停止运行。

.. note::

    如果调用 C 标准库或 C++ sleep 函数，例如在 ``unistd.h`` 中定义的 ``usleep``，那么只有当睡眠时间超过 :ref:`一个 FreeRTOS 滴答周期 <CONFIG_FREERTOS_HZ>` 时，任务才会阻塞并让出内核。如果时间较短，线程将处于忙等待状态，不会让步给另一个 RTOS 任务。

默认情况下，所有 pthread 具有相同的 RTOS 优先级，但可以通过调用 :ref:`ESP-IDF 提供的扩展 API <esp-pthread>` 对此优先级进行更改。

标准功能
-----------------

ESP-IDF 中实现了以下标准 API。

请参考 `标准 pthread 文档 <https://man7.org/linux/man-pages/man7/pthreads.7.html>`__ 或 ``pthread.h``，了解每个函数标准参数和行为的详细信息。下文还列出了 pthread API 与标准 API 相比的差异或局限性。

.. _posix_thread_api:

线程 API
^^^^^^^^^^^

* ``pthread_create()``
    - ``attr`` 参数仅支持设置堆栈大小和分离状态，其他属性字段将被忽略。
    - 与 FreeRTOS 任务函数不同，``start_routine`` 函数允许返回。如果函数返回，分离类型的线程会被自动删除。而默认的可连接类型线程将被挂起，直到调用 ``pthread_join()``。
* ``pthread_join()``
* ``pthread_detach()``
* ``pthread_exit()``
* ``sched_yield()``
* ``pthread_self()``
    - 如果从不是 pthread 的 FreeRTOS 任务中调用此函数，断言会失败。
* ``pthread_equal()``

线程属性
^^^^^^^^^^^^^^^^^

* ``pthread_attr_init()``
* ``pthread_attr_destroy()``
    - 此函数不需要释放任何资源，而是将 ``attr`` 结构体重置为默认值。其实现与 ``pthread_attr_init()`` 相同。
* ``pthread_attr_getstacksize()`` / ``pthread_attr_setstacksize()``
* ``pthread_attr_getdetachstate()`` / ``pthread_attr_setdetachstate()``

Once
^^^^^^^^

* ``pthread_once()``

支持静态初始化常量 ``PTHREAD_ONCE_INIT``。

.. note::

    在使用 pthread 或 FreeRTOS API 创建的任务中都可以调用此函数。

互斥锁
^^^^^^^

POSIX 互斥锁被实现为 FreeRTOS 互斥信号量（普通类型用于“快速”或“错误检查”互斥锁，递归类型用于“递归”互斥锁），因此与使用 :cpp:func:`xSemaphoreCreateMutex` 创建的互斥锁具有相同的优先级继承行为。

* ``pthread_mutex_init()``
* ``pthread_mutex_destroy()``
* ``pthread_mutex_lock()``
* ``pthread_mutex_timedlock()``
* ``pthread_mutex_trylock()``
* ``pthread_mutex_unlock()``
* ``pthread_mutexattr_init()``
* ``pthread_mutexattr_destroy()``
* ``pthread_mutexattr_gettype()`` / ``pthread_mutexattr_settype()``

支持静态初始化常量 ``PTHREAD_MUTEX_INITIALIZER``，但不支持其他互斥锁类型的非标准静态初始化常量。

.. note::

    在使用 pthread 或 FreeRTOS API 创建的任务中都可以调用这些函数。

条件变量
^^^^^^^^^^^^^^^^^^^

* ``pthread_cond_init()``
    - ``attr`` 参数未实现，将被忽略。
* ``pthread_cond_destroy()``
* ``pthread_cond_signal()``
* ``pthread_cond_broadcast()``
* ``pthread_cond_wait()``
* ``pthread_cond_timedwait()``

支持静态初始化常量 ``PTHREAD_COND_INITIALIZER``。

* ``pthread_cond_timedwait()`` 超时的分辨率为 RTOS 滴答周期（参见 :ref:`CONFIG_FREERTOS_HZ`）。在请求超时后，超时最多会延迟一个滴答周期。

.. note::

    在使用 pthread 或 FreeRTOS API 创建的任务中都可以调用这些函数。

信号量
^^^^^^^^^^

ESP-IDF 中实现了 POSIX **未命名信号量**，下文介绍了可访问的 API。除非另有说明，否则 ESP-IDF 中未命名信号量的实现遵循 `POSIX 标准规定的信号量 <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/semaphore.h.html>`_。

* `sem_init() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_init.html>`_
* `sem_destroy() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_destroy.html>`_

    - ``pshared`` 被忽略。信号量始终可以在 FreeRTOS 任务之间共享。

* `sem_post() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_post.html>`_

    - 如果信号量的值已经是 ``SEM_VALUE_MAX``，则返回 ``-1``，并将 ``errno`` 设置为 ``EAGAIN``。

* `sem_wait() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_wait.html>`_
* `sem_trywait() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_trywait.html>`_
* `sem_timedwait() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_timedwait.html>`_

    - 通过 abstime 传递的时间值将被向上舍入到下一个 FreeRTOS 时钟滴答。
    - 超时实际发生在被舍入到的滴答之后，下一个滴答之前。
    - 在计算超时后，任务有可能被立即抢占（可能性较小），从而延迟下一个阻塞操作系统调用的超时，延迟的时间等于抢占的持续时间。

* `sem_getvalue() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_getvalue.html>`_

读/写锁
^^^^^^^^^^^^^^^^
ESP-IDF 中实现了 POSIX 读写锁规范的以下 API 函数：

* `pthread_rwlock_init() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_init.html>`_

    - ``attr`` 参数未实现，将被忽略。

* `pthread_rwlock_destroy() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_destroy.html>`_
* `pthread_rwlock_rdlock() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_rdlock.html>`_
* `pthread_rwlock_tryrdlock() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_tryrdlock.html>`_
* `pthread_rwlock_wrlock() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_wrlock.html>`_
* `pthread_rwlock_trywrlock() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_trywrlock.html>`_
* `pthread_rwlock_unlock() <https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_unlock.html>`_

支持静态初始化器常量 ``PTHREAD_RWLOCK_INITIALIZER``。

.. note::

    在 pthread 或 FreeRTOS API 创建的任务中都可以调用此函数。

线程特定数据
^^^^^^^^^^^^^^^^^^^^

* ``pthread_key_create()``
    - 支持 ``destr_function`` 参数。如果线程函数正常退出并调用 ``pthread_exit()``，此参数就会被调用，或者在使用 FreeRTOS 函数 :cpp:func:`vTaskDelete` 直接删除了底层任务时被调用。
* ``pthread_key_delete()``
* ``pthread_setspecific()`` / ``pthread_getspecific()``

.. note::

    在 pthread 或 FreeRTOS API 创建的任务中都可以调用此函数。当从 FreeRTOS API 创建的任务中调用这些函数时，必须先启用 :ref:`CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS` 配置选项，以确保在删除任务之前清理线程数据。

.. note::

    ESP-IDF 中还有其他的线程本地存储选项，包括性能更高的选项。参见 :doc:`/api-guides/thread-local-storage`。

未实现 API
---------------

``pthread.h`` 头文件是一个标准头文件，包含了在 ESP-IDF 中未实现的额外 API 和功能，包括：

* 如果调用 ``pthread_cancel()``，返回 ``ENOSYS``。
* ``pthread_condattr_init()`` 如果被调用，返回 ``ENOSYS``。

其他未列出的 pthread 函数未在 ESP-IDF 中实现，如果从 ESP-IDF 应用程序中直接引用，将产生编译器错误或链接器错误。如果希望 ESP-IDF 支持某个尚未实现的 API，请 `在 GitHub 上发起功能请求 <https://github.com/espressif/esp-idf/issues>`_ 并提供详细信息。

.. _esp-pthread:

ESP-IDF 扩展
------------------

在 ``esp_pthreads.h`` 头文件中定义的 API :cpp:func:`esp_pthread_set_cfg` 提供了自定义扩展，能够对后续 ``pthread_create()`` 的调用行为进行控制。目前提供以下配置：

.. list::
    - 如果调用 ``pthread_create()`` 时未指定默认堆栈大小，可设置新线程的默认堆栈大小（覆盖 :ref:`CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT`）。
    - 堆栈内存属性决定用于分配 pthread 堆栈的内存类型。该字段使用 ESP-IDF 堆属性标志，这一标志在 :component_file:`heap/include/esp_heap_caps.h` 文件中定义。为了确保分配的内存能够通过 8 位地址访问 (MALLOC_CAP_8BIT)，用户必须设置相应的标志，此外也可添加其他自定义标志。用户应当确保选择了正确的堆栈内存属性。了解内存位置的更多信息，请参考 :ref:`memory_capabilities` 文档。
    - 新线程的 RTOS 优先级（覆盖 :ref:`CONFIG_PTHREAD_TASK_PRIO_DEFAULT`）。
    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - 新线程的内核亲和性/内核固定（覆盖 :ref:`CONFIG_PTHREAD_TASK_CORE_DEFAULT`）。
    - 新线程的 FreeRTOS 任务名称（覆盖 :ref:`CONFIG_PTHREAD_TASK_NAME_DEFAULT`）

此配置的作用范围是调用线程或 FreeRTOS 任务，这意味着 :cpp:func:`esp_pthread_set_cfg` 可以在不同的线程或任务中独立调用。如果在当前配置中设置了 ``inherit_cfg`` 标志，那么当一个线程递归调用 ``pthread_create()`` 时，任何新创建的线程都会继承该线程的配置，否则新线程将采用默认配置。

示例
--------

- :example:`system/pthread` 演示了如何使用 pthread API 创建线程。
- :example:`cxx/pthread` 演示了如何通过线程使用 C++ 标准库函数。


API 参考
-------------

.. include-build-file:: inc/esp_pthread.inc
