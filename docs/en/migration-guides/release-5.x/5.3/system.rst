System
======

:link_to_translation:`zh_CN:[中文]`

Power Management
-----------------------

* ``esp_sleep_enable_ext1_wakeup_with_level_mask`` is deprecated, use :cpp:func:`esp_sleep_enable_ext1_wakeup_io` and :cpp:func:`esp_sleep_disable_ext1_wakeup_io` instead.

Unit Testing
-----------------------

In the past versions of Unity framework, it was possible to omit a semicolon at the end of a ``TEST_ASSERT_*`` macro statement. This is no longer the case in the newer version of Unity, used in IDF v5.3.

For example, the following code:

.. code-block:: c

    TEST_ASSERT(some_func() == ESP_OK)

will now result in a compilation error. To fix this, add a semicolon at the end of the statement:

    TEST_ASSERT(some_func() == ESP_OK);

Partition Table
---------------

Partition Table generation tool has been fixed to ensure that the size of partition of type ``app`` is having flash sector (minimum erase size) aligned size (please see :ref:`partition-offset-and-size`). If the partition does not have aligned size, partition table generator tool will raise an error. This fix ensures that OTA updates for a case where the file size is close or equal to the size of partition works correctly (erase operation does not go beyond the partition size).

In case you have the ``app`` partition size which is not a multiple of the 4 KB then please note that while migrating to ESP-IDF 5.3, you must align this size to its lower 4 KB boundary for the build to succeed. This does not impact the partition table for existing devices as such but ensures that generated firmware size remains within the OTA update capablilty limit.
