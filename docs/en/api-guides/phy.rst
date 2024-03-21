PHY
==================

:link_to_translation:`zh_CN:[中文]`

Multiple Antennas
--------------------------
The multiple antennas selecting can be depicted as following picture::

                    __________
                   |Enabled   |
                ___|Antenna 0 |\\                                              _________
                   |__________| \\        GPIO[0] <----> antenna_select[0] ---|         | --- antenna 0
    RX/TX ___                    \\____\  GPIO[1] <----> antenna_select[1] ---| Antenna | --- antenna 1
             \      __________   //    /  GPIO[2] <----> antenna_select[2] ---| Switch  | ...  ...
              \ ___|Enabled   | //        GPIO[3] <----> antenna_select[3] ---|_________| --- antenna 15
               \   |Antenna 1 |//
                   |__________|


{IDF_TARGET_NAME} supports up to sixteen antennas through external antenna switch. The antenna switch can be controlled by up to four address pins - antenna_select[0:3]. Different input value of antenna_select[0:3] means selecting different antenna. For example, the value '0b1011' means the antenna 11 is selected. The default value of antenna_select[3:0] is '0b0000', which means the antenna 0 is selected by default.

Up to four GPIOs are connected to the four active high antenna_select pins. {IDF_TARGET_NAME} can select the antenna by control the GPIO[0:3]. The API :cpp:func:`esp_phy_set_ant_gpio()` is used to configure which GPIOs are connected to antenna_selects. If GPIO[x] is connected to antenna_select[x], then gpio_config->gpio_cfg[x].gpio_select should be set to 1 and gpio_config->gpio_cfg[x].gpio_num should be provided.

For the specific implementation of the antenna switch, there may be illegal values in `antenna_select[0:3]`. It means that {IDF_TARGET_NAME} may support less than sixteen antennas through the switch. For example, ESP32-WROOM-DA which uses RTC6603SP as the antenna switch, supports two antennas. Two GPIOs are connected to two active high antenna selection inputs. The value '0b01' means the antenna 0 is selected, the value '0b10' means the antenna 1 is selected. Values '0b00' and '0b11' are illegal.

Although up to sixteen antennas are supported, only one or two antennas can be simultaneously enabled for RX/TX. The API :cpp:func:`esp_phy_set_ant()` is used to configure which antennas are enabled.

The enabled antennas selecting algorithm is also configured by :cpp:func:`esp_phy_set_ant()`. The RX/TX antenna mode can be :cpp:enumerator:`ESP_PHY_ANT_MODE_ANT0`, :cpp:enumerator:`ESP_PHY_ANT_MODE_ANT1`, or :cpp:enumerator:`ESP_PHY_ANT_MODE_AUTO`. If the antenna mode is :cpp:enumerator:`ESP_PHY_ANT_MODE_ANT0`, the enabled antenna 0 is selected for RX/TX data. If the antenna mode is :cpp:enumerator:`ESP_PHY_ANT_MODE_ANT1`, the enabled antenna 1 is selected for RX/TX data. Otherwise, Wi-Fi automatically selects the enabled antenna that has better signal.

If the RX antenna mode is :cpp:enumerator:`ESP_PHY_ANT_MODE_AUTO`, the default antenna mode also needs to be set, because the RX antenna switching only happens when some conditions are met. For example, the RX antenna starts to switch if the RSSI is lower than -65 dBm or another antenna has better signal. RX uses the default antenna if the conditions are not met. If the default antenna mode is :cpp:enumerator:`ESP_PHY_ANT_MODE_ANT1`, the enabled antenna 1 is used as the default RX antenna, otherwise the enabled antenna 0 is used.

Some limitations need to be considered:

 - The TX antenna can be set to :cpp:enumerator:`ESP_PHY_ANT_MODE_AUTO` only if the RX antenna mode is :cpp:enumerator:`ESP_PHY_ANT_MODE_AUTO`, because TX antenna selecting algorithm is based on RX antenna in :cpp:enumerator:`ESP_PHY_ANT_MODE_AUTO` type.
 - When the TX antenna mode or RX antenna mode is configured to :cpp:enumerator:`ESP_PHY_ANT_MODE_AUTO` the switching mode will easily trigger the switching phase, as long as there is deterioration of the RF signal. So in situations where the RF signal is not stable, the antenna switching will occur frequently, resulting in an RF performance that may not meet expectations.

Following is the recommended scenarios to use the multiple antennas:

 - The applications can always choose to select a specified antenna or implement their own antenna selecting algorithm, e.g., selecting the antenna mode based on the information collected by the application. Refer to ESP-IDF example :idf_file:`examples/phy/antenna/README.md` for the antenna selecting algorithm design.
 - Both RX/TX antenna modes are configured to ESP_PHY_ANT_MODE_ANT0 or ESP_PHY_ANT_MODE_ANT1.


Multiple Antennas Configuration
+++++++++++++++++++++++++++++++++++++

Generally, following steps can be taken to configure the multiple antennas:

- Configure which GPIOs are connected to the antenna_selects. For example, if four antennas are supported and GPIO20/GPIO21 are connected to antenna_select[0]/antenna_select[1], the configurations look like:

.. code-block:: c

    esp_phy_ant_gpio_config_t ant_gpio_config = {
        .gpio_cfg[0] = { .gpio_select = 1, .gpio_num = 20 },
        .gpio_cfg[1] = { .gpio_select = 1, .gpio_num = 21 }
    };

- Configure which antennas are enabled and how RX/TX use the enabled antennas. For example, if antenna1 and antenna3 are enabled, the RX needs to select the better antenna automatically and uses antenna1 as its default antenna, the TX always selects the antenna3. The configuration looks like:

.. code-block:: c

    esp_phy_ant_config_t config = {
        .rx_ant_mode = ESP_PHY_ANT_MODE_AUTO,
        .rx_ant_default = ESP_PHY_ANT_ANT0,
        .tx_ant_mode = ESP_PHY_ANT_MODE_ANT1,
        .enabled_ant0 = 1,
        .enabled_ant1 = 3
    };
