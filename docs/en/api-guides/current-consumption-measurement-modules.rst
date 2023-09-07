Current Consumption Measurement of Modules
==========================================

{IDF_TARGET_SOC_BOOT_PIN:default="Not updated", esp32="IO0", esp32s2="IO0", esp32s3="IO0", esp32c3="IO9", esp32c2="IO9", "esp32c6"="IO9", "esp32h2"="IO9"}

You may want to know the current consumption of a `module <https://www.espressif.com/en/products/modules>`__ in deep-sleep mode, :doc:`other power-saving modes </api-reference/system/sleep_modes>`, and active mode to develop some applications sensitive to power consumption. This section introduces how to measure the current consumption of a module running such an application.


Notes to Measurement
--------------------

Can We Use a Development Board?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32c6

    For {IDF_TARGET_NAME}, you can use development boards such as `ESP32-C6-DevKitC-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/index.html>`__ and `ESP32-C6-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitm-1/index.html>`__ to measure current consumption of corresponding modules as these development boards are equipped with headers, which can be used to measure current drawn by modules.

.. only:: esp32h2

    For {IDF_TARGET_NAME}, you can use development boards such as `ESP32-H2-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32h2/esp32-h2-devkitm-1/index.html>`__ to measure current consumption of corresponding modules as these development boards are equipped with headers, which can be used to measure current drawn by modules.

.. only:: esp32c6 or esp32h2

    With such development boards, you can measure current consumption of modules in deep-sleep mode by flashing chips with the :example:`deep_sleep <system/deep_sleep>` example. However, you can also measure current of bare modules equipped with {IDF_TARGET_NAME} chip using the following method.

.. only:: esp32 or esp32s2 or esp32s3 or esp32c2 or esp32c3

    For {IDF_TARGET_NAME}, using a development board directly to measure current consumption of the corresponding module is not recommended, as some circuits still consume power on the board even when you flash the chip with the :example:`deep_sleep <system/deep_sleep>` example. Therefore, you need to cut off the power supply circuit to the module to measure the module's current. This method is inconvenient and increases measurement costs.


How to Choose an Appropriate Ammeter?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the :example:`deep_sleep <system/deep_sleep>` example, the module will be woken up every 20 seconds. In deep-sleep mode, the current in the module is just several microamps (μA), while in active mode, the current is in the order of milliamps (mA). The high dynamic current range makes accurate measurement difficult. Ordinary ammeters cannot dynamically switch the measurement range fast enough.

Additionally, ordinary ammeters have a relatively high internal resistance, resulting in a significant voltage drop. This may cause the module to enter an unstable state, as it is powered by a voltage smaller than the minimum required voltage supply.

Therefore, an ammeter suitable for measuring current in deep-sleep mode should have low internal resistance and, ideally, switch current ranges dynamically. We recommend two options: the `Joulescope ammeter <https://www.joulescope.com/>`__ and the `Power Profiler Kit II from Nordic <https://www.nordicsemi.com/Products/Development-hardware/Power-Profiler-Kit-2.?lang=en>`__.


Joulescope Ammeter
""""""""""""""""""

The Joulescope ammeter combines high-speed sampling and rapid dynamic current range switching to provide accurate and seamless current and energy measurements, even for devices with rapidly varying current consumption. Joulescope accurately measures electrical current over nine orders of magnitude from amps down to nanoamps. This wide range allows for accurate and precise current measurements for devices. Additionally, Joulescope has a total voltage drop of 25 mV at 1 A, which keeps the module running normally. These two features make Joulescope a perfect option for measuring the module switching between deep-sleep mode and wake-up mode.

Joulescope has no display screen. You need to connect it to a PC to visualize the current waveforms of the measured module. For specific instructions, please follow the documentation provided by the manufacturer.


Nordic Power Profiler Kit II
""""""""""""""""""""""""""""

The Nordic Power Profiler Kit II has an advanced analog measurement unit with a high dynamic measurement range. This allows for accurate power consumption measurements for the entire range typically seen in low-power embedded applications, all the way from single μAs to 1 A. The resolution varies between 100 nA and 1 mA, depending on the measurement range, and is high enough to detect small spikes often seen in low-power optimized systems.


Hardware Connection
-------------------

To measure the power consumption of a bare module, you need an `ESP-Prog <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/other/esp-prog/user_guide.html>`__ to flash the :example:`deep_sleep <system/deep_sleep>` example to the module and power the module during measurement, a suitable ammeter (here we use the Joulescope ammeter), a computer, and of course a bare module with necessary jumper wires. For the connection, please refer to the following figure.

.. figure:: /../_static/hardware_connection_power_measure.png
    :align: center
    :scale: 80%
    :alt: Hardware Connection (click to enlarge)

    Hardware Connection (click to enlarge)

Please connect the pins of **UART TX**, **UART RX**, **SPI Boot**, **Enable**, and **Ground** on the measured module with corresponding pins on ESP-Prog, and connect the **VPROG** pin on ESP-Prog with the **IN+** port on the Joulescope ammeter and connect its **OUT+** port with the **Power supply (3V3)** pin on the measured module. For the specific names of these pins in different modules, please refer to the list below.

.. list-table:: Pin Names of Modules Based on {IDF_TARGET_NAME} Chip
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - Function of Module Pin
      - Pin Name

    * - UART TX
      - TXD0

    * - UART RX
      - RXD0

    * - SPI Boot
      - {IDF_TARGET_SOC_BOOT_PIN}

    * - Enable
      - EN

    * - Power Supply
      - 3V3

    * - Ground
      - GND

.. only:: esp32

    For modules based on the ESP32 chip, the names of UART TX and UART RX pins may also be U0TXD and U0RXD.

.. only:: esp32c2

    For modules based on the ESP32-C2 chip, the names of UART TX and UART RX pins may also be TXD and RXD.

.. only:: esp32c3

    For modules based on the ESP32-C3 chip, the names of UART TX and UART RX pins may also be TXD and RXD, TX and RX, or TX0 and RX0.

For details of the pin names, please refer to the `datasheet of specific module <https://www.espressif.com/en/support/download/documents/modules>`__.


Measurement Steps
-----------------

ESP32-S3-WROOM-1 is used as an example in the measurement, and other modules can be measured similarly. For the specific current consumption of chips in different modes, please refer to the Current Consumption subsection in the corresponding `chip datasheet <https://www.espressif.com/en/support/download/documents/chips?keys=&field_download_document_type_tid%5B%5D=510>`__.

You can refer to the following steps to measure the current in deep-sleep mode.

- Connect the aforementioned devices according to the hardware connection.

- Flash the :example:`deep_sleep <system/deep_sleep>` example to the module. For details, please refer to :doc:`Start a Project on Linux and macOS </get-started/linux-macos-start-project>` for a computer with Linux or macOS system or :doc:`Start a Project on Windows </get-started/windows-start-project>` for a computer with Windows system.

.. only:: esp32 or esp32s2 or esp32s3

    Please note that when you configure the example by running ``idf.py menuconfig``, you need to disable ``Enable touch wake up`` in the ``Example Configuration`` to reduce the bottom current.

.. only:: esp32

    For modules with an external resistor on GPIO12 (such as ESP32-WROVER-E/IE), you should call :cpp:func:`rtc_gpio_isolate` before going into deep sleep. This is to isolate the GPIO12 pin from external circuits to further minimize current consumption. Please note, for other modules, you do not have to call this function, otherwise, you may get abnormal results.

- By default, the module will be woken up every 20 seconds (you can change the timing by modifying the code of this example). To check if the example runs as expected, you can monitor the module operation by running ``idf.py -p PORT monitor`` (please replace PORT with your serial port name).

- Open the Joulescope software to see the current waveform as shown in the image below.

From the waveforms, you can obtain that the current of the module in deep-sleep mode is 8.14 μA. In addition, you can also see the current of the module in active mode, which is about 23.88 mA. The waveforms also show that the average power consumption during deep-sleep mode is 26.85 μW, and the average power consumption during active mode is 78.32 mW.

.. figure:: /../_static/current_measure_waveform.png
    :align: center
    :scale: 100%
    :alt: Current Waveform of ESP32-S3-WROOM-1 (click to enlarge)

    Current Waveform of ESP32-S3-WROOM-1 (click to enlarge)

The figure below shows the total power consumption of one cycle is 6.37 mW.

.. figure:: /../_static/power_measure_waveform.png
    :align: center
    :scale: 100%
    :alt: Power Consumption of ESP32-S3-WROOM-1 (click to enlarge)

    Power Consumption of ESP32-S3-WROOM-1 (click to enlarge)

By referring to these power consumption in different modes, you can estimate the power consumption of your applications and choose the appropriate power source.
