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
the peripheral attached to it, with only one ISR that will get called. Non-shared interrupts can have multiple peripherals triggering 
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

Peripherals that can generate interrupts can be divided in two types: external peripherals, outside the Xtensa
cores in the ESP32, and internal peripherals, inside the ESP32. Interrupt handling differs slightly between
these two types of peripherals.

Each Xtensa core has its own set of internal peripherals: three timer comparators, a performance monitor and two
software interrupts. These peripherals can only be configured from the core they are associated with. When
generating an interrupt, the interrupt they generate is hard-wired to their associated core; it's not possible
to have e.g. an internal timer comparator of one core generate an interrupt on another core. That is why these
sources can only be managed using a task running on that specific core. Internal interrupt sources are still
allocatable using esp_intr_alloc as normal, but they cannot be shared and will always have a fixed interrupt
level (namely, the one associated in hardware with the peripheral). Internal interrupt sources are defined
in esp_intr_alloc.h as ETS_INTERNAL_*_INTR_SOURCE.

The remaining interrupt slots in both cores are wired to an interrupt multiplexer, which can be used to
route any external interrupt source to any of these interrupt slots. Allocating an external interrupt will always
allocate it on the core that does the allocation, and freeing the interrupt should always happen on the same
core. Disabling and enabling the interrupt from another core is allowed, however. External interrupts can
share an interrupt slot bu passing ESP_INTR_FLAG_SHARED as a flag to esp_intr_alloc. External interrupt sources 
are defined in soc/soc.h as ETS_*_INTR_SOURCE.

Care should be taken when allocating an interrupt using a task not pinned to a certain core; while running
code not in a critical secion, these tasks can migrate between cores at any moment, possibly making an
interrupt operation fail because of the reasons mentioned above. It is advised to always use 
xTaskCreatePinnedToCore with a specific CoreID argument to create tasks that will handle interrupts.

Application Example
-------------------

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `esp_intr_alloc.h <https://github.com/espressif/esp-idf/blob/master/components/esp32/include/esp_intr_alloc.h>`_


Macros
^^^^^^

.. doxygendefine:: ESP_INTR_FLAG_LEVEL1
.. doxygendefine:: ESP_INTR_FLAG_LEVEL2
.. doxygendefine:: ESP_INTR_FLAG_LEVEL3
.. doxygendefine:: ESP_INTR_FLAG_LEVEL4
.. doxygendefine:: ESP_INTR_FLAG_LEVEL5
.. doxygendefine:: ESP_INTR_FLAG_LEVEL6
.. doxygendefine:: ESP_INTR_FLAG_NMI
.. doxygendefine:: ESP_INTR_FLAG_LOWMED
.. doxygendefine:: ESP_INTR_FLAG_HIGH
.. doxygendefine:: ESP_INTR_FLAG_SHARED
.. doxygendefine:: ESP_INTR_FLAG_EDGE
.. doxygendefine:: ESP_INTR_FLAG_IRAM
.. doxygendefine:: ESP_INTR_FLAG_INTRDISABLED

Type Definitions
^^^^^^^^^^^^^^^^

Enumerations
^^^^^^^^^^^^

Structures
^^^^^^^^^^

Functions
^^^^^^^^^

.. doxygenfunction:: esp_intr_mark_shared
.. doxygenfunction:: esp_intr_reserve
.. doxygenfunction:: esp_intr_alloc
.. doxygenfunction:: esp_intr_alloc_intrstatus
.. doxygenfunction:: esp_intr_free
.. doxygenfunction:: esp_intr_get_cpu
.. doxygenfunction:: esp_intr_get_intno
.. doxygenfunction:: esp_intr_disable
.. doxygenfunction:: esp_intr_enable
.. doxygenfunction:: esp_intr_noniram_disable
.. doxygenfunction:: esp_intr_noniram_enable
