
.. flat-table::
    :widths: 1 3 3

    * - Max CPU Frequency Set
      - Lock Acquisition
      - CPU and APB Frequencies
    * - :rspan:`2` 96
      - ``ESP_PM_CPU_FREQ_MAX`` acquired
      -
        - CPU: 96 MHz
        - APB: 32 MHz
    * - ``ESP_PM_APB_FREQ_MAX`` acquired, ``ESP_PM_CPU_FREQ_MAX`` not acquired
      -
        - CPU: 32 MHz
        - APB: 32 MHz
    * - None
      - Min values for both frequencies set with :cpp:func:`esp_pm_configure`
    * - :rspan:`2` 64
      - ``ESP_PM_CPU_FREQ_MAX`` acquired
      -
        - CPU: 64 MHz
        - APB: 32 MHz
    * - ``ESP_PM_APB_FREQ_MAX`` acquired, ``ESP_PM_CPU_FREQ_MAX`` not acquired
      -
        - CPU: 32 MHz
        - APB: 32 MHz
    * - None
      - Min values for both frequencies set with :cpp:func:`esp_pm_configure`
    * - :rspan:`2` 48
      - ``ESP_PM_CPU_FREQ_MAX`` acquired
      -
        - CPU: 48 MHz
        - APB: 32 MHz
    * - ``ESP_PM_APB_FREQ_MAX`` acquired, ``ESP_PM_CPU_FREQ_MAX`` not acquired
      -
        - CPU: 32 MHz
        - APB: 32 MHz
    * - None
      - Min values for both frequencies set with :cpp:func:`esp_pm_configure`
