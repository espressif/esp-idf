POSIX Threads Support
=====================

Overview
--------

ESP-IDF is based on FreeRTOS but offers a range of POSIX-compatible APIs that allow easy porting of third party code. This includes support for common parts of the POSIX Threads "pthreads" API.

POSIX Threads are implemented in ESP-IDF as wrappers around equivalent FreeRTOS features. The runtime memory or performance overhead of using the pthreads API is quite low, but not every feature available in either pthreads or FreeRTOS is available via the ESP-IDF pthreads support.

Pthreads can be used in ESP-IDF by including standard ``pthread.h`` header, which is included in the toolchain libc. An additional ESP-IDF specific header, ``esp_pthread.h``, provides additional non-POSIX APIs for using some ESP-IDF features with pthreads.

C++ Standard Library implementations for ``std::thread``, ``std::mutex``, ``std::condition_variable``, etc. are implemented using pthreads (via GCC libstdc++). Therefore, restrictions mentioned here also apply to the equivalent C++ standard library functionality.

RTOS Integration
----------------

Unlike many operating systems using POSIX Threads, ESP-IDF is a real-time operating system with a real-time scheduler. This means that a thread will only stop running if a higher priority task is ready to run, the thread blocks on an OS synchronization structure like a mutex, or the thread calls any of the functions ``sleep``, :cpp:func:`vTaskDelay`, or ``usleep``.

.. note::

   If calling a standard libc or C++ sleep function, such as ``usleep`` defined in ``unistd.h``, then the task will only block and yield the CPU if the sleep time is longer than :ref:`one FreeRTOS tick period <CONFIG_FREERTOS_HZ>`. If the time is shorter, the thread will busy-wait instead of yielding to another RTOS task.

By default, all POSIX Threads have the same RTOS priority, but it is possible to change this by calling a :ref:`custom API <esp-pthread>`.

Standard features
-----------------

The following standard APIs are implemented in ESP-IDF.

Refer to standard POSIX Threads documentation, or pthread.h, for details about the standard arguments and behaviour of each function. Differences or limitations compared to the standard APIs are noted below.

Thread APIs
^^^^^^^^^^^

* ``pthread_create()``
  - The ``attr`` argument is supported for setting stack size and detach state only. Other attribute fields are ignored.
  - Unlike FreeRTOS task functions, the ``start_routine`` function is allowed to return. A "detached" type thread is automatically deleted if the function returns. The default "joinable" type thread will be suspended until pthread_join() is called on it.
* ``pthread_join()``
* ``pthread_detach()``
* ``pthread_exit()``
* ``sched_yield()``
* ``pthread_self()``
  - An assert will fail if this function is called from a FreeRTOS task which is not a pthread.
* ``pthread_equal()``

Thread Attributes
^^^^^^^^^^^^^^^^^

* ``pthread_attr_init()``
* ``pthread_attr_destroy()``
  - This function doesn't need to free any resources and instead resets the ``attr`` structure to defaults (implementation is same as ``pthread_attr_init()``).
* ``pthread_attr_getstacksize()`` / ``pthread_attr_setstacksize()``
* ``pthread_attr_getdetachstate()`` / ``pthread_attr_setdetachstate()``

Once
^^^^

* ``pthread_once()``

Static initializer constant ``PTHREAD_ONCE_INIT`` is supported.

.. note:: This function can be called from tasks created using either pthread or FreeRTOS APIs

Mutexes
^^^^^^^

POSIX Mutexes are implemented as FreeRTOS Mutex Semaphores (normal type for "fast" or "error check" mutexes, and Recursive type for "recursive" mutexes). This means that they have the same priority inheritance behaviour as mutexes created with :cpp:func:`xSemaphoreCreateMutex`.

* ``pthread_mutex_init()``
* ``pthread_mutex_destroy()``
* ``pthread_mutex_lock()``
* ``pthread_mutex_timedlock()``
* ``pthread_mutex_trylock()``
* ``pthread_mutex_unlock()``
* ``pthread_mutexattr_init()``
* ``pthread_mutexattr_destroy()``
* ``pthread_mutexattr_gettype()`` / ``pthread_mutexattr_settype()``

Static initializer constant ``PTHREAD_MUTEX_INITIALIZER`` is supported, but the non-standard static initializer constants for other mutex types are not supported.

.. note:: These functions can be called from tasks created using either pthread or FreeRTOS APIs

Condition Variables
^^^^^^^^^^^^^^^^^^^

* ``pthread_cond_init()``
  - The ``attr`` argument is not implemented and is ignored.
* ``pthread_cond_destroy()``
* ``pthread_cond_signal()``
* ``pthread_cond_broadcast()``
* ``pthread_cond_wait()``
* ``pthread_cond_timedwait()``

Static initializer constant ``PTHREAD_COND_INITIALIZER`` is supported.

* The resolution of ``pthread_cond_timedwait()`` timeouts is the RTOS tick period (see :ref:`CONFIG_FREERTOS_HZ`). Timeouts may be delayed up to one tick period after the requested timeout.

.. note:: These functions can be called from tasks created using either pthread or FreeRTOS APIs

Read/Write Locks
^^^^^^^^^^^^^^^^^^^

* ``pthread_rwlock_init()``
  - The ``attr`` argument is not implemented and is ignored.
* ``pthread_rwlock_destroy()``
* ``pthread_rwlock_rdlock()``
* ``pthread_rwlock_wrlock()``
* ``pthread_rwlock_unlock()``

Static initializer constant ``PTHREAD_RWLOCK_INITIALIZER`` is supported.

.. note:: These functions can be called from tasks created using either pthread or FreeRTOS APIs

Thread-Specific Data
^^^^^^^^^^^^^^^^^^^^

* ``pthread_key_create()``
  - The ``destr_function`` argument is supported and will be called if a thread function exits normally, calls ``pthread_exit()``, or if the underlying task is deleted directly using the FreeRTOS function :cpp:func:`vTaskDelete`.
* ``pthread_key_delete()``
* ``pthread_setspecific()`` / ``pthread_getspecific()``

.. note:: These functions can be called from tasks created using either pthread or FreeRTOS APIs

.. note:: There are other options for thread local storage in ESP-IDF, including options with higher performance. See :doc:`/api-guides/thread-local-storage`.

Not Implemented
---------------

The ``pthread.h`` header is a standard header and includes additional APIs and features which are not implemented in ESP-IDF. These include:

* ``pthread_cancel()`` returns ``ENOSYS`` if called.
* ``pthread_condattr_init()`` returns ``ENOSYS`` if called.

Other POSIX Threads functions (not listed here) are not implemented and will produce either a compiler or a linker error if referenced from an ESP-IDF application. If you identify a useful API that you would like to see implemented in ESP-IDF, please open a `feature request on GitHub <https://github.com/espressif/esp-idf/issues>` with the details.

.. _esp-pthread:

ESP-IDF Extensions
------------------

The API :cpp:func:`esp_pthread_set_cfg` defined in the ``esp_pthreads.h`` header offers custom extensions to control how subsequent calls to ``pthread_create()`` will behave. Currently, the following configuration can be set:

.. list::
  - Default stack size of new threads, if not specified when calling ``pthread_create()`` (overrides :ref:`CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT`).
  - RTOS priority of new threads (overrides :ref:`CONFIG_PTHREAD_TASK_PRIO_DEFAULT`).
  :not CONFIG_FREERTOS_UNICORE: - Core affinity / core pinning of new threads (overrides :ref:`CONFIG_PTHREAD_TASK_CORE_DEFAULT`).
  - FreeRTOS task name for new threads (overrides :ref:`CONFIG_PTHREAD_TASK_NAME_DEFAULT`)

This configuration is scoped to the calling thread (or FreeRTOS task), meaning that :cpp:func:`esp_pthread_set_cfg` can be called independently in different threads or tasks. If the ``inherit_cfg`` flag is set in the current configuration then any new thread created will inherit the creator's configuration (if that thread calls ``pthread_create()`` recursively), otherwise the new thread will have the default configuration.

Examples
--------

- :example:`system/pthread` demonstrates using the pthreads API to create threads
- :example:`cxx/pthread` demonstrates using C++ Standard Library functions with threads


API Reference
-------------

.. include-build-file:: inc/esp_pthread.inc

