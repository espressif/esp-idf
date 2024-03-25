PHY
==================

:link_to_translation:`zh_CN:[中文]`

Multiple Antennas
--------------------------

Principles and Components of Multiple Antennas
+++++++++++++++++++++++++++++++++++++++++++++++

Multi-antenna functionality primarily involves routing signals from internal antenna modules to specific IO pins, controlling external antenna switches through IO pins to select designated antennas, supporting up to 16 antennas.

The components of multiple antennas can be depicted as following picture::

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

{IDF_TARGET_NAME} Multiple antennas primarily consists of three parts: the PHY Antenna Module inside the chip, IO MUX and GPIO Matrix, and external antenna switches.


1. PHY Antenna Module:
- Both antenna modules support operation in transmit (TX) or receive (RX) mode, and can be configured via software to select a particular module for transmission or reception.
- Each antenna module supports outputting up to 4 antenna selection signals antenna_select[3:0], which can be configured by software and mapped to any IO pin individually.
- When an antenna module is in operation, the logic level of the IO pin corresponds to the configured signal value.

2. IO MUX and GPIO Matrix:
- Routes the internal 4-way antenna signals to specific IO pins.

3. External Antenna Switches:
- Typically multi-way selectors, they choose the working antenna based on the logic level of the ant_sel_in[x] pin. For example, ant_sel_in[3:0] as "0b1011" selects antenna 11.

Steps for Multi-Antenna Usage
++++++++++++++++++++++++++++++

1. Determine the IO pins used for controlling antenna switching based on hardware circuit design and external antenna switches.

2. Configure antenna selection signals to output to specified IO pins.
- API :cpp:func:`esp_phy_set_ant_gpio()` is used to configure antenna_selects[3:0] signals to connect with GPIO[x3:x0]. If GPIO[x0] is connected to antenna_select[0], gpio_config->gpio_cfg[x0].gpio_select should be set to 1, and the value of gpio_config->gpio_cfg[x0].gpio_num should be GPIO[x0].

3. Configure internal antenna operation mode and output signals.
- API :cpp:func:`esp_phy_set_ant()` is used to configure the use of internal antenna module 0 or 1 for transmission or reception, and to configure the output signal values when antenna module 0 or 1 is in operation.
- ESP_PHY_ANT_MODE_AUTO mode is currently not recommended for use.

Multi-Antenna Configuration Reference Example
+++++++++++++++++++++++++++++++++++++++++++++++

Typically, the following steps can be performed to configure multi-antenna:
- Configure antenna_selects to connect with which GPIOs. For example, if four antennas are supported and GPIO20/GPIO21 are connected to antenna_select[0]/antenna_select[1], the configuration is as follows:

.. code-block:: c

    esp_phy_ant_gpio_config_t ant_gpio_config = {
        .gpio_cfg[0] = { .gpio_select = 1, .gpio_num = 20 },
        .gpio_cfg[1] = { .gpio_select = 1, .gpio_num = 21 }
    };

- Configure which antennas are enabled and how enabled antennas are used for receiving/sending data. For example, if antennas 1 and 3 are enabled, data reception needs to automatically select the better antenna, with antenna 1 set as the default antenna, and data transmission always selecting antenna 3. The configuration is as follows:

.. code-block:: c

    esp_phy_ant_config_t config = {
        .rx_ant_mode = ESP_PHY_ANT_MODE_AUTO,
        .rx_ant_default = ESP_PHY_ANT_ANT0,
        .tx_ant_mode = ESP_PHY_ANT_MODE_ANT1,
        .enabled_ant0 = 1,
        .enabled_ant1 = 3
    };

Notes
++++++++++++++++++++++++++

1. Different antenna switches may have invalid input values for `ant_sel_in[3:0]`, meaning the number of antennas supported by {IDF_TARGET_NAME} via external antenna switches may be less than 16. For example, ESP32-WROOM-DA uses RTC6603SP as the antenna switch, supporting only 2 antennas. The two antenna selection input pins are active high and are connected to two GPIOs. '0b01' indicates antenna 0 is selected, '0b10' indicates antenna 1 is selected. Input values '0b00' and '0b11' are invalid.

2. Despite supporting up to 16 antennas, only a maximum of two antennas can be enabled simultaneously for sending and receiving data.

3. The use of ESP_PHY_ANT_MODE_AUTO mode is currently not recommended, primarily due to the following limitations:
- For the antenna selection algorithm based on ESP_PHY_ANT_MODE_AUTO type for sending data, the antenna for sending data can only be set to ESP_PHY_ANT_MODE_AUTO when the antenna mode for receiving data is ESP_PHY_ANT_MODE_AUTO.
- When the receiving or sending antenna mode is configured as ESP_PHY_ANT_MODE_AUTO, frequent antenna switching may occur if RF signal degradation is detected. Unstable RF signals can lead to frequent antenna switching, resulting in the overall RF performance not meeting expectations.

Recommended Scenarios for Using Multiple Antennas
++++++++++++++++++++++++++++++++++++++++++++++++++

1. Applications can either select specified antennas or implement their own antenna selection algorithms based on collected information, such as selecting antenna modes according to application-specific criteria. Refer to the IDF example :idf_file:`examples/phy/antenna/README.md` for designing antenna selection algorithms.

2. Configure antenna modes for both receiving and sending data as ESP_PHY_ANT_MODE_ANT0 or ESP_PHY_ANT_MODE_ANT1.
