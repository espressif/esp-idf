GATT DEFINES
============

Overview
--------

`Instructions`_

Application Example
-------------------

`Instructions`_

.. _Instructions: ../template.html


API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`bt/bluedroid/api/include/esp_gatt_defs.h`


Macros
^^^^^^

.. doxygendefine:: ESP_GATT_UUID_IMMEDIATE_ALERT_SVC
.. doxygendefine:: ESP_GATT_UUID_LINK_LOSS_SVC
.. doxygendefine:: ESP_GATT_UUID_TX_POWER_SVC
.. doxygendefine:: ESP_GATT_UUID_CURRENT_TIME_SVC
.. doxygendefine:: ESP_GATT_UUID_REF_TIME_UPDATE_SVC
.. doxygendefine:: ESP_GATT_UUID_NEXT_DST_CHANGE_SVC
.. doxygendefine:: ESP_GATT_UUID_GLUCOSE_SVC
.. doxygendefine:: ESP_GATT_UUID_HEALTH_THERMOM_SVC
.. doxygendefine:: ESP_GATT_UUID_DEVICE_INFO_SVC
.. doxygendefine:: ESP_GATT_UUID_HEART_RATE_SVC
.. doxygendefine:: ESP_GATT_UUID_PHONE_ALERT_STATUS_SVC
.. doxygendefine:: ESP_GATT_UUID_BATTERY_SERVICE_SVC
.. doxygendefine:: ESP_GATT_UUID_BLOOD_PRESSURE_SVC
.. doxygendefine:: ESP_GATT_UUID_ALERT_NTF_SVC
.. doxygendefine:: ESP_GATT_UUID_HID_SVC
.. doxygendefine:: ESP_GATT_UUID_SCAN_PARAMETERS_SVC
.. doxygendefine:: ESP_GATT_UUID_RUNNING_SPEED_CADENCE_SVC
.. doxygendefine:: ESP_GATT_UUID_CYCLING_SPEED_CADENCE_SVC
.. doxygendefine:: ESP_GATT_UUID_CYCLING_POWER_SVC
.. doxygendefine:: ESP_GATT_UUID_LOCATION_AND_NAVIGATION_SVC
.. doxygendefine:: ESP_GATT_UUID_USER_DATA_SVC
.. doxygendefine:: ESP_GATT_UUID_WEIGHT_SCALE_SVC
.. doxygendefine:: ESP_GATT_UUID_PRI_SERVICE
.. doxygendefine:: ESP_GATT_UUID_SEC_SERVICE
.. doxygendefine:: ESP_GATT_UUID_INCLUDE_SERVICE
.. doxygendefine:: ESP_GATT_UUID_CHAR_DECLARE
.. doxygendefine:: ESP_GATT_UUID_CHAR_EXT_PROP
.. doxygendefine:: ESP_GATT_UUID_CHAR_DESCRIPTION
.. doxygendefine:: ESP_GATT_UUID_CHAR_CLIENT_CONFIG
.. doxygendefine:: ESP_GATT_UUID_CHAR_SRVR_CONFIG
.. doxygendefine:: ESP_GATT_UUID_CHAR_PRESENT_FORMAT
.. doxygendefine:: ESP_GATT_UUID_CHAR_AGG_FORMAT
.. doxygendefine:: ESP_GATT_UUID_CHAR_VALID_RANGE
.. doxygendefine:: ESP_GATT_UUID_EXT_RPT_REF_DESCR
.. doxygendefine:: ESP_GATT_UUID_RPT_REF_DESCR
.. doxygendefine:: ESP_GATT_UUID_GAP_DEVICE_NAME
.. doxygendefine:: ESP_GATT_UUID_GAP_ICON
.. doxygendefine:: ESP_GATT_UUID_GAP_PREF_CONN_PARAM
.. doxygendefine:: ESP_GATT_UUID_GAP_CENTRAL_ADDR_RESOL
.. doxygendefine:: ESP_GATT_UUID_GATT_SRV_CHGD
.. doxygendefine:: ESP_GATT_UUID_ALERT_LEVEL
.. doxygendefine:: ESP_GATT_UUID_TX_POWER_LEVEL
.. doxygendefine:: ESP_GATT_UUID_CURRENT_TIME
.. doxygendefine:: ESP_GATT_UUID_LOCAL_TIME_INFO
.. doxygendefine:: ESP_GATT_UUID_REF_TIME_INFO
.. doxygendefine:: ESP_GATT_UUID_NW_STATUS
.. doxygendefine:: ESP_GATT_UUID_NW_TRIGGER
.. doxygendefine:: ESP_GATT_UUID_ALERT_STATUS
.. doxygendefine:: ESP_GATT_UUID_RINGER_CP
.. doxygendefine:: ESP_GATT_UUID_RINGER_SETTING
.. doxygendefine:: ESP_GATT_UUID_GM_MEASUREMENT
.. doxygendefine:: ESP_GATT_UUID_GM_CONTEXT
.. doxygendefine:: ESP_GATT_UUID_GM_CONTROL_POINT
.. doxygendefine:: ESP_GATT_UUID_GM_FEATURE
.. doxygendefine:: ESP_GATT_UUID_SYSTEM_ID
.. doxygendefine:: ESP_GATT_UUID_MODEL_NUMBER_STR
.. doxygendefine:: ESP_GATT_UUID_SERIAL_NUMBER_STR
.. doxygendefine:: ESP_GATT_UUID_FW_VERSION_STR
.. doxygendefine:: ESP_GATT_UUID_HW_VERSION_STR
.. doxygendefine:: ESP_GATT_UUID_SW_VERSION_STR
.. doxygendefine:: ESP_GATT_UUID_MANU_NAME
.. doxygendefine:: ESP_GATT_UUID_IEEE_DATA
.. doxygendefine:: ESP_GATT_UUID_PNP_ID
.. doxygendefine:: ESP_GATT_UUID_HID_INFORMATION
.. doxygendefine:: ESP_GATT_UUID_HID_REPORT_MAP
.. doxygendefine:: ESP_GATT_UUID_HID_CONTROL_POINT
.. doxygendefine:: ESP_GATT_UUID_HID_REPORT
.. doxygendefine:: ESP_GATT_UUID_HID_PROTO_MODE
.. doxygendefine:: ESP_GATT_UUID_HID_BT_KB_INPUT
.. doxygendefine:: ESP_GATT_UUID_HID_BT_KB_OUTPUT
.. doxygendefine:: ESP_GATT_UUID_HID_BT_MOUSE_INPUT
.. doxygendefine:: ESP_GATT_HEART_RATE_MEAS
.. doxygendefine:: ESP_GATT_BODY_SENSOR_LOCATION
.. doxygendefine:: ESP_GATT_HEART_RATE_CNTL_POINT
.. doxygendefine:: ESP_GATT_UUID_BATTERY_LEVEL
.. doxygendefine:: ESP_GATT_UUID_SC_CONTROL_POINT
.. doxygendefine:: ESP_GATT_UUID_SENSOR_LOCATION
.. doxygendefine:: ESP_GATT_UUID_RSC_MEASUREMENT
.. doxygendefine:: ESP_GATT_UUID_RSC_FEATURE
.. doxygendefine:: ESP_GATT_UUID_CSC_MEASUREMENT
.. doxygendefine:: ESP_GATT_UUID_CSC_FEATURE
.. doxygendefine:: ESP_GATT_UUID_SCAN_INT_WINDOW
.. doxygendefine:: ESP_GATT_UUID_SCAN_REFRESH
.. doxygendefine:: ESP_GATT_ILLEGAL_UUID
.. doxygendefine:: ESP_GATT_ILLEGAL_HANDLE
.. doxygendefine:: ESP_GATT_ATTR_HANDLE_MAX
.. doxygendefine:: ESP_GATT_MAX_ATTR_LEN
.. doxygendefine:: ESP_GATT_RSP_BY_APP
.. doxygendefine:: ESP_GATT_AUTO_RSP
.. doxygendefine:: ESP_GATT_IF_NONE

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_gatt_if_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_gatt_prep_write_type
.. doxygenenum:: esp_gatt_status_t
.. doxygenenum:: esp_gatt_conn_reason_t
.. doxygenenum:: esp_gatt_auth_req_t
.. doxygenenum:: esp_gatt_perm_t
.. doxygenenum:: esp_gatt_char_prop_t
.. doxygenenum:: esp_gatt_write_type_t

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_attr_desc_t
    :members:

.. doxygenstruct:: esp_attr_control_t
    :members:

.. doxygenstruct:: esp_gatts_attr_db_t
    :members:

.. doxygenstruct:: esp_attr_value_t
    :members:

.. doxygenstruct:: esp_gatts_incl_svc_desc_t
    :members:

.. doxygenstruct:: esp_gatts_incl128_svc_desc_t
    :members:

.. doxygenstruct:: esp_gatt_value_t
    :members:

.. doxygenstruct:: esp_gatt_rsp_t
    :members:


Functions
^^^^^^^^^


