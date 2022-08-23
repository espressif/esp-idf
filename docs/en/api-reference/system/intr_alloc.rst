Interrupt allocation
====================

Overview
--------

.. only:: esp32

  The {IDF_TARGET_NAME} has two cores, with 32 interrupts each. Each interrupt has a certain priority level, most (but not all) interrupts are connected to the interrupt mux.

.. only:: esp32s2

  The {IDF_TARGET_NAME} has one core, with 32 interrupts. Each interrupt has a certain priority level, most (but not all) interrupts are connected to the interrupt mux.

.. only:: esp32s3

  The {IDF_TARGET_NAME} has two cores, with 32 interrupts. Each interrupt has a certain priority level, most (but not all) interrupts are connected to the interrupt mux.

.. only:: esp32c3 or esp32c2

  The {IDF_TARGET_NAME} has one core, with 31 interrupts. Each interrupt has a programmable priority level.

Because there are more interrupt sources than interrupts, sometimes it makes sense to share an interrupt in multiple drivers. The :cpp:func:`esp_intr_alloc` abstraction exists to hide all these implementation details.

A driver can allocate an interrupt for a certain peripheral by calling :cpp:func:`esp_intr_alloc` (or :cpp:func:`esp_intr_alloc_intrstatus`). It can use the flags passed to this function to set the type of interrupt allocated, specifying a particular level or trigger method. The interrupt allocation code will then find an applicable interrupt, use the interrupt mux to hook it up to the peripheral, and install the given interrupt handler and ISR to it.

This code presents two different types of interrupts, handled differently: shared interrupts and non-shared interrupts. The simplest ones are non-shared interrupts: a separate interrupt is allocated per :cpp:func:`esp_intr_alloc` call and this interrupt is solely used for the peripheral attached to it, with only one ISR that will get called. On the other hand, shared interrupts can have multiple peripherals triggering them, with multiple ISRs being called when one of the peripherals attached signals an interrupt. Thus, ISRs that are intended for shared interrupts should check the interrupt status of the peripheral they service in order to check if any action is required.

Non-shared interrupts can be either level- or edge-triggered. Shared interrupts can only be level interrupts due to the chance of missed interrupts when edge interrupts are used.

For example, let's say DevA and DevB share an interrupt. DevB signals an interrupt, so INT line goes high. The ISR handler calls code for DevA but does nothing. Then, ISR handler calls code for DevB, but while doing that, DevA signals an interrupt. DevB's ISR is done, it clears interrupt status for DevB and exits interrupt code. Now, an interrupt for DevA is still pending, but because the INT line never went low, as DevA kept it high even when the interrupt for DevB was cleared, the interrupt is never serviced.


.. only:: esp32 or esp32s3

  Multicore issues
  ----------------

  Peripherals that can generate interrupts can be divided in two types:

    - External peripherals, within the {IDF_TARGET_NAME} but outside the Xtensa cores themselves. Most {IDF_TARGET_NAME} peripherals are of this type.
    - Internal peripherals, part of the Xtensa CPU cores themselves.

  Interrupt handling differs slightly between these two types of peripherals.

  Internal peripheral interrupts
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  Each Xtensa CPU core has its own set of six internal peripherals:

    - Three timer comparators
    - A performance monitor
    - Two software interrupts.

  Internal interrupt sources are defined in esp_intr_alloc.h as ``ETS_INTERNAL_*_INTR_SOURCE``.

  These peripherals can only be configured from the core they are associated with. When generating an interrupt, the interrupt they generate is hard-wired to their associated core; it's not possible to have, for example, an internal timer comparator of one core generate an interrupt on another core. That is why these sources can only be managed using a task running on that specific core. Internal interrupt sources are still allocatable using :cpp:func:`esp_intr_alloc` as normal, but they cannot be shared and will always have a fixed interrupt level (namely, the one associated in hardware with the peripheral).

  External Peripheral Interrupts
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  The remaining interrupt sources are from external peripherals. These are defined in soc/soc.h as ``ETS_*_INTR_SOURCE``.

  Non-internal interrupt slots in both CPU cores are wired to an interrupt multiplexer, which can be used to route any external interrupt source to any of these interrupt slots.

  - Allocating an external interrupt will always allocate it on the core that does the allocation.
  - Freeing an external interrupt must always happen on the same core it was allocated on.
  - Disabling and enabling external interrupts from another core is allowed.
  - Multiple external interrupt sources can share an interrupt slot by passing ``ESP_INTR_FLAG_SHARED`` as a flag to :cpp:func:`esp_intr_alloc`.

  Care should be taken when calling :cpp:func:`esp_intr_alloc` from a task which is not pinned to a core. During task switching, these tasks can migrate between cores. Therefore it is impossible to tell which CPU the interrupt is allocated on, which makes it difficult to free the interrupt handle and may also cause debugging difficulties. It is advised to use :cpp:func:`xTaskCreatePinnedToCore` with a specific CoreID argument to create tasks that will allocate interrupts. In the case of internal interrupt sources, this is required.

IRAM-Safe Interrupt Handlers
----------------------------

The ``ESP_INTR_FLAG_IRAM`` flag registers an interrupt handler that always runs from IRAM (and reads all its data from DRAM), and therefore does not need to be disabled during flash erase and write operations.

This is useful for interrupts which need a guaranteed minimum execution latency, as flash write and erase operations can be slow (erases can take tens or hundreds of milliseconds to complete).

It can also be useful to keep an interrupt handler in IRAM if it is called very frequently, to avoid flash cache misses.

Refer to the :ref:`SPI flash API documentation <iram-safe-interrupt-handlers>` for more details.

Multiple Handlers Sharing A Source
----------------------------------

Several handlers can be assigned to a same source, given that all handlers are allocated using the ``ESP_INTR_FLAG_SHARED`` flag. They will all be allocated to the interrupt, which the source is attached to, and called sequentially when the source is active. The handlers can be disabled and freed individually. The source is attached to the interrupt (enabled), if one or more handlers are enabled, otherwise detached. A handler will never be called when disabled, while **its source may still be triggered** if any one of its handler enabled.

Sources attached to non-shared interrupt do not support this feature.

Though the framework support this feature, you have to use it *very carefully*. There usually exist two ways to stop an interrupt from being triggered: *disable the source* or *mask peripheral interrupt status*. IDF only handles enabling and disabling of the source itself, leaving status and mask bits to be handled by users.
**Status bits shall either be masked before the handler responsible for it is disabled, either be masked and then properly handled in another enabled interrupt**.
Please note that leaving some status bits unhandled without masking them, while disabling the handlers for them, will cause the interrupt(s) to be triggered indefinitely, resulting therefore in a system crash.

API Reference
-------------

.. include-build-file:: inc/esp_intr_alloc.inc


