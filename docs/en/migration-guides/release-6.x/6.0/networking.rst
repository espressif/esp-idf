Networking
===========

:link_to_translation:`zh_CN:[中文]`

Ethernet
********

``esp_eth_phy_802_3_reset_hw()`` API Changes
--------------------------------------------

This change only applies if you maintain your own Ethernet PHY driver based on :component_file:`esp_eth/src/phy/esp_eth_phy_802_3.c` common functions. The :cpp:func:`esp_eth_phy_802_3_reset_hw` API accepts only one parameter now and resets the Ethernet PHY with internal timing configuration which is defined during initialization. Previously, the API required a ``reset_assert_us`` parameter to specify the reset pin assertion time. This parameter has been removed.

Usage example:

.. code-block:: c

    esp_eth_phy_802_3_reset_hw(phy_802_3);


Removed RMII Clock Kconfig Options
----------------------------------

Removed the following RMII clock Kconfig options from `components/esp_eth`. Clock configuration is now handled exclusively through EMAC config structures.

**Removed Options**:
- `ETH_PHY_INTERFACE_RMII`, `ETH_RMII_CLK_INPUT`, `ETH_RMII_CLK_OUTPUT`
- `ETH_RMII_CLK_IN_GPIO`, `ETH_RMII_CLK_OUTPUT_GPIO0`, `ETH_RMII_CLK_OUT_GPIO`

**Migration**:

.. code-block:: c

    // Before: Configuration via Kconfig
    // CONFIG_ETH_RMII_CLK_INPUT=y

    // After: Explicit configuration in code
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;  // or EMAC_CLK_EXT_IN
    emac_config.clock_config.rmii.clock_gpio = 0;  // GPIO0 for ESP32


**Impact**: Applications using ``ETH_ESP32_EMAC_DEFAULT_CONFIG()`` continue to work. Custom clock configurations must be set explicitly in the EMAC config structure or use the `Ethernet Init component <https://components.espressif.com/components/espressif/ethernet_init>`_.

