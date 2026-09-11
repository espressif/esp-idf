
.. flat-table::
    :widths: 1 3 3

    * - Max CPU Frequency Set
      - Lock Acquisition
      - CPU and APB Frequencies
    * - :rspan:`2` 320
      - ``ESP_PM_CPU_FREQ_MAX`` acquired
      -
        - CPU: 320 MHz
        - APB: 53.3 MHz
    * - ``ESP_PM_APB_FREQ_MAX`` acquired, ``ESP_PM_CPU_FREQ_MAX`` not acquired
      -
        - CPU: 53.3 MHz
        - APB: 53.3 MHz
    * - None
      - Min values for both frequencies set with :cpp:func:`esp_pm_configure`
    * - :rspan:`2` 240
      - ``ESP_PM_CPU_FREQ_MAX`` acquired
      -
        - CPU: 240 MHz
        - APB: 40 MHz
    * - ``ESP_PM_APB_FREQ_MAX`` acquired, ``ESP_PM_CPU_FREQ_MAX`` not acquired
      -
        - CPU: 40 MHz
        - APB: 40 MHz
    * - None
      - Min values for both frequencies set with :cpp:func:`esp_pm_configure`
