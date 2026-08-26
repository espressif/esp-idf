Tools
=====

:link_to_translation:`zh_CN:[中文]`

``idf.py flash`` default behavior (fast reflash)
------------------------------------------------

The default behavior of ``idf.py flash`` has changed. When ``*_flashed.bin`` files exist in the build directory (e.g. from a previous successful flash), ``idf.py flash`` now performs **fast reflashing**: it only writes the flash regions that changed, which speeds up repeated flashing during development. After flashing, it verifies that the device flash content matches the expected binaries; if verification fails, a full flash is performed.

If you prefer the previous behavior where all binaries are always written in full (regardless of existing ``*_flashed.bin`` files), you can force a complete flash by using the full flash mode: run ``idf.py flash -a`` or ``idf.py flash --all``. This ensures that every binary is re-written to the device, which is recommended when working with brand-new, empty, or erased chips, or whenever you want to disable fast reflash and incremental flashing.

For more details, see :ref:`flash-with-idf-py` in the *IDF Python Tools* guide.
