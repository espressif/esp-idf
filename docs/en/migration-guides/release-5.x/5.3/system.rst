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
