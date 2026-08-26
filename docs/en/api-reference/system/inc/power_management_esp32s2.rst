
.. flat-table::
    :widths: 1 3 3

    * - Max CPU Frequency Set
      - Lock Acquisition
      - CPU and APB Frequencies
    * - :rspan:`2` 240
      - ``ESP_PM_CPU_FREQ_MAX`` acquired
      -
        - CPU: 240 MHz
        - APB: 80 MHz
    * - ``ESP_PM_APB_FREQ_MAX`` acquired, ``ESP_PM_CPU_FREQ_MAX`` not acquired
      -
        - CPU: 80 MHz
        - APB: 80 MHz
    * - None
      - Min values for both frequencies set with :cpp:func:`esp_pm_configure`
    * - :rspan:`2` 160
      - ``ESP_PM_CPU_FREQ_MAX`` acquired
      -
        - CPU: 160 MHz
        - APB: 80 MHz
    * - ``ESP_PM_APB_FREQ_MAX`` acquired, ``ESP_PM_CPU_FREQ_MAX`` not acquired
      -
        - CPU: 80 MHz
        - APB: 80 MHz
    * - None
      - Min values for both frequencies set with :cpp:func:`esp_pm_configure`
    * - :rspan:`1` 80
      - Any of ``ESP_PM_CPU_FREQ_MAX``or ``ESP_PM_APB_FREQ_MAX`` acquired
      -
        - CPU: 80 MHz
        - APB: 80 MHz
    * - None
      - Min values for both frequencies set with :cpp:func:`esp_pm_configure`
