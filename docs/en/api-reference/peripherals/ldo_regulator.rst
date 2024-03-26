Low Dropout Voltage Regulator (LDO)
===================================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_LDO_CHANNELS:default="unknown", esp32p4="4"}

Introduction
------------

The {IDF_TARGET_NAME} chip internally integrates {IDF_TARGET_LDO_CHANNELS} channels of low-dropout voltage regulators (LDOs). Each channel's voltage is programmable. In our hardware reference designs, some of these LDO outputs are typically used to power the internal Flash and PSRAM, while the remaining LDOs can be used to supply external devices.

.. note::

    It's essential to read the manual first and ensure that the required current does not exceed the chip's specifications.

Functional Overview
-------------------

The description of the LDO driver is divided into the following sections:

- :ref:`ldo-channel-acquisition` - Introduces the types of LDO channels and how to apply for LDO channel resources.
- :ref:`ldo-adjust-voltage` - Describes how to adjust the voltage of the LDO channel.

.. _ldo-channel-acquisition:

LDO Channel Acquisition
^^^^^^^^^^^^^^^^^^^^^^^

LDO channels can be classified into two types: fixed voltage and adjustable voltage. For a fixed voltage channel, it allows multiple users to simultaneously use it (in software, we allow a variable to have multiple immutable references ). However, for an adjustable voltage channel, only one user is allowed to use it at a time (in software, we don't allow a variable to have multiple mutable references or coexistence of mutable and immutable references).

In the driver, the LDO channel is represented by the :cpp:type:`esp_ldo_channel_handle_t`. You can use the :cpp:func:`esp_ldo_acquire_channel` function to request LDO channel resources. Upon successful acquisition, a handle for the LDO channel will be returned, which can be used for subsequent voltage adjustment operations. When applying for a channel, the :cpp:type:`esp_ldo_channel_config_t` structure is used to specify the basic information of the LDO channel, including the channel ID, the desired output voltage, and whether the voltage can be dynamically adjusted.

- :cpp:member:`esp_ldo_channel_config_t::chan_id` - LDO channels are uniquely identified by a label, which is used to distinguish different LDO channels. Please note that this information needs to be determined based on the circuit schematic and chip datasheet. For example, an LDO channel labeled as ``LDO_VO3`` corresponds to an ID of ``3``.
- :cpp:member:`esp_ldo_channel_config_t::voltage_mv` - The desired output voltage of the LDO channel, in millivolts.
- :cpp:member:`esp_ldo_channel_config_t::ldo_extra_flags::adjustable` - Whether the LDO channel's output voltage can be dynamically adjusted. Only when it is set to `true`, can the :cpp:func:`esp_ldo_channel_adjust_voltage` function be used to dynamically adjust the output voltage.

Since multiple users are allowed to use a fixed voltage LDO channel simultaneously, the driver internally maintains a reference counter. The LDO channel will be automatically closed when the last user releases the LDO channel resources. The function to release LDO channel resources is :cpp:func:`esp_ldo_release_channel`. Additionally, it is important to note that the acquisition and release of LDO channels should appear in pairs during usage.

.. _ldo-adjust-voltage:

LDO Voltage Adjustment
^^^^^^^^^^^^^^^^^^^^^^

:cpp:func:`esp_ldo_channel_adjust_voltage` function is used to adjust the output voltage of an LDO channel at runtime. However, please note that this function can only be used for LDO channels with adjustable voltage. Attempting to use this function on a fixed voltage LDO channel will result in an error.

Also, it is important to keep in mind that due to hardware limitations, the LDO channel voltage may have a deviation of approximately 50-100mV. Therefore, it is not advisable to rely on the LDO channel's output voltage for precise analog control.

Application Examples
--------------------

.. list::

    :SOC_MIPI_DSI_SUPPORTED: * Use the internal LDO channel to power up the MIPI DPHY: :example:`peripherals/lcd/mipi_dsi`

API Reference
-------------

.. include-build-file:: inc/esp_ldo_regulator.inc
