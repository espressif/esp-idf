PHY
==================

:link_to_translation:`en:[English]`

多根天线
-------------------
下图描述多根天线的选择过程::

                    __________
                   |Enabled   |
                ___|Antenna 0 |\\                                              _________
                   |__________| \\        GPIO[0] <----> antenna_select[0] ---|         | --- antenna 0
    RX/TX ___                    \\____\  GPIO[1] <----> antenna_select[1] ---| Antenna | --- antenna 1
             \      __________   //    /  GPIO[2] <----> antenna_select[2] ---| Switch  | ...  ...
              \ ___|Enabled   | //        GPIO[3] <----> antenna_select[3] ---|_________| --- antenna 15
               \   |Antenna 1 |//
                   |__________|


{IDF_TARGET_NAME} 通过外部天线开关，最多支持 16 根天线。天线开关最多可由四个地址管脚控制 - antenna_select[0:3]。向 antenna_select[0:3] 输入不同的值，以选择不同的天线。例如，输入值 '0b1011' 表示选中天线 11。antenna_select[3:0] 的默认值为 "0b0000"，表示默认选择了天线 0。

四个高电平有效 antenna_select 管脚有多达四个 GPIO 连接。{IDF_TARGET_NAME} 可以通过控制 GPIO[0:3] 选择天线。API :cpp:func:`esp_phy_set_ant_gpio()` 用于配置 antenna_selects 连接哪些 GPIO。如果 GPIO[x] 连接到 antenna_select[x]，gpio_config->gpio_cfg[x].gpio_select 应设置为 1，且要提供 gpio_config->gpio_cfg[x].gpio_num 的值。

天线开关的具体实现不同，`antenna_select[0:3]` 的输入值中可能存在非法值，即 {IDF_TARGET_NAME} 通过外部天线开关支持的天线数可能小于 16 根。例如，ESP32-WROOM-DA 使用 RTC6603SP 作为天线开关，仅支持 2 根天线。两个天线选择输入管脚为高电平有效，连接到两个 GPIO。'0b01' 表示选中天线 0，'0b10' 表示选中天线 1。输入值 '0b00' 和 '0b11' 为非法值。

尽管最多支持 16 根天线，发送和接收数据时，最多仅能同时使能两根天线。API :cpp:func:`esp_phy_set_ant()` 用于配置使能哪些天线。

使能天线后，选择算法的过程同样可由 :cpp:func:`esp_phy_set_ant()` 配置。接收/发送数据源的天线模式可以是 ESP_PHY_ANT_MODE_ANT0、ESP_PHY_ANT_MODE_ANT1 或 ESP_PHY_ANT_MODE_AUTO。如果天线模式为 ESP_PHY_ANT_MODE_AUTO，使能的天线 0 用于接收/发送数据。如果天线模式为 ESP_PHY_ANT_MODE_ANT1，使能天线 1 用于接收/发送数据。否则，会自动选择使能天线中信号较好的天线。

如果接收数据的天线模式为 ESP_PHY_ANT_MODE_AUTO，还需要设置默认天线模式。只有在满足某些条件时，接收数据天线才会切换，例如，如果 RSSI 低于 -65 dBm，或另一根天线信号更好。如果条件不满足，接收数据使用默认天线。如果默认天线模式为 ESP_PHY_ANT_MODE_ANT1，则使能的天线 1 是默认接收数据天线，否则是使能的天线 0。

有一些限制情况需要考虑：

 - 因为发送数据天线基于 ESP_PHY_ANT_MODE_AUTO 类型的接收数据天线选择算法，只有接收数据的天线模式为 ESP_PHY_ANT_MODE_AUTO 时，发送数据天线才能设置为 ESP_PHY_ANT_MODE_AUTO。
 - 接收或者发送天线模式配置为 ESP_PHY_ANT_MODE_AUTO 时，只要存在 RF 信号的恶化，很容易触发天线切换。如果射频信号不稳定，天线会频繁切换，使得总的射频性能无法达到预期效果。

推荐在以下场景中使用多根天线：

 - 应用程序可以始终选择指定的天线，也可以执行自身天线选择算法，如根据应用程序收集的信息来选择天线模式等。请参考 IDF 示例 :idf_file:`examples/phy/antenna/README.md` 来设计天线选择算法。
 - 接收/发送数据的天线模式均配置为 ESP_PHY_ANT_MODE_ANT0 或 ESP_PHY_ANT_MODE_ANT1。


多根天线配置
++++++++++++++++++++++++++++

通常，可以执行以下步骤来配置多根天线：

- 配置 antenna_selects 连接哪些 GPIOs，例如，如果支持四根天线，且 GPIO20/GPIO21 连接到 antenna_select[0]/antenna_select[1]，配置如下所示:

.. code-block:: c

    esp_phy_ant_gpio_config_t ant_gpio_config = {
        .gpio_cfg[0] = { .gpio_select = 1, .gpio_num = 20 },
        .gpio_cfg[1] = { .gpio_select = 1, .gpio_num = 21 }
    };

- 配置使能哪些天线、以及接收/发送数据如何使用使能的天线，例如，如果使能了天线 1 和天线 3，接收数据需要自动选择较好的天线，并将天线 1 作为默认天线，发送数据始终选择天线 3。配置如下所示:

.. code-block:: c

    esp_phy_ant_config_t config = {
        .rx_ant_mode = ESP_PHY_ANT_MODE_AUTO,
        .rx_ant_default = ESP_PHY_ANT_ANT0,
        .tx_ant_mode = ESP_PHY_ANT_MODE_ANT1,
        .enabled_ant0 = 1,
        .enabled_ant1 = 3
    };
