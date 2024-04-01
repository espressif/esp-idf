PHY
==================

:link_to_translation:`en:[English]`

多根天线
-------------------

多根天线功能原理和组成
++++++++++++++++++++++++++++

多根天线功能主要通过将内部天线模块工作信号输出到具体 IO 引脚上，通过 IO 引脚控制外部天线切换器选择指定天线，最多支持 16 根天线。

下图描述多根天线功能组成::

                         ___________________________________________________________________________
                    ____|____                          _________                                    |
                   |PHY      |--antenna_select[0] --> |         |                                   |
                ___|Antenna 0|--antenna_select[1] --> |         |                                   |
               /   |Module   |--antenna_select[2] --> |         |                               ____|____
              /    |_________|--antenna_select[3] --> | IO MUX  |--GPIO[x0] --> ant_sel_in[0]--|         | --- antenna 0
    RX/TX ___/          |                             | And     |--GPIO[x1] --> ant_sel_in[1]--| Antenna | --- antenna 1
             \      ____|____                         | GPIO    |--GPIO[x2] --> ant_sel_in[2]--| Switch  | ...  ...
              \    |PHY      |--antenna_select[0] --> | Matrix  |--GPIO[x3] --> ant_sel_in[3]--|_________| --- antenna 15
               \___|Antenna 1|--antenna_select[1] --> |         |
                   |Module   |--antenna_select[2] --> |         |
                   |_________|--antenna_select[3] --> |_________|

{IDF_TARGET_NAME} 多根天线功能主要包含 3 个部分：芯片内部的 PHY 天线模块、IO MUX 和 GPIO Matrix、以及外部的天线切换器。

1.芯片内部的天线选择模块 PHY Antenna Module：
- 两个天线模块均支持工作于发送 (TX) 或接收 (RX) 模式，可以通过软件配置发送和接收选用某个模块。
- 每个天线模块最多支持输出 4 个天线选择信号 antenna_select[3:0]，该信号值可由软件配置并且可以一一映射到任意 IO 引脚。
- 当某个天线模块处于工作状态时，IO 引脚的高低电平值为软件配置的信号值。

2.IO MUX 和 GPIO Matrix：
- 将内部 4 路天线信号输出到具体的 IO 引脚上。

3.外部的天线切换器：
- 一般为多路选择器，通过 ant_sel_in[x] 引脚的电平，选择工作的天线，例如 ant_sel_in[3:0] 为 "0b1011"，表示选中天线 11。

多根天线使用步骤
+++++++++++++++++++++++++++

1.根据硬件电路设计及外部天线切换器确定用于控制天线切换的 IO 引脚。

2.配置天线选择信号输出到指定 IO 引脚
- API :cpp:func:`esp_phy_set_ant_gpio()` 用于配置 antenna_selects[3:0] 信号连接 GPIO[x3:x0]。如果 GPIO[x0] 连接到 antenna_select[0]，gpio_config->gpio_cfg[x0].gpio_select 应设置为 1，且 gpio_config->gpio_cfg[x0].gpio_num 的值为 GPIO[x0]。

3.配置内部天线工作模式及输出信号
- API :cpp:func:`esp_phy_set_ant()` 用于配置发送或接收时使用内部天线模块 0 或 1，并配置当天线模块 0 或 1 工作时的输出信号值。
- 对于 ESP_PHY_ANT_MODE_AUTO 模式目前不推荐使用。

多根天线配置参考例子
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

注意事项
++++++++++++++++++++++++++

1.不同天线切换器，`ant_sel_in[3:0]` 的输入值中可能存在非法值，即 {IDF_TARGET_NAME} 通过外部天线开关支持的天线数可能小于 16 根。例如，ESP32-WROOM-DA 使用 RTC6603SP 作为天线切换器，仅支持 2 根天线。两个天线选择输入管脚为高电平有效，连接到两个 GPIO。'0b01' 表示选中天线 0，'0b10' 表示选中天线 1。输入值 '0b00' 和 '0b11' 为非法值。

2.尽管最多支持 16 根天线，发送和接收数据时，最多仅能同时使能两根天线。

3.对于 ESP_PHY_ANT_MODE_AUTO 模式目前不推荐使用，主要有以下限制情况需要考虑：
 - 因为发送数据天线基于 ESP_PHY_ANT_MODE_AUTO 类型的接收数据天线选择算法，只有接收数据的天线模式为 ESP_PHY_ANT_MODE_AUTO 时，发送数据天线才能设置为 ESP_PHY_ANT_MODE_AUTO。
 - 接收或者发送天线模式配置为 ESP_PHY_ANT_MODE_AUTO 时，只要存在 RF 信号的恶化，很容易触发天线切换。如果射频信号不稳定，天线会频繁切换，使得总的射频性能无法达到预期效果。

推荐在以下场景中使用多根天线
++++++++++++++++++++++++++++

1.应用程序可以始终选择指定的天线，也可以执行自身天线选择算法，如根据应用程序收集的信息来选择天线模式等。请参考 IDF 示例 :idf_file:`examples/phy/antenna/README.md` 来设计天线选择算法。

2.接收/发送数据的天线模式均配置为 ESP_PHY_ANT_MODE_ANT0 或 ESP_PHY_ANT_MODE_ANT1。

