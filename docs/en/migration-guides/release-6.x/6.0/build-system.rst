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
