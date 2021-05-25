***********************
芯片系列对比
***********************

:link_to_translation:`en: [English]`

下表对比了 ESP-IDF 各系列芯片的主要特性，如需了解更多信息，请参考 `相关文档`_ 中各系列芯片的技术规格书。

.. list-table:: 芯片系列对比
   :widths: 20 40 40 40
   :header-rows: 1

   * - 特性
     - ESP32 系列
     - ESP32-S2 系列
     - ESP32-C3 系列
   * - 发布时间
     - 2016
     - 2020
     - 2020
   * - 产品型号
     - 请参考 `ESP32 技术规格书 (PDF) <https://espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_
     - 请参考 `ESP32-S2 技术规格书 (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf>`_
     - 请参考 `ESP32-C3 技术规格书 (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_cn.pdf>`_
   * - 内核
     - 搭载低功耗 Xtensa® LX6 32 位双核处理器，处理速度总共高达 600 MIPS，其中 ESP32-U4WDH/ESP32-S0WD（单核）处理速度可达 200 MIPS，ESP32-D2WD 可达 400 MIPS。
     - 搭载低功耗 Xtensa® LX7 32 位单核处理器，处理速度高达 300 MIPS
     - 搭载 RISC-V 32 位单核处理器
   * - Wi-Fi 协议
     - 802.11 b/g/n、2.4 GHz
     - 802.11 b/g/n、2.4 GHz
     - 802.11 b/g/n、2.4 GHz
   * - Bluetooth®
     - Bluetooth v4.2 BR/EDR 和 Bluetooth Low Energy
     - ✖️
     - Bluetooth 5.0
   * - 主频
     - 240 MHz（ESP32-S0WD、ESP32-D2WD 和 ESP32-U4WDH 为 160 MHz）
     - 240 MHz
     - 160 MHz
   * - SRAM
     - 520 KB
     - 320 KB
     - 400 KB
   * - ROM
     - 448 KB 用于程序启动和内核功能调用
     - 128 KB 用于程序启动和内核功能调用
     - 384 KB 用于程序启动和内核功能调用
   * - 嵌入式 flash
     - 2 MB、4 MB 或无嵌入式 flash，不同型号有差异
     - 2 MB、4 MB 或无嵌入式 flash，不同型号有差异
     - 4 MB 或无嵌入式 flash，不同型号有差异
   * - 外部 flash
     - 最大支持 16 MB，一次最多可映射 11 MB + 248 KB
     - 最大支持 1 GB，一次最多可映射 11.5 MB
     - 最大支持 16 MB，一次最多可映射 8 MB
   * - 片外 RAM
     - 最大支持 8 MB，一次最多可映射 4 MB
     - 最大支持 1 GB，一次最多可映射 11.5 MB
     - ✖️
   * - Cache
     - ✔️ 2 路组相联
     - ✔️ 4 路组相联，独立的指令和数据 cache
     - ✔️ 8 路组相连，32 位数据/指令总线宽度
   * - **外设**
     - 
     -
     - 
   * - 模/数转换器 (ADC)
     - 两个 12 位 SAR ADC，多达 18 个通道
     - 两个 13 位 SAR ADC，多达 18 个通道
     - 两个 12 位 SAR ADC，最多支持 6 个通道
   * - 数/模转换器 (DAC)
     - 两个 8 位通道
     - 两个 8 位通道
     - ✖️ 
   * - 定时器
     - 4 个 64 位通用定时器，3 个看门狗定时器
     - 4 个 64 位通用定时器，3 个看门狗定时器
     - 2 个 54 位通用定时器，3 个看门狗定时器     
   * - 温度传感器
     - ✖️
     - 1
     - 1
   * - 触摸传感器
     - 10
     - 14
     - ✖️
   * - 霍尔传感器
     - 1
     - ✖️
     - ✖️
   * - 通用输入/输出接口 (GPIO)
     - 34
     - 43
     - 22
   * - 串行外设接口 (SPI)
     - 4
     - 4 个 SPI，比 ESP32 支持更多模式
     - 3
   * - LCD 接口
     - 1
     - 1
     - ✖️
   * - 通用异步收发器 (UART)
     - 3
     - 2 [#one]_
     - 2 [#one]_
   * - I2C 接口
     - 2
     - 2
     - 1
   * - I2S 接口
     - 2 个，可配置为 8/16/32/40/48 位的输入输出通道
     - 1 个，可配置为 8/16/24/32/48/64 位的输入输出通道
     - 1 个，可配置为 8/16/24/32 位的输入输出通道
   * - Camera 接口
     - 1
     - 1
     - ✖️
   * - DMA
     - UART、SPI、I2S、SDIO 从机、SD/MMC 主机、EMAC、BT 和 Wi-Fi 都有专用的 DMA 控制器 
     - UART、SPI、AES、SHA、I2S 和 ADC 控制器都有专用的 DMA 控制器
     - 通用 DMA 控制器，3 个接收通道和 3 个发送通道
   * - 红外遥控器 (RMT)
     - 支持 8 通道
     - 支持 4 通道 [#one]_，可配置为红外发射和接收
     - 支持 4 通道 [#two]_，双通道的红外发射和双通道的红外接收
   * - 脉冲计数器
     - 8 通道
     - 4 通道 [#one]_
     - ✖️
   * - LED PWM
     - 16 通道
     - 8 通道 [#one]_
     - 6 通道 [#two]_
   * - MCPWM
     - 2，提供六个 PWM 输出
     - ✖️ 
     - ✖️ 
   * - USB OTG
     - ✖️
     - 1
     - ✖️           
   * - TWAI® 控制器（兼容 ISO 11898-1 协议）
     - 1
     - 1
     - 1
   * - SD/SDIO/MMC 主机控制器
     - 1
     - ✖️
     - ✖️
   * - SDIO 从机控制器
     - 1
     - ✖️
     - ✖️
   * - 以太网 MAC 接口
     - 1
     - ✖️
     - ✖️
   * - 超低功耗协处理器 (ULP)
     - ULP FSM
     - PicoRV32 内核，8 KB SRAM，ULP FSM 支持更多指令
     - ✖️
   * - 辅助调试
     - ✖️
     - ✖️
     - 1
   * - **安全机制**
     - 
     -
     - 
   * - 安全启动
     - ✔️
     - ✔️ 比 ESP32 更快更安全
     - ✔️ 比 ESP32 更快更安全
   * - Flash 加密
     - ✔️
     - ✔️ 支持 PSRAM 加密，比 ESP32 更安全
     - ✔️ 比 ESP32 更安全
   * - OTP
     - 1024 位
     - 4096 位
     - 4096 位
   * - AES
     - ✔️ AES-128, AES-192, AES-256 (FIPS PUB 197)
     - ✔️ AES-128, AES-192, AES-256 (FIPS PUB 197)
     - ✔️ AES-128, AES-256 (FIPS PUB 197)
   * - HASH
     - SHA-1, SHA-256, SHA-384, SHA-512 (FIPS PUB 180-4)
     - SHA-1, SHA-224, SHA-256, SHA-384, SHA-512, SHA-512/224, SHA-512/256, SHA-512/t (FIPS PUB 180-4); 支持 DMA 
     - SHA-1, SHA-224, SHA-256 (FIPS PUB 180-4)
   * - RSA
     - 高达 4096 位
     - 高达 4096 位，与 ESP32 相比，改善了加速选项
     - 高达 3072 位
   * - 随机数生成器 (RNG)
     - ✔️
     - ✔️
     - ✔️
   * - HMAC
     - ✖️
     - ✔️
     - ✔️
   * - 数字签名
     - ✖️
     - ✔️
     - ✔️
   * - XTS
     - ✖️
     - ✔️ XTS-AES-128, XTS-AES-256
     - ✔️ XTS-AES-128
   * - **其它**
     - 
     -
     - 
   * - Deep-sleep 功耗（超低功耗传感器监测方式）
     - 100 μA（ADC 以 1% 占空比工作时）
     - 22 μA（触摸传感器以 1% 占空比工作时）
     - 无此模式
   * - 封装尺寸
     - QFN48 5*5、6*6，不同型号有差异
     - QFN56 7*7 
     - QFN32 5*5

.. note::

    .. [#one] 与 ESP32 相比，减小了芯片面积 

    .. [#two] 与 ESP32 和 ESP32-S2 相比，减小了芯片面积  

.. note::

    芯片大小 (die size)：ESP32-C3 < ESP32-S2 < ESP32


相关文档
=================

- `ESP32 技术规格书 (PDF) <https://espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_
- ESP32-PICO 技术规格书 (PDF)

    - `ESP32-PICO-D4 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_cn.pdf>`_
    - `ESP32-PICO-V3 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-v3_datasheet_cn.pdf>`_
    - `ESP32-PICO-V3-02 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-v3-02_datasheet_cn.pdf>`_

- `ESP32-S2 技术规格书 (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf>`_
- `ESP32-C3 技术规格书 (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_cn.pdf>`_
- `ESP 产品选型 <http://products.espressif.com:8000/#/>`_