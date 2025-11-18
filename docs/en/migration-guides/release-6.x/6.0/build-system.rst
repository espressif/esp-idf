Build System
============

:link_to_translation:`zh_CN:[中文]`

Linker Orphan-Handling Behavior Changed to Error
------------------------------------------------

Starting with ESP-IDF v6.0, the build system no longer allows orphan sections in the final ELF file. The linker will now produce an error if any orphan sections are encountered during linking.

.. note::

    An *orphan section* is a section that is not explicitly placed into any output section by the linker script and is not discarded during linking.

How to Resolve Orphan Section Errors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you encounter an orphan section error during linking, you can resolve it using one of the following methods:

1. Remove the code or data that causes the orphan section, if it's unused or unnecessary.
2. Explicitly place the orphan section using a :ref:`linker fragment file <ldgen-linker-fragment-files>`.
3. Suppress errors by setting :ref:`CONFIG_COMPILER_ORPHAN_SECTIONS` to ``warning`` or ``place``.

.. warning::

    The option 3 is **not recommended**, as orphan sections may indicate misconfigured memory mapping or unintentional behavior in your application.

Change in Global Constructor Order
----------------------------------

Initially, global constructors were executed using the internal ``do_global_ctors()``` function. This approach was used to support Xtensa targets, which emit ``.ctors.*`` sections ordered in **descending** order.

On RISC-V targets, the toolchain emits ``.init_array.*`` sections, which follow a standard **ascending** order. While priority constructors in ``.init_array.*`` were correctly processed, the non-priority ``.init_array`` section was previously handled in **descending** order and matched the Xtensa ``.ctors`` behavior.

Starting from ESP-IDF v6.0, the startup code uses ``__libc_init_array()``, consistent with standard toolchain behavior. This function processes both priority and non-priority constructors in **ascending** order.

To support this behavior, the following breaking changes were introduced:

- On Xtensa targets ``.ctors.*`` entries are now converted to ascending order to ensure compatibility with ``__libc_init_array()``.
- The processing order of non-priority ``.init_array`` and legacy ``.ctors`` sections was changed from **descending** to **ascending**.

These changes align ESP-IDF with toolchain expectations and improve consistency across supported architectures.

If the application relies on the previous (descending) constructor order and is affected by this change, consider the following approaches.

Update Constructor Registration Logic
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In some cases, data structures were built assuming that constructors run in reverse order. To preserve the original behavior, update the registration logic to insert new items at the tail instead of the head.

Example (from ``components/unity/unity_runner.c``):

.. code-block:: diff

    -    // Insert at the head
    -    desc->next = s_unity_tests_first;
    -    s_unity_tests_first = desc;
    +    // Insert at the end
    +    _unity_tests_last->next = desc;
    +    s_unity_tests_last = desc;

.. note::

    This approach is suitable only for very specific cases.

Use Constructor Priorities
~~~~~~~~~~~~~~~~~~~~~~~~~~

To explicitly control constructor order, use the ``constructor()`` function attribute with a numeric priority:

.. code-block:: c

    __attribute__((constructor(PRIO)))
    void foo(void);

Replace ``PRIO`` with an integer value. Lower values are executed earlier. This is the preferred method when specific ordering is required.

Changes in Configuration Files Syntax
-------------------------------------

ESP-IDF v6 uses esp-idf-kconfig v3, which introduces several changes in the configuration (``Kconfig``) files. For the full list of changes, please refer to the `Migration Guide From esp-idf-kconfig v2.x to v3.x <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/developer-guide/migration-guide.html>`_.

Compiler Options
----------------

The default compiler warnings will be considered as errors. The configuration option :ref:`CONFIG_COMPILER_DISABLE_DEFAULT_ERRORS` has been changed to N.
