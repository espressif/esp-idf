Random Number Generation
========================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_RF_NAME: default="Wi-Fi or Bluetooth", esp32s2="Wi-Fi", esp32h2="Bluetooth or 802.15.4 Thread/Zigbee", esp32c6="Wi-Fi or Bluetooth or 802.15.4 Thread/Zigbee"}
{IDF_TARGET_RF_IS: default="are", esp32s2="is"}
{IDF_TARGET_BOOTLOADER_RANDOM_INCOMPATIBLE: default="", esp32="I2S, "}

{IDF_TARGET_NAME} contains a hardware random number generator (RNG). You can use the APIs :cpp:func:`esp_random` and :cpp:func:`esp_fill_random` to obtained random values from it.

The hardware RNG produces true random numbers so long as one or more of the following conditions are met:

- RF subsystem is enabled. i.e., {IDF_TARGET_RF_NAME} {IDF_TARGET_RF_IS} enabled.
- An internal entropy source has been enabled by calling :cpp:func:`bootloader_random_enable` and not yet disabled by calling :cpp:func:`bootloader_random_disable`.
- While the ESP-IDF :ref:`second-stage-bootloader` is running. This is because the default ESP-IDF bootloader implementation calls :cpp:func:`bootloader_random_enable` when the bootloader starts, and :cpp:func:`bootloader_random_disable` before executing the application.

When any of these conditions are true, samples of physical noise are continuously mixed into the internal hardware RNG state to provide entropy. Consult the **{IDF_TARGET_NAME} Technical Reference Manual** > **Random Number Generator (RNG)** [`PDF <{IDF_TARGET_TRM_EN_URL}#rng>`__] chapter for more details.

If none of the above conditions are true, the output of the RNG should be considered as pseudo-random only.

Startup
-------

During startup, ESP-IDF bootloader temporarily enables a non-RF entropy source (internal reference voltage noise) that provides entropy for any first boot key generation. However, after the application starts executing, then normally only pseudo-random numbers are available until {IDF_TARGET_RF_NAME} {IDF_TARGET_RF_IS} initialized.

To re-enable the entropy source temporarily during application startup, or for an application that does not use {IDF_TARGET_RF_NAME}, call the function :cpp:func:`bootloader_random_enable` to re-enable the internal entropy source. The function :cpp:func:`bootloader_random_disable` must be called to disable the entropy source again before using ADC, {IDF_TARGET_BOOTLOADER_RANDOM_INCOMPATIBLE} {IDF_TARGET_RF_NAME}.

.. note::

    The entropy source enabled during the boot process by the ESP-IDF Second Stage Bootloader seeds the internal RNG state with some entropy. However, the internal hardware RNG state is not large enough to provide a continuous stream of true random numbers. This is why a continuous entropy source must be enabled whenever true random numbers are required.

.. note::

    If an application requires a source of true random numbers but cannot permanently enable a hardware entropy source, consider using a strong software DRBG implementation such as the mbedTLS CTR-DRBG or HMAC-DRBG, with an initial seed of entropy from hardware RNG true random numbers.

.. only:: not esp32

    Secondary Entropy
    -----------------

    {IDF_TARGET_NAME} RNG contains a secondary entropy source, based on sampling an asynchronous 8 MHz internal oscillator (see the Technical Reference Manual for details). This entropy source is always enabled in ESP-IDF and is continuously mixed into the RNG state by hardware. In testing, this secondary entropy source was sufficient to pass the `Dieharder`_ random number test suite without the main entropy source enabled (test input was created by concatenating short samples from continuously resetting {IDF_TARGET_NAME}). However, it is currently only guaranteed that true random numbers are produced when the main entropy source is also enabled as described above.

API Reference
-------------

.. include-build-file:: inc/esp_random.inc
.. include-build-file:: inc/bootloader_random.inc

``getrandom()``
---------------

A compatible version of the Linux ``getrandom()`` function is also provided for ease of porting:

.. code-block:: c

    #include <sys/random.h>

    ssize_t getrandom(void *buf, size_t buflen, unsigned int flags);

This function is implemented by calling :cpp:func:`esp_fill_random` internally.

The ``flags`` argument is ignored. This function is always non-blocking but the strength of any random numbers is dependent on the same conditions described above.

Return value is -1 (with ``errno`` set to ``EFAULT``) if the ``buf`` argument is NULL, and equal to ``buflen`` otherwise.

``getentropy()``
----------------

A compatible version of the Linux ``getentropy()`` function is also provided for easy porting:

.. code-block:: c

    #include <unistd.h>

    int getentropy(void *buffer, size_t length);

This function is implemented by calling :cpp:func:`getrandom` internally.

The strength of any random numbers is dependent on the same conditions described above.

Return value is 0 on success and -1 otherwise with ``errno`` set to:

    - ``EFAULT`` if the ``buffer`` argument is NULL.
    - ``EIO`` if the ``length`` is more then 256.

.. _Dieharder: https://webhome.phy.duke.edu/~rgb/General/dieharder.php
