网络
=====

:link_to_translation:`en:[English]`

以太网
******

``esp_eth_phy_802_3_reset_hw()`` API 变更
------------------------------------------

此变更仅适用于维护自己的以太网 PHY 驱动程序，该驱动程序基于 :component_file:`esp_eth/src/phy/esp_eth_phy_802_3.c` 通用函数。现在 :cpp:func:`esp_eth_phy_802_3_reset_hw` API 仅接受一个参数，并使用初始化期间定义的内部时序配置重置以太网 PHY。以前，该 API 需要一个 ``reset_assert_us`` 参数来指定复位引脚断言时间。此参数已删除。

使用示例：

.. code-block:: c

    esp_eth_phy_802_3_reset_hw(phy_802_3);


移除 RMII 时钟 Kconfig 选项
---------------------------

已从 `components/esp_eth` 中移除了以下 RMII 时钟相关的 Kconfig 选项。时钟配置现在仅通过 EMAC 配置结构体进行管理。

**移除的选项**：
- `ETH_PHY_INTERFACE_RMII`、`ETH_RMII_CLK_INPUT`、`ETH_RMII_CLK_OUTPUT`
- `ETH_RMII_CLK_IN_GPIO`、`ETH_RMII_CLK_OUTPUT_GPIO0`、`ETH_RMII_CLK_OUT_GPIO`

**迁移方式**：

.. code-block:: c

    // 之前：通过 Kconfig 配置
    // CONFIG_ETH_RMII_CLK_INPUT=y

    // 之后：在代码中显式配置
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;  // 或 EMAC_CLK_EXT_IN
    emac_config.clock_config.rmii.clock_gpio = 0;  // ESP32 使用 GPIO0


**影响**：使用 ``ETH_ESP32_EMAC_DEFAULT_CONFIG()`` 的应用程序可继续正常工作。自定义时钟配置需在 EMAC 配置结构体中显式设置，或使用 `Ethernet Init 组件 <https://components.espressif.com/components/espressif/ethernet_init>`_。

