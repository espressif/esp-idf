Interrupt Allocation
====================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

.. only:: esp32 or esp32s3

    The {IDF_TARGET_NAME} has two cores, with 32 interrupts each. Each interrupt has a fixed priority, most (but not all) interrupts are connected to the interrupt matrix.

.. only:: esp32s2

    The {IDF_TARGET_NAME} has one core, with 32 interrupts. Each interrupt has a fixed priority, most (but not all) interrupts are connected to the interrupt matrix.

.. only:: esp32c2 or esp32c3

    The {IDF_TARGET_NAME} has one core, with 31 interrupts. Each interrupt's priority is independently programmable.

.. only:: esp32c6 or esp32h2

    The {IDF_TARGET_NAME} has one core, with 28 external asynchronous interrupts. Each interrupt's priority is independently programmable. In addition, there are also 4 core local interrupt sources (CLINT). See **{IDF_TARGET_NAME} Technical Reference Manual** [`PDF <{IDF_TARGET_TRM_EN_URL}#riscvcpu>`__] for more details.

.. only:: esp32p4

    The {IDF_TARGET_NAME} has two cores, with 32 external asynchronous interrupts each. Each interrupt's priority is independently programmable. In addition, there are also 3 core local interrupt sources (CLINT) on each core. See **{IDF_TARGET_NAME} Technical Reference Manual** [`PDF <{IDF_TARGET_TRM_EN_URL}#riscvcpu>`__] for more details.

Because there are more interrupt sources than interrupts, sometimes it makes sense to share an interrupt in multiple drivers. The :cpp:func:`esp_intr_alloc` abstraction exists to hide all these implementation details.

A driver can allocate an interrupt for a certain peripheral by calling :cpp:func:`esp_intr_alloc` (or :cpp:func:`esp_intr_alloc_intrstatus`). It can use the flags passed to this function to specify the type, priority, and trigger method of the interrupt to allocate. The interrupt allocation code will then find an applicable interrupt, use the interrupt matrix to hook it up to the peripheral, and install the given interrupt handler and ISR to it.

The interrupt allocator presents two different types of interrupts, namely shared interrupts and non-shared interrupts, both of which require different handling. Non-shared interrupts will allocate a separate interrupt for every :cpp:func:`esp_intr_alloc` call, and this interrupt is use solely for the peripheral attached to it, with only one ISR that will get called. Shared interrupts can have multiple peripherals triggering them, with multiple ISRs being called when one of the peripherals attached signals an interrupt. Thus, ISRs that are intended for shared interrupts should check the interrupt status of the peripheral they service in order to check if any action is required.

Non-shared interrupts can be either level- or edge-triggered. Shared interrupts can only be level interrupts due to the chance of missed interrupts when edge interrupts are used.

To illustrate why shard interrupts can only be level-triggered, take the scenario where peripheral A and peripheral B share the same edge-triggered interrupt. Peripheral B triggers an interrupt and sets its interrupt signal high, causing a low-to-high edge, which in turn latches the CPU's interrupt bit and triggers the ISR. The ISR executes, checks that peripheral A did not trigger an interrupt, and proceeds to handle and clear peripheral B's interrupt signal. Before the ISR returns, the CPU clears its interrupt bit latch. Thus, during the entire interrupt handling process, if peripheral A triggers an interrupt, it will be missed due the CPU clearing the interrupt bit latch.


.. only:: esp32 or esp32s3

    Multicore Issues
    ----------------

    Peripherals that can generate interrupts can be divided in two types:

      - External peripherals, within the {IDF_TARGET_NAME} but outside the Xtensa cores themselves. Most {IDF_TARGET_NAME} peripherals are of this type.
      - Internal peripherals, part of the Xtensa CPU cores themselves.

    Interrupt handling differs slightly between these two types of peripherals.

    Internal Peripheral Interrupts
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Each Xtensa CPU core has its own set of six internal peripherals:

      - Three timer comparators
      - A performance monitor
      - Two software interrupts

    Internal interrupt sources are defined in ``esp_intr_alloc.h`` as ``ETS_INTERNAL_*_INTR_SOURCE``.

    These peripherals can only be configured from the core they are associated with. When generating an interrupt, the interrupt they generate is hard-wired to their associated core; it is not possible to have, for example, an internal timer comparator of one core generate an interrupt on another core. That is why these sources can only be managed using a task running on that specific core. Internal interrupt sources are still allocatable using :cpp:func:`esp_intr_alloc` as normal, but they cannot be shared and will always have a fixed interrupt level (namely, the one associated in hardware with the peripheral).

    External Peripheral Interrupts
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The remaining interrupt sources are from external peripherals.

.. only:: esp32p4

    Multicore Considerations
    ------------------------

    Each core on {IDF_TARGET_NAME} provides internal interrupts that are triggered by the core itself and external interrupts that are triggered by peripherals. However, ESP-IDF only makes use of the external interrupts on {IDF_TARGET_NAME}. Most {IDF_TARGET_NAME} interrupt sources are external interrupts.

    Each external interrupt slot of each core is wired to the interrupt matrix. The interrupt matrix allows any interrupt slot to be connected to any external interrupt source. Mapping multiple external interrupts sources to a single slot is also supported. These external interrupt sources are defined in ``soc/interrupts.h`` as ``ETS_*_INTR_SOURCE``.

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    - Allocating an external interrupt will always allocate it on the core that does the allocation.
    - Freeing an external interrupt must always happen on the same core it was allocated on.
    - Disabling and enabling external interrupts from another core is allowed.
    - Multiple external interrupt sources can share an interrupt slot by passing ``ESP_INTR_FLAG_SHARED`` as a flag to :cpp:func:`esp_intr_alloc`.

    Care should be taken when calling :cpp:func:`esp_intr_alloc` from a task which is not pinned to a core. During task switching, these tasks can migrate between cores. Therefore it is impossible to tell which CPU the interrupt is allocated on, which makes it difficult to free the interrupt handle and may also cause debugging difficulties. It is advised to use :cpp:func:`xTaskCreatePinnedToCore` with a specific CoreID argument to create tasks that allocate interrupts. In the case of internal interrupt sources, this is required.


IRAM-Safe Interrupt Handlers
----------------------------

The ``ESP_INTR_FLAG_IRAM`` flag registers an interrupt handler that always runs from IRAM (and reads all its data from DRAM), and therefore does not need to be disabled during flash erase and write operations.

This is useful for interrupts which need a guaranteed minimum execution latency, as flash write and erase operations can be slow (erases can take tens or hundreds of milliseconds to complete).

It can also be useful to keep an interrupt handler in IRAM if it is called very frequently, to avoid flash cache misses.

Refer to the :ref:`SPI flash API documentation <iram-safe-interrupt-handlers>` for more details.

.. _intr-alloc-shared-interrupts:

Multiple Handlers Sharing A Source
----------------------------------

Several handlers can be assigned to a same source, given that all handlers are allocated using the ``ESP_INTR_FLAG_SHARED`` flag. They will all be allocated to the interrupt, which the source is attached to, and called sequentially when the source is active. The handlers can be disabled and freed individually. The source is attached to the interrupt (enabled), if one or more handlers are enabled, otherwise detached. A handler will never be called when disabled, while **its source may still be triggered** if any one of its handler enabled.

Sources attached to non-shared interrupt do not support this feature.

.. only:: not SOC_CPU_HAS_FLEXIBLE_INTC

    By default, when ``ESP_INTR_FLAG_SHARED`` flag is specified, the interrupt allocator will allocate only priority level 1 interrupts. Use ``ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED`` to also allow allocating shared interrupts at priority levels 2 and 3.

Though the framework supports this feature, you have to use it **very carefully**. There usually exist two ways to stop an interrupt from being triggered: **disable the source** or **mask peripheral interrupt status**. ESP-IDF only handles enabling and disabling of the source itself, leaving status and mask bits to be handled by users.

**Status bits shall either be masked before the handler responsible for it is disabled, or be masked and then properly handled in another enabled interrupt**.

.. note::

    Leaving some status bits unhandled without masking them, while disabling the handlers for them, will cause the interrupt(s) to be triggered indefinitely, resulting therefore in a system crash.


Troubleshooting Interrupt Allocation
------------------------------------

On most Espressif SoCs, CPU interrupts are a limited resource. Therefore it is possible for a program to run out of CPU interrupts, for example by initializing several peripheral drivers. Typically, this will result in the driver initialization function returning ``ESP_ERR_NOT_FOUND`` error code.

If this happens, you can use :cpp:func:`esp_intr_dump` function to print the list of interrupts along with their status. The output of this function typically looks like this:

.. code-block::

    CPU 0 interrupt status:
    Int  Level  Type   Status
    0     1    Level  Reserved
    1     1    Level  Reserved
    2     1    Level  Used: RTC_CORE
    3     1    Level  Used: TG0_LACT_LEVEL
    ...

The columns of the output have the following meaning:

.. list::

    - ``Int``: CPU interrupt input number. This is typically not used in software directly, and is provided for reference only.
    :not SOC_CPU_HAS_FLEXIBLE_INTC: - ``Level``: Interrupt priority (1-7) of the CPU interrupt. This priority is fixed in hardware, and cannot be changed.
    :SOC_CPU_HAS_FLEXIBLE_INTC: - ``Level``: For interrupts which have been allocated, the priority of the interrupt. For free interrupts ``*`` is printed.
    :not SOC_CPU_HAS_FLEXIBLE_INTC: - ``Type``: Interrupt type (Level or Edge) of the CPU interrupt. This type is fixed in hardware, and cannot be changed.
    :SOC_CPU_HAS_FLEXIBLE_INTC: - ``Type``: For interrupts which have been allocated, the type (Level or Edge) of the interrupt. For free interrupts ``*`` is printed.
    - ``Status``: One of the possible statuses of the interrupt:
        - ``Reserved``: The interrupt is reserved either at hardware level, or by one of the parts of ESP-IDF. It can not be allocated using :cpp:func:`esp_intr_alloc`.
        - ``Used: <source>``: The interrupt is allocated and connected to a single peripheral.
        - ``Shared: <source1> <source2> ...``: The interrupt is allocated and connected to multiple peripherals. See :ref:`intr-alloc-shared-interrupts` above.
        - ``Free``: The interrupt is not allocated and can be used by :cpp:func:`esp_intr_alloc`.
        :not SOC_CPU_HAS_FLEXIBLE_INTC: - ``Free (not general-use)``: The interrupt is not allocated, but is either a high-priority interrupt (priority 4-7) or an edge-triggered interrupt. High-priority interrupts can be allocated using :cpp:func:`esp_intr_alloc` but requires the handlers to be written in Assembly, see :doc:`../../api-guides/hlinterrupts`. Edge-triggered low- and medium-priority interrupts can also be allocated using :cpp:func:`esp_intr_alloc`, but are not used often since most peripheral interrupts are level-triggered.

If you have confirmed that the application is indeed running out of interrupts, a combination of the following suggestions can help resolve the issue:

.. list::

    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - On multi-core targets, try initializing some of the peripheral drivers from a task pinned to the second core. Interrupts are typically allocated on the same core where the peripheral driver initialization function runs. Therefore by running the initialization function on the second core, more interrupt inputs can be used.
    - Determine the interrupts which can tolerate higher latency, and allocate them using ``ESP_INTR_FLAG_SHARED`` flag (optionally ORed with ``ESP_INTR_FLAG_LOWMED``). Using this flag for two or more peripherals will let them use a single interrupt input, and therefore save interrupt inputs for other peripherals. See :ref:`intr-alloc-shared-interrupts` above.
    :not SOC_CPU_HAS_FLEXIBLE_INTC: - Some peripheral driver may default to allocating interrupts with ``ESP_INTR_FLAG_LEVEL1`` flag, so priority 2 and 3 interrupts do not get used by default. If :cpp:func:`esp_intr_dump` shows that some priority 2 or 3 interrupts are available, try changing the interrupt allocation flags when initializing the driver to ``ESP_INTR_FLAG_LEVEL2`` or ``ESP_INTR_FLAG_LEVEL3``.
    - Check if some of the peripheral drivers do not need to be used all the time, and initialize or deinitialize them on demand. This can reduce the number of simultaneously allocated interrupts.


API Reference
-------------

.. include-build-file:: inc/esp_intr_types.inc
.. include-build-file:: inc/esp_intr_alloc.inc
