Deep Sleep Wake Stubs
=====================

ESP32 supports running a "deep sleep wake stub" when coming out of deep sleep. This function runs immediately as soon as the chip wakes up - before any normal initialisation, bootloader, or ESP-IDF code has run. After the wake stub runs, the SoC can go back to sleep or continue to start ESP-IDF normally.

Deep sleep wake stub code is loaded into "RTC Fast Memory" and any data which it uses must also be loaded into RTC memory. RTC memory regions hold their contents during deep sleep.

Rules for Wake Stubs
--------------------

Wake stub code must be carefully written:

* As the SoC has freshly woken from sleep, most of the peripherals are in reset states. The SPI flash is unmapped.

* The wake stub code can only call functions implemented in ROM or loaded into RTC Fast Memory (see below.)

* The wake stub code can only access data loaded in RTC memory. All other RAM will be unintiailised and have random contents. The wake stub can use other RAM for temporary storage, but the contents will be overwritten when the SoC goes back to sleep or starts ESP-IDF.

* RTC memory must include any read-only data (.rodata) used by the stub.

* Data in RTC memory is initialised whenever the SoC restarts, except when waking from deep sleep. When waking from deep sleep, the values which were present before going to sleep are kept.

* Wake stub code is a part of the main esp-idf app. During normal running of esp-idf, functions can call the wake stub functions or access RTC memory. It is as if these were regular parts of the app.

Implementing A Stub
-------------------

The wake stub in esp-idf is called ``esp_wake_deep_sleep()``. This function runs whenever the SoC wakes from deep sleep. There is a default version of this function provided in esp-idf, but the default function is weak-linked so if your app contains a function named ``esp_wake_deep_sleep()`` then this will override the default.

If supplying a custom wake stub, the first thing it does should be to call ``esp_default_wake_deep_sleep()``.

It is not necessary to implement ``esp_wake_deep_sleep()`` in your app in order to use deep sleep. It is only necessary if you want to have special behaviour immediately on wake.

If you want to swap between different deep sleep stubs at runtime, it is also possible to do this by calling the ``esp_set_deep_sleep_wake_stub()`` function. This is not necessary if you only use the default ``esp_wake_deep_sleep()`` function.

All of these functions are declared in the ``esp_deepsleep.h`` header under components/esp32.

Loading Code Into RTC Memory
----------------------------

Wake stub code must be resident in RTC Fast Memory. This can be done in one of two ways.

The first way is to use the ``RTC_IRAM_ATTR`` attribute to place a function into RTC memory::

    void RTC_IRAM_ATTR esp_wake_deep_sleep(void) {
        esp_default_wake_deep_sleep();
        // Add additional functionality here
    }

The second way is to place the function into any source file whose name starts with ``rtc_wake_stub``. Files names ``rtc_wake_stub*`` have their contents automatically put into RTC memory by the linker.

The first way is simpler for very short and simple code, or for source files where you want to mix "normal" and "RTC" code. The second way is simpler when you want to write longer pieces of code for RTC memory.


Loading Data Into RTC Memory
----------------------------

Data used by stub code must be resident in RTC memory. The data can be placed in RTC Fast memory or in RTC Slow memory which is also used by the ULP.

Specifying this data can be done in one of two ways:

The first way is to use the ``RTC_DATA_ATTR`` and ``RTC_RODATA_ATTR`` to specify any data (writeable or read-only, respectively) which should be loaded into RTC memory::

    RTC_DATA_ATTR int wake_count;

    void RTC_IRAM_ATTR esp_wake_deep_sleep(void) {
        esp_default_wake_deep_sleep();
        static RTC_RODATA_ATTR const char fmt_str[] = "Wake count %d\n";
        ets_printf(fmt_str, wake_count++);
    }

The RTC memory area where this data will be placed can be configured via menuconfig option named ``CONFIG_ESP32_RTCDATA_IN_FAST_MEM``. This option allows to keep slow memory area for ULP programs and once it is enabled the data marked with ``RTC_DATA_ATTR`` and ``RTC_RODATA_ATTR`` are placed in the RTC fast memory segment otherwise it goes to RTC slow memory (default option). This option depends on the ``CONFIG_FREERTOS_UNICORE`` because RTC fast memory can be accessed only by PRO_CPU.

The similar attributes ``RTC_FAST_ATTR`` and ``RTC_SLOW_ATTR`` can be used to specify data that will be force placed into RTC_FAST and RTC_SLOW memory respectively. Any access to data marked with ``RTC_FAST_ATTR`` is allowed by PRO_CPU only and it is responsibility of user to make sure about it.

Unfortunately, any string constants used in this way must be declared as arrays and marked with RTC_RODATA_ATTR, as shown in the example above.

The second way is to place the data into any source file whose name starts with ``rtc_wake_stub``.

For example, the equivalent example in ``rtc_wake_stub_counter.c``::

    int wake_count;

    void RTC_IRAM_ATTR esp_wake_deep_sleep(void) {
        esp_default_wake_deep_sleep();
        ets_printf("Wake count %d\n", wake_count++);
    }

The second way is a better option if you need to use strings, or write other more complex code.

To reduce wake-up time use the `CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` Kconfig option, see more information in :doc:`Fast boot from Deep Sleep <bootloader>`.
