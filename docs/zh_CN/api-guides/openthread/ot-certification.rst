Thread 认证
===========

:link_to_translation:`en:[English]`

乐鑫的 Thread 解决方案，包括所有支持 Thread 的 ESP32 系列 SoC 和 ESP Thread 边界路由器解决方案，均已通过 Thread Group 的 Thread 组件认证。

每个已认证组件均基于特定的 ESP-IDF 发布标签，可作为产品开发的基础。如果产品使用相同的 ESP-IDF 发布标签开发，且未修改 Thread 相关源代码或默认配置，则可能符合继承认证 (Certification by Inheritance) 的条件。

如果修改了 Thread 相关源代码或默认配置，请咨询 Thread Group，以确认是否仍适用继承认证，或是否需要由授权测试实验室 (ATL) 进行测试。

满足适用的继承条件后，产品可以向 Thread Group 申请继承认证，并获得独立的 Thread 证书。

已认证组件列表
--------------

参考下表选择与您的产品相匹配的乐鑫已认证组件。申请继承时，被引用的乐鑫组件称为父产品 (Parent)，您的产品称为子产品 (Child)。

CID 表示认证编号 (Certification ID)。

Thread 设备
~~~~~~~~~~~

.. list-table::
    :width: 100%
    :widths: 20 20 20 20 20
    :header-rows: 1

    * - 已认证组件
      - 芯片型号
      - CID
      - ESP-IDF 版本
      - OpenThread 版本
    * - ESP32-H2
      - ESP32-H2
      - 14A204
      - v5.3.2
      - v1.4.0
    * - ESP32-C6
      - ESP32-C6
      - 14A205
      - v5.1.5
      - v1.4.0
    * - ESP32-C5
      - ESP32-C5
      - 14A240
      - v5.3.3
      - v1.4.0
    * - |ESP32 C-Series|
      - ESP32-C5、ESP32-C6
      - 14A416
      - v5.5.4
      - v1.4.0
    * - |ESP32 H-Series|
      - ESP32-H2、ESP32-H4、ESP32-H21
      - 14A446
      - v5.5.4
      - v1.4.0


Thread 边界路由器
~~~~~~~~~~~~~~~~~

.. list-table::
    :width: 100%
    :widths: 20 20 20 20 20 20
    :header-rows: 1

    * - 已认证组件
      - Host 型号
      - RCP 型号
      - CID
      - ESP-IDF 版本
      - OpenThread 版本
    * - `Espressif TBR <https://documentation.espressif.com/Espressif%20Thread%20Border%20Router%20Thread%20V1.4%20Interoperability%20Certification.pdf>`__
      - ESP32-S3
      - ESP32-H2
      - 13A080
      - v5.3.3
      - v1.4.0
    * - |ESP32 C-Series TBR Assembly|
      - ESP32-C2、ESP32-C3、ESP32-C5、ESP32-C6、ESP32-C61
      - ESP32-H2、ESP32-H4、ESP32-H21、ESP32-C5、ESP32-C6、ESP32-S31
      - 14A468
      - v5.5.4
      - v1.4.0
    * - |ESP32 S-Series TBR Assembly|
      - ESP32-S3、ESP32-S31
      - ESP32-H2、ESP32-H4、ESP32-H21、ESP32-C5、ESP32-C6、ESP32-S31
      - 14A513
      - v5.5.4
      - v1.4.0

上述证书对应 ESP32 Host 与 ESP32 RCP（Radio Co-Processor，无线协处理器）组成的 Thread 边界路由器方案，请确认您的产品所使用的芯片是对应证书中的一款。如果上述证书均无法覆盖您的产品所使用的芯片，请联系乐鑫官方。

继承认证条件
------------

基于乐鑫已认证组件开发终端产品，通常属于终端产品继承组件认证的场景 (Type III)。这里的父产品是指上表中乐鑫已认证的方案，其对应证书为父证书。

申请前，您需要确认以下条件：

- 您的公司持有有效且支持相应认证业务的 Thread Group 会员资格。
- 产品同时使用父组件对应的芯片和 Thread 协议栈，并按继承要求保持已认证 Thread 实现不变。

申请流程
--------

1. **成为 Thread Group 会员**

   如果您的公司尚未加入 Thread Group，请通过 |join| 申请，并选择支持您认证场景的会员等级。已有会员的公司应确认会员状态和认证权限。会员权益请以 |membership| 为准。

2. **确认父证书与软件配置**

   对照前文“已认证组件列表”中的表格，根据产品类型及目标 Thread 设备角色选择适用的父证书。对于 Thread 设备，请核对芯片型号及所需 Thread 功能；对于边界路由器，还需核对 Host 与 RCP 组合。选取合适的父证书 CID，用于后续继承认证申请。

3. **准备申请材料**

   登录 |portal|，获取当前使用的继承认证申请表，并参考申请表中的说明填写必要内容。作为继承证明材料，请提供产品固件的编译日志 (build log)，日志中应显示所使用的 ESP-IDF 版本号。

4. **提交继承申请**

   通过会员门户提供的认证渠道提交继承认证申请，填写选定的父证书 CID，并提交要求的支持材料。按 Thread Group 的通知支付适用的认证管理费用。Thread Group 将审核申请，并在需要时要求补充或修正资料。申请受理后分配的产品 CID 仅用于标识该产品，不代表认证已经完成。

5. **获得产品证书**

   审核通过后，Thread Group 将向您的产品颁发认证证书，并提供适用的认证标志许可协议及标志资料。请核对证书中的产品名称、型号、软硬件版本和 Thread 功能信息。完成认证后，方可按 Thread Group 的品牌及许可要求宣称产品已获得 Thread 认证，并使用适用的认证标志。产品也可以选择在 Thread Group 网站公开列出。

常见问题
--------

使用乐鑫已认证芯片后，产品是否自动获得 Thread 认证？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

否。您需要以自己的产品信息提交申请，并取得该产品的证书。乐鑫的父证书 CID 用于申请中的继承引用，不能替代您的产品认证。

继承认证是否需要送样测试？
~~~~~~~~~~~~~~~~~~~~~~~~~~

符合继承条件并获 Thread Group 接受的申请，无需进行 ATL 认证测试，但仍需提交继承证明材料。不符合继承条件时，应与 Thread Group 确认测试认证流程。

使用的 ESP-IDF 版本与证书对应版本不同时应如何处理？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果您使用的 ESP-IDF 版本与乐鑫证书对应的版本不同，请根据以下情况处理：

- **版本较低**：建议将 ESP-IDF 升级至乐鑫证书对应的版本，再基于该证书申请继承认证。
- **版本较高**：请联系乐鑫官方。乐鑫可能会针对您使用的 ESP-IDF 版本认证更新后的组件，并申请取得对应的可继承证书；随后，您可以使用更新后的证书为您的产品申请继承认证。

参考资料
--------

- |certification|
- |portal|

.. |ESP32 C-Series| replace:: `ESP32 C-Series <https://documentation.espressif.com/ESP32-C%20series%20Thread%2014A416.0_2026_06_30.pdf>`__
.. |ESP32 H-Series| replace:: `ESP32 H-Series <https://documentation.espressif.com/ESP32-H%20series%20Thread%2014A446.0_2026_08_09.pdf>`__
.. |ESP32 C-Series TBR Assembly| replace:: `ESP32 C-Series TBR Assembly <https://documentation.espressif.com/ESP32-C%20series%20TBR%2014A468.0_2026_08_10.pdf>`__
.. |ESP32 S-Series TBR Assembly| replace:: `ESP32 S-Series TBR Assembly <https://documentation.espressif.com/esp32_s_series_host_esp32_rcp_thread_br_cert_inheritable_cid_14a513.pdf>`__
.. |certification| replace:: `Thread Group 认证指南 <https://threadgroup.org/What-is-Thread/Certification>`__
.. |portal| replace:: `Thread Group 会员门户 <https://portal.threadgroup.org/>`__
.. |join| replace:: `Thread Group 会员申请页面 <https://threadgroup.org/Becomemember>`__
.. |membership| replace:: `Thread Group 会员信息 <https://threadgroup.org/thread-group#Membershipbenefits>`__
