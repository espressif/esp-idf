:orphan:

乐鑫自定义 HCI 命令
========================

:link_to_translation:`en:[English]`

乐鑫蓝牙主机专用的自定义 HCI 命令
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下自定义 HCI 命令仅适用于乐鑫的蓝牙主机（ESP-Bluedroid 主机 或 ESP-NimBLE 主机）。如果使用非 ESP 主机或 HCI UART，这些命令将默认禁用，除非应用程序调用了相应的初始化函数。请注意，这些初始化函数及附加的自定义 HCI 命令仅供乐鑫蓝牙主机使用。应用程序开发者 **不应** 在其应用程序中调用这些初始化函数。

.. only:: esp32

    .. doxygendefine:: ESP_BT_VS_COMMON_ECHO_OCF
    .. doxygendefine:: ESP_BT_VS_SET_COEX_STATUS_OCF
    .. doxygendefine:: ESP_BT_VS_CONFIG_DUP_EXC_LIST_OCF
    .. doxygendefine:: ESP_BT_VS_SET_ADV_REPORT_FLOW_CTRL_OCF
    .. doxygendefine:: ESP_BT_VS_UPD_ADV_REPORT_FLOW_CTRL_NUM_OCF
    .. doxygendefine:: ESP_BT_VS_CLR_LEGACY_ADV_OCF
    .. doxygendefine:: ESP_BT_VS_SET_MIN_ENC_KEY_SIZE_OCF


.. only:: esp32c3 or esp32s3

    .. doxygendefine:: ESP_BT_VS_COMMON_ECHO_OCF
    .. doxygendefine:: ESP_BT_VS_CONFIG_DUP_EXC_LIST_OCF
    .. doxygendefine:: ESP_BT_VS_SET_ADV_REPORT_FLOW_CTRL_OCF
    .. doxygendefine:: ESP_BT_VS_UPD_ADV_REPORT_FLOW_CTRL_NUM_OCF
    .. doxygendefine:: ESP_BT_VS_CLR_LEGACY_ADV_OCF
    .. doxygendefine:: ESP_BT_VS_ENABLE_CSA2_OCF


.. only:: esp32c2

    .. doxygendefine:: ESP_BT_VS_SET_ADV_REPORT_FLOW_CTRL_OCF
    .. doxygendefine:: ESP_BT_VS_UPD_ADV_REPORT_FLOW_CTRL_NUM_OCF
    .. doxygendefine:: ESP_BT_VS_CLR_LEGACY_ADV_OCF
    .. doxygendefine:: ESP_BT_VS_ENABLE_CSA2_OCF
    .. doxygendefine:: ESP_BT_VS_SET_LE_VENDOR_EVTS_MASK_OCF


.. only:: esp32c5 or esp32c6 or esp32h2 or esp32c61

    .. doxygendefine:: ESP_BT_VS_CONFIG_DUP_EXC_LIST_OCF
    .. doxygendefine:: ESP_BT_VS_SET_ADV_REPORT_FLOW_CTRL_OCF
    .. doxygendefine:: ESP_BT_VS_UPD_ADV_REPORT_FLOW_CTRL_NUM_OCF
    .. doxygendefine:: ESP_BT_VS_CLR_LEGACY_ADV_OCF
    .. doxygendefine:: ESP_BT_VS_SET_DUP_LIST_PARAMS_OCF
    .. doxygendefine:: ESP_BT_VS_ENABLE_DUP_EXC_LIST_OCF
    .. doxygendefine:: ESP_BT_VS_ENABLE_ARRANGEMENT_OCF
    .. doxygendefine:: ESP_BT_VS_SET_SCHED_ROLE_LEN_OCF
    .. doxygendefine:: ESP_BT_VS_SET_PCL_RSSI_THRESH_OCF
    .. doxygendefine:: ESP_BT_VS_ENABLE_CSA2_OCF
    .. doxygendefine:: ESP_BT_VS_SET_LOG_PARAMS_OCF
    .. doxygendefine:: ESP_BT_VS_SET_LE_VENDOR_EVTS_MASK_OCF
    .. doxygendefine:: ESP_BT_VS_SET_CONST_PEER_SCA_OCF



乐鑫蓝牙主机专用的自定义 HCI 事件
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下自定义 HCI 事件仅适用于乐鑫的蓝牙主机（ESP-Bluedroid 主机 或 ESP-NimBLE 主机）。如果使用非 ESP 主机或 HCI UART，这些事件将默认禁用，除非应用程序调用了相应的初始化函数。请注意，这些初始化函数及附加的自定义 HCI 事件仅供乐鑫蓝牙主机使用。应用程序开发者 **不应** 在其应用程序中调用这些初始化函数。

.. only:: esp32

    .. doxygendefine:: ESP_BT_VS_LE_ADV_LOST_EVT_SUBCODE
    .. doxygendefine:: ESP_BT_VS_LEGACY_REM_AUTH_EVT_SUBCODE


.. only:: esp32c3 or esp32s3

    .. doxygendefine:: ESP_BT_VS_LE_ADV_LOST_EVT_SUBCODE


.. only:: esp32c2

    .. doxygendefine:: ESP_BT_VS_LE_CONN_SCAN_REQ_RXED_EVT_SUBCODE
    .. doxygendefine:: ESP_BT_VS_LE_CHAN_UPDATE_COMP_EVT_SUBCODE
    .. doxygendefine:: ESP_BT_VS_LE_SLEEP_WAKEUP_EVT_SUBCODE
    .. doxygendefine:: ESP_BT_VS_LE_ADV_LOST_EVT_SUBCODE


.. only:: esp32c6 or esp32h2 or esp32c5 or esp32c61

    .. doxygendefine:: ESP_BT_VS_LE_CONN_SCAN_REQ_RXED_EVT_SUBCODE
    .. doxygendefine:: ESP_BT_VS_LE_CHAN_UPDATE_COMP_EVT_SUBCODE
    .. doxygendefine:: ESP_BT_VS_LE_SLEEP_WAKEUP_EVT_SUBCODE
    .. doxygendefine:: ESP_BT_VS_LE_ADV_LOST_EVT_SUBCODE


乐鑫内部调试 HCI 命令
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下调试 HCI 命令已包含在乐鑫低功耗蓝牙控制器的预编译库中。这些命令不会被链接到应用程序的二进制文件中，除非应用程序中调用了 `esp_ble_internalTestFeaturesEnable(true)`。这些命令仅供乐鑫内部使用。应用程序开发者 **不应** 在其应用程序中调用 `esp_ble_internalTestFeaturesEnable(true)`。

.. only:: esp32

    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RELATED_OCF
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_ADV_DELAY_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_FOREVER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_EXPECTED_PEER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_ADV_TXED_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_SCAN_RXED_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_CLEAR_RAND_ADDR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_MAX_TXPWR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_RANGE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_AA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_ADV_AA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_CHAN_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CTRL_STATUS_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CTRL_COMPILE_VER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RELATED_SUBCMD_MAX

    以下调试 HCI 命令已包含在乐鑫经典蓝牙控制器的预编译库中。这些命令不会被链接到应用程序的二进制文件中，除非应用程序中调用了相应的初始化函数。这些命令仅供乐鑫内部使用。应用程序开发者 **不应** 在其应用程序中调用初始化函数。

    .. doxygendefine:: ESP_BT_VS_WR_DM1_ENABLE_OCF
    .. doxygendefine:: ESP_BT_VS_CLK_UPDATE_OCF
    .. doxygendefine:: ESP_BT_VS_SET_AFH_OCF
    .. doxygendefine:: ESP_BT_VS_SET_EVT_MASK_OCF
    .. doxygendefine:: ESP_BT_VS_SET_AFH_REPORTING_MODE_OCF
    .. doxygendefine:: ESP_BT_VS_MASK_RMT_CHANNEL_CLASSIFICATION_OCF
    .. doxygendefine:: ESP_BT_VS_WR_AUTO_RATE_INIT_OCF


.. only:: esp32c3 or esp32s3

    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RELATED_OCF
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_ADV_DELAY_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_PREF_CODED_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_DEFAULT_PRIV_MODE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_FOREVER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_EXPECTED_PEER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_ADV_TXED_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_SCAN_RXED_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_ENH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_ENH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_CCA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_CLEAR_RAND_ADDR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_MAX_TXPWR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_RANGE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_AA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_ADV_AA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_CHAN_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CTRL_STATUS_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CTRL_COMPILE_VER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_AUX_ADV_OFFSET_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_AUX_OFFSET_THRESHOLD_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RELATED_SUBCMD_MAX

.. only:: esp32c2

    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RELATED_OCF
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_ADV_DELAY_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_PREF_CODED_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_DEFAULT_PRIV_MODE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_FOREVER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_EXPECTED_PEER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_ADV_TXED_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_SCAN_RXED_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_ENH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_ENH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_IGNORE_WL_FOR_DIR_ADV_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_ADV_RXED_RSSI_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_CCA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_CCA_WIN_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_READ_CCA_DATA_SUBCM
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_CLEAR_RAND_ADDR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_MAX_TXPWR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_RANGE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_AA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_ADV_AA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_CHAN_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SKIP_LIGHT_SLEEP_CHECK_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_WAKEUP_OVERHEAD_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_ADV_MIN_ITVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CTRL_STATUS_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_RECODE_RX_STATE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_RECODE_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_CLR_RECODE_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CTRL_COMPILE_VER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_AUX_ADV_OFFSET_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_BACKOFF_UPLIMIT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_RXED_ADV_ADI_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_RX_SENS_THRESH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_AGC_MAX_GAIN_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RELATED_SUBCMD_MAX


.. only::  esp32c6 or esp32h2 or esp32c5 or esp32c61

    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RELATED_OCF
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_ADV_DELAY_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_PREF_CODED_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_DEFAULT_PRIV_MODE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_FOREVER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_EXPECTED_PEER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_ADV_TXED_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_SCAN_RXED_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_ENH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_ENH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_IGNORE_WL_FOR_DIR_ADV_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_ADV_RXED_RSSI_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_CCA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_CCA_WIN_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_READ_CCA_DATA_SUBCM
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_CLEAR_RAND_ADDR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_MAX_TXPWR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXPWR_RANGE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_AA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_ADV_AA_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCAN_CHAN_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SKIP_LIGHT_SLEEP_CHECK_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_WAKEUP_OVERHEAD_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_ADV_MIN_ITVL_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CTRL_STATUS_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_CONN_PHY_TXPWR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CONN_PHY_TXPWR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_RXBUF_EMPTY_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RESTART_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_RECODE_RX_STATE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_RECODE_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_CLR_RECODE_CNT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_CTRL_COMPILE_VER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_AUX_ADV_OFFSET_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_INIT_FLEXIBLE_MODE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_FLEXIBLE_MODE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_FLEXIBLE_CONN_ERR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_FLEXIBLE_ADV_ERR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_FLEXIBLE_SCAN_ERR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_TXED_CRCERR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_BACKOFF_UPLIMIT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_RXED_ADV_ADI_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCH_RAND_MODE_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_RX_SENS_THRESH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_CHECK_MSYS_BUF_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_UPDATE_BLE_TIMER_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_UPDATE_BLE_RTC_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_LOCKED_MEM_NUM_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ALLOW_MEM_ALLOC_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_SCH_RAND_INFO_PTR_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_DIAG_IO_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_AGC_MAX_GAIN_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_ENABLE_CHAN_ASSESS_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_BACKOFF_UPLIMIT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_CONN_TOP_PRIO_RESV_THRESH_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_SET_TEST_EVT_MSK_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_GET_WAKEUP_TIMEOUT_SUBCMD
    .. doxygendefine:: ESP_BT_VS_CFG_TEST_RELATED_SUBCMD_MAX



.. only:: esp32 or esp32c5 or esp32c6 or esp32c61 or esp32h2

    乐鑫内部调试 HCI 事件
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    以下调试 HCI 事件已包含在乐鑫蓝牙控制器的预编译库中。这些事件不会被链接到应用程序的二进制文件中，除非应用程序中调用了相应的初始化函数。这些事件仅供乐鑫内部使用。应用程序开发者 **不应** 在其应用程序中调用初始化函数。

    .. only:: esp32

        .. doxygendefine:: ESP_BT_VS_AFH_CHG_EVT_SUBCODE
        .. doxygendefine:: ESP_BT_VS_CH_CLASSIFICATION_EVT_SUBCODE
        .. doxygendefine:: ESP_BT_VS_CH_CLASSIFICATION_REPORTING_MODE_EVT_SUBCODE


    .. only:: esp32c5 or esp32c6 or esp32c61 or esp32h2

        .. doxygendefine:: ESP_BT_VS_LE_RUNNING_STATUS_EVT_SUBCODE
