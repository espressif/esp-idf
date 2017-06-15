Interrupt allocation
====================

Overview
--------

The ESP32 has two cores, with 32 interrupts each. Each interrupt has a certain priority level, most (but not all) interrupts are connected
to the interrupt mux. Because there are more interrupt sources than interrupts, sometimes it makes sense to share an interrupt in
multiple drivers. The esp_intr_alloc abstraction exists to hide all these implementation details.

A driver can allocate an interrupt for a certain peripheral by calling esp_intr_alloc (or esp_intr_alloc_sintrstatus). It can use
the flags passed to this function to set the type of interrupt allocated, specifying a specific level or trigger method. The
interrupt allocation code will then find an applicable interrupt, use the interrupt mux to hook it up to the peripheral, and
install the given interrupt handler and ISR to it.

This code has two different types of interrupts it handles differently: Shared interrupts and non-shared interrupts. The simplest
of the two are non-shared interrupts: a separate interrupt is allocated per esp_intr_alloc call and this interrupt is solely used for
the peripheral attached to it, with only one ISR that will get called. Shared interrupts can have multiple peripherals triggering 
it, with multiple ISRs being called when one of the peripherals attached signals an interrupt. Thus, ISRs that are intended for shared
interrupts should check the interrupt status of the peripheral they service in order to see if any action is required.

Non-shared interrupts can be either level- or edge-triggered. Shared interrupts can
only be level interrupts (because of the chance of missed interrupts when edge interrupts are
used.)
(The logic behind this: DevA and DevB share an int. DevB signals an int. Int line goes high. ISR handler
calls code for DevA -> does nothing. ISR handler calls code for DevB, but while doing that,
DevA signals an int. ISR DevB is done, clears int for DevB, exits interrupt code. Now an 
interrupt for DevA is still pending, but because the int line never went low (DevA kept it high
even when the int for DevB was cleared) the interrupt is never serviced.)


Multicore issues
----------------

Peripherals that can generate interrupts can be divided in two types:

  - External peripherals, within the ESP32 but outside the Xtensa cores themselves. Most ESP32 peripherals are of this type.
  - Internal peripherals, part of the Xtensa CPU cores themselves.

Interrupt handling differs slightly between these two types of peripherals.

Internal peripheral interrupts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each Xtensa CPU core has its own set of six internal peripherals:

  - Three timer comparators
  - A performance monitor
  - Two software interrupts.

Internal interrupt sources are defined in esp_intr_alloc.h as ``ETS_INTERNAL_*_INTR_SOURCE``.

These peripherals can only be configured from the core they are associated with. When generating an interrupt,
the interrupt they generate is hard-wired to their associated core; it's not possible to have e.g. an internal
timer comparator of one core generate an interrupt on another core. That is why these sources can only be managed
using a task running on that specific core. Internal interrupt sources are still allocatable using esp_intr_alloc
as normal, but they cannot be shared and will always have a fixed interrupt level (namely, the one associated in
hardware with the peripheral).

External Peripheral Interrupts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The remaining interrupt sources are from external peripherals. These are defined in soc/soc.h as ``ETS_*_INTR_SOURCE``.

Non-internal interrupt slots in both CPU cores are wired to an interrupt multiplexer, which can be used to
route any external interrupt source to any of these interrupt slots.

- Allocating an external interrupt will always allocate it on the core that does the allocation.
- Freeing an external interrupt must always happen on the same core it was allocated on.
- Disabling and enabling external interrupts from another core is allowed.
- Multiple external interrupt sources can share an interrupt slot by passing ``ESP_INTR_FLAG_SHARED`` as a flag to esp_intr_alloc().

Care should be taken when calling esp_intr_alloc() from a task which is not pinned to a core. During task switching, these tasks can migrate between cores. Therefore it is impossible to tell which CPU the interrupt is allocated on, which makes it difficult to free the interrupt handle and may also cause debugging difficulties. It is advised to use xTaskCreatePinnedToCore() with a specific CoreID argument to create tasks that will allocate interrupts. In the case of internal interrupt sources, this is required.

IRAM-Safe Interrupt Handlers
----------------------------

The ``ESP_INTR_FLAG_IRAM`` flag registers an interrupt handler that always runs from IRAM (and reads all its data from DRAM), and therefore does not need to be disabled during flash erase and write operations.

This is useful for interrupts which need a guaranteed minimum execution latency, as flash write and erase operations can be slow (erases can take tens or hundreds of milliseconds to complete).

It can also be useful to keep an interrupt handler in IRAM if it is called very frequently, to avoid flash cache misses.

Refer to the :ref:`SPI flash API documentation <iram-safe-interrupt-handlers>` for more details.

API Reference
-------------

.. include:: /_build/inc/esp_intr_alloc.inc


