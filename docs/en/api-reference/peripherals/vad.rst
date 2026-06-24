Voice Activity Detection (VAD)
==============================

:link_to_translation:`zh_CN:[中文]`


Introduction
------------

Voice Activity Detection (VAD) module facilitates the hardware implementation of the first-stage algorithm for voice wake-up and other multimedia functions.

Additionally, it provides hardware support for low-power voice wake-up solutions.

.. only:: SOC_LP_I2S_SUPPORTED

    For LP I2S documentation, see :doc:`Low Power Inter-IC Sound <./lp_i2s>`.


Hardware State Machine
----------------------

LP VAD driver provides a structure :cpp:type:`lp_vad_config_t` to configure the LP VAD module:

- :cpp:member:`lp_vad_config_t::init_frame_num`, number of init frames that are used for VAD to denoise, this helps the VAD to decrease the accidental trigger ratio. Note too big values may lead to voice activity miss.
- :cpp:member:`lp_vad_config_t::min_energy_thresh`, minimum energy threshold, voice activities with energy higher than this value will be detected.
- :cpp:member:`lp_vad_config_t::skip_band_energy_thresh`, skip band energy threshold or not, the passband energy check determines whether the proportion of passband energy within the total frequency domain meets the required threshold. Note in different environments, enabling the passband energy check may reduce false trigger rates but could also increase the rate of missed detections.
- :cpp:member:`lp_vad_config_t::speak_activity_thresh`, when in speak-activity-listening-state, if number of the detected speak activity is higher than this value, VAD runs into speak-activity-detected-state.
- :cpp:member:`lp_vad_config_t::non_speak_activity_thresh`, when in speak-activity-detected-state, if the number of the detected speak activity is higher than this value, but lower than :cpp:member:`lp_vad_config_t::max_speak_activity_thresh`,
    * if the number of the detected non-speak activity is higher than this value, VAD runs into speak-activity-listening-state
    * if the number of the detected non-speak activity is lower than this value, VAD keeps in speak-activity-detected-state
- :cpp:member:`lp_vad_config_t::min_speak_activity_thresh`, when in speak-activity-detected-state, if the number of the detected speak activity is higher than this value, but lower than :cpp:member:`lp_vad_config_t::max_speak_activity_thresh`, then the VAD state machine will depends on the value of :cpp:member:`lp_vad_config_t::non_speak_activity_thresh`
- :cpp:member:`lp_vad_config_t::max_speak_activity_thresh`, when in speak-activity-detected-state, if the number of the detected speak activity is higher than this value, VAD runs into speak-activity-listening-state

Above configurations can change the VAD state machine shown below:

.. code-block:: text

                                               ┌──────────────────────────────────┐
                                               │                                  │
                                 ┌─────────────┤  speak-activity-listening-state  │ ◄───────────────┐
                                 │             │                                  │                 │
                                 │             └──────────────────────────────────┘                 │
                                 │                          ▲                                       │
                                 │                          │                                       │
                                 │                          │                                       │
                                 │                          │                                       │
                                 │                          │                                       │
    detected speak activity      │                          │  detected speak activity              │   detected speak activity
            >=                   │                          │          >=                           │           >=
    'speak_activity_thresh'      │                          │  'min_speak_activity_thresh'          │   'max_speak_activity_thresh'
                                 │                          │                                       │
                                 │                          │          &&                           │
                                 │                          │                                       │
                                 │                          │  detected non-speak activity          │
                                 │                          │           <                           │
                                 │                          │  'non_speak_activity_thresh'          │
                                 │                          │                                       │
                                 │                          │                                       │
                                 │                          │                                       │
                                 │                          │                                       │
                                 │                          │                                       │
                                 │              ┌───────────┴─────────────────────┐                 │
                                 │              │                                 │                 │
                                 └───────────►  │ speak-activity-detected-state   ├─────────────────┘
                                                │                                 │
                                                └─┬───────────────────────────────┘
                                                  │
                                                  │                     ▲
                                                  │                     │
                                                  │                     │
                                                  │                     │  detected speak activity
                                                  │                     │          >=
                                                  │                     │  'min_speak_activity_thresh'
                                                  │                     │
                                                  │                     │          &&
                                                  │                     │
                                                  │                     │  detected non-speak activity
                                                  │                     │           <
                                                  └─────────────────────┘  'non_speak_activity_thresh'


HP Driver Functional Overview
-----------------------------

The VAD HP driver is used for configure the LP VAD to be working under the control of the HP core. The HP core can also be woken up by the VAD when voice activity is detected.

Resource Allocation
^^^^^^^^^^^^^^^^^^^

.. only:: SOC_LP_I2S_SUPPORT_VAD

    :cpp:type:`lp_vad_init_config_t` is the configuration structure that is needed to create a LP I2S VAD unit handle. To create a LP I2S VAD unit handle, you will need to first create a LP I2S channel handle. see :doc:`Low Power Inter-IC Sound <./lp_i2s>`.

    You can call :cpp:func:`lp_i2s_vad_new_unit` to create the handle. If the VAD unit is no longer used, you should recycle the allocated resource by calling :cpp:func:`lp_i2s_vad_del_unit`.

    .. code:: c

        vad_unit_handle_t vad_handle = NULL;
        lp_vad_init_config_t init_config = {
        .lp_i2s_chan = rx_handle,
        .vad_config = {
            .init_frame_num = 100,
            .min_energy_thresh = 100,
            .speak_activity_thresh = 10,
            .non_speak_activity_thresh = 30,
            .min_speak_activity_thresh = 3,
            .max_speak_activity_thresh = 100,
            },
        };
        ESP_ERROR_CHECK(lp_i2s_vad_new_unit(vad_id, init_config, &vad_handle));

        ESP_ERROR_CHECK(lp_i2s_vad_del_unit(vad_handle));

Enable and Disable the VAD
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: SOC_LP_I2S_SUPPORT_VAD

    Before using a VAD unit to detect voice activity, you need to enable the VAD unit by calling :cpp:func:`lp_i2s_vad_enable`, this function switches the driver state from **init** to **enable**, and also enables the VAD hardware. Calling :cpp:func:`lp_i2s_vad_disable` does the opposite, that is, put the driver back to the **init** state, the hardware will stop as well.

HP Core Wake-up
^^^^^^^^^^^^^^^

.. only:: SOC_LP_I2S_SUPPORT_VAD

    :cpp:func:`esp_sleep_enable_vad_wakeup` can help you to set the VAD to be working as the HP core wake-up source. To make VAD work during sleep, you should let the system maintain the RTC domain and XTAL power. See code example below:

    .. code:: c

        ESP_ERROR_CHECK(esp_sleep_enable_vad_wakeup());


LP Driver Functional Overview
-----------------------------

The VAD LP driver is mainly for LP core wake-up. The VAD can be configured under HP core control, then it can wakeup the LP core when voice activities are detected.

Resource Allocation
^^^^^^^^^^^^^^^^^^^

.. only:: SOC_LP_I2S_SUPPORT_VAD

    :cpp:type:`lp_core_lp_vad_cfg_t` and :cpp:func:`lp_core_lp_vad_init` are used to initialize the VAD LP driver.

    :cpp:func:`lp_core_lp_vad_deinit` is used to recycle the allocated resources.

Enable and Disable the VAD
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: SOC_LP_I2S_SUPPORT_VAD

    :cpp:func:`lp_core_lp_vad_enable` and :cpp:func:`lp_core_lp_vad_disable` are used for enabling / disabling the hardware.

LP Core Wake-up
^^^^^^^^^^^^^^^

.. only:: SOC_LP_I2S_SUPPORT_VAD

    Set :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_VAD` in :cpp:type:`ulp_lp_core_cfg_t` to enable the VAD to be working as the LP core wake-up source.

    .. code:: c

        static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
        {
            TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                                (firmware_end - firmware_start)) == ESP_OK);

            TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);
        }

        ulp_lp_core_cfg_t cfg = {
            .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_VAD,
        };
        load_and_start_lp_core_firmware(&cfg, lp_core_main_vad_bin_start, lp_core_main_vad_bin_end);


API Reference
-------------

.. include-build-file:: inc/lp_i2s_vad.inc
.. include-build-file:: inc/ulp_lp_core_lp_vad_shared.inc
