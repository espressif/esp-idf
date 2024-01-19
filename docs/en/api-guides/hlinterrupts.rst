High Priority Interrupts
========================

:link_to_translation:`zh_CN:[中文]`

.. toctree::
   :maxdepth: 1

The Xtensa architecture supports 32 interrupts, divided over 7 priority levels from level 1 to 7, with level 7 being an non-maskable interrupt (NMI), plus an assortment of exceptions. On the {IDF_TARGET_NAME}, the :doc:`../api-reference/system/intr_alloc` can route most interrupt sources to these interrupts via the interrupt mux. Normally, interrupts are written in C, but ESP-IDF allows high-priority interrupts to be written in assembly as well, resulting in very low interrupt latencies.

Interrupt Priorities
--------------------

.. only:: esp32

  .. list-table::
      :header-rows: 1
      :widths: 20 30 50
      :align: center

      * - Priority Level
        - Symbol
        - Remark
      * - 1
        - N/A
        - Exception and low priority interrupts, handled by ESP-IDF.
      * - 2-3
        - N/A
        - Medium priority interrupts, handled by ESP-IDF.
      * - 4
        - xt_highint4
        - High priority interrupt, free to use. [1]_
      * - 5
        - xt_highint5
        - Normally used by ESP-IDF debug logic. [1]_
      * - NMI
        - xt_nmi
        - Non-maskable interrupt, free to use.
      * - dbg
        - xt_debugexception
        - Debug exception. Called on e.g., a BREAK instruction. [2]_

  .. [1] ESP-IDF debug logic can be configured to run on ``xt_highint4`` or ``xt_highint5`` in :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL`. Bluetooth's interrupt can be configured to run on priority level 4 by enabling :ref:`CONFIG_BTDM_CTRL_HLI`. If :ref:`CONFIG_BTDM_CTRL_HLI` is enabled, ESP-IDF debug logic must be running on priority level 5 interrupt.
  .. [2] If :ref:`CONFIG_BTDM_CTRL_HLI` is enabled, ``xt_debugexception`` is used to fix the `live lock issue <https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf>`_ in ESP32 ECO3.

.. only:: not esp32

  .. list-table::
      :header-rows: 1
      :widths: 20 30 50
      :align: center

      * - Priority Level
        - Symbol
        - Remark
      * - 1
        - N/A
        - Exception and low priority interrupts, handled by ESP-IDF.
      * - 2-3
        - N/A
        - Medium priority interrupts, handled by ESP-IDF.
      * - 4
        - xt_highint4
        - Normally used by ESP-IDF debug logic.
      * - 5
        - xt_highint5
        - High priority interrupts, free to use.
      * - NMI
        - xt_nmi
        - Non-maskable interrupt, free to use.
      * - dbg
        - xt_debugexception
        - Debug exception. Called on e.g., a BREAK instruction.

Using these symbols is done by creating an assembly file with suffix ``.S`` and defining the named symbols, like this:

.. code-block:: none

        .section .iram1,"ax"
        .global     xt_highint5
        .type       xt_highint5,@function
        .align      4
    xt_highint5:
        ... your code here
        rsr     a0, EXCSAVE_5
        rfi     5

For a real-life example, see the :component_file:`esp_system/port/soc/{IDF_TARGET_PATH_NAME}/highint_hdl.S` file; the panic handler interrupt is implemented there.


Notes
-----

- Do not call C code from a high-priority interrupt; as these interrupts are run from a critical section, this can cause the target to crash. Note that although the panic handler interrupt does call normal C code, this exception is allowed due to the fact that this handler never returns (i.e., the application does not continue to run after the panic handler), so breaking C code execution flow is not a problem.

.. only:: esp32

  When :ref:`CONFIG_BTDM_CTRL_HLI` is enabled, C code is also called from a high-priority interrupt, this is possible thanks to some additional protection added to it.

- Make sure your assembly code gets linked in. Indeed, as the free-to-use symbols are declared as weak, the linker may discard the file containing the symbol. This happens if the only symbol defined, or used from the user file is the ``xt_*`` free-to-use symbol. To avoid this, in the assembly file containing the ``xt_*`` symbol, define another symbol, like:

.. code-block:: none

  .global ld_include_my_isr_file
  ld_include_my_isr_file:

Here it is called ``ld_include_my_isr_file`` but can have any name, as long as it is not defined anywhere else in the project.

Then, in the component ``CMakeLists.txt``, add this name as an unresolved symbol to the ld command line arguments:

.. code-block:: none

  target_link_libraries(${COMPONENT_TARGET} "-u ld_include_my_isr_file")

This will ensure the linker to always includes the file defining ``ld_include_my_isr_file``, so that the ISR is always linked.

- High-priority interrupts can be routed and handled using :cpp:func:`esp_intr_alloc` and associated functions. The handler and handler arguments to :cpp:func:`esp_intr_alloc` must be NULL, however.

- In theory, medium priority interrupts could also be handled in this way. ESP-IDF does not support this yet.

- To check Xtensa instruction set architecture (ISA), please refer to `Xtensa ISA Summary <https://www.cadence.com/content/dam/cadence-www/global/en_US/documents/tools/ip/tensilica-ip/isa-summary.pdf>`_.
