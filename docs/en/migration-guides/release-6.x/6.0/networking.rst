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
