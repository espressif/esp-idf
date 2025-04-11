数据交换
===================

:link_to_translation:`en:[English]`

本文档为低功耗蓝牙 (Bluetooth LE) 入门教程其四，旨在对低功耗蓝牙连接中的数据交换过程进行简要介绍。随后，本教程会结合 :example:`NimBLE_GATT_Server <bluetooth/ble_get_started/nimble/NimBLE_GATT_Server>` 例程，基于 NimBLE 主机层协议栈，对 GATT 服务器的代码实现进行介绍。


学习目标
---------------------------

- 学习特征数据和服务的数据结构细节
- 学习 GATT 的不同数据访问操作
- 学习 :example:`NimBLE_GATT_Server <bluetooth/ble_get_started/nimble/NimBLE_GATT_Server>` 例程的代码结构


GATT 数据特征与服务
----------------------------------

GATT 服务是低功耗蓝牙连接中两个设备进行数据交换的基础设施，其最小数据单元是属性。在 :ref:`数据表示与交换 <gatt_att_introduction>` 中，我们对 ATT 层的属性以及 GATT 层的特征数据、服务与规范进行了简要介绍。下面我们对基于属性的数据结构细节进行说明。


属性
^^^^^^^^^^^^^^^^^^^^^^^^^^^

属性由以下四部分组成

.. list-table::
    :align: center
    :widths: 10 30 60
    :header-rows: 1

    *   -   序号
        -   名称
        -   说明
    *   -   1
        -   句柄 (Handle)
        -   16 位无符号整型，表示属性在 :ref:`属性表 <attribute_table>` 中的索引
    *   -   2
        -   类型 (Type)
        -   ATT 属性使用 UUID (Universally Unique ID) 对类型进行区分
    *   -   3
        -   访问权限
        -   是否需要加密/授权？可读或可写？
    *   -   4
        -   值
        -   实际用户数据或另一属性的元数据

低功耗蓝牙中存在两种类型的 UUID ，如下

1. SIG 定义的 16 位 UUID
2. 厂商自定义的 128 位 UUID

在 SIG 官方提供的 `Assigned Numbers <https://www.bluetooth.com/specifications/assigned-numbers/>`_ 标准文件中，给出了一些常用特征数据和服务的 UUID ，例如

.. list-table::
    :align: center
    :widths: 20 60 20
    :header-rows: 1

    *   -   分类
        -   类型名称
        -   UUID
    *   -   服务
        -   血压服务 (Blood Pressure Service)
        -   `0x1810`
    *   -   服务
        -   通用音频服务 (Common Audio Service)
        -   `0x1853`
    *   -   特征数据
        -   年龄 (Age)
        -   `0x2A80`
    *   -   特征数据
        -   外观 (Appearance)
        -   `0x2A01`

事实上，这些服务和特征数据的定义也由 SIG 一并给出。例如心率测量值 (Heart Rate Measurement) 的值中必须含有标志位、心率测量值场，可以含有能量拓展场、 RR-间隔场以及传输间隔场等。所以，使用 SIG 定义的 UUID 使得不同厂商的低功耗蓝牙设备之间可以识别对方的服务或特征数据，实现跨厂商的低功耗蓝牙设备通信。

厂商自定义的 128 位 UUID 则用于满足厂商开发私有服务或数据特征的需求，例如本例程中 LED 特征数据的 UUID 为 `0x00001525-1212-EFDE-1523-785FEABCD123`，是一个厂商自定义的 128 位 UUID 。


特征数据
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _characteristic_attributes:

一个特征数据常由以下几个属性组成

.. list-table::
    :align: center
    :widths: 10 30 30 30
    :header-rows: 1

    *   -   序号
        -   名称
        -   作用
        -   备注
    *   -   1
        -   特征数据声明 (Characteristic Declaration)
        -   含有特征数据值的读写属性 (Properties)、句柄以及 UUID 信息
        -   UUID 为 `0x2803`，只读属性
    *   -   2
        -   特征数据值 (Characteristic Value)
        -   实际的用户数据
        -   UUID 标识特征数据的类型
    *   -   3
        -   特征数据描述符 (Characteristic Descriptor)
        -   特征数据的其他描述信息
        -   可选属性


特征数据声明和特征数据值之间的关系
##################################################################


下面以心率测量值 (Heart Rate Measurement) 为例，说明特征数据声明和特征数据值之间的关系。

下表为一属性表，含心率测量值数据特征的两个属性。首先来看句柄为 0 的属性，其 UUID 为 `0x2803`，访问权限为只读，说明这是一个特征数据声明属性。属性值中，读写属性为只读，句柄指向 1 ，说明句柄为 1 的属性为该特征数据的值属性； UUID 为 `0x2A37`，说明这个特征数据类型为心率测量值。

接下来看句柄为 1 的属性，其 `UUID` 为 `0x2A37`，访问权限为只读，与特征数据声明属性的值一一对应。该属性的值由标志位和测量值两部分组成，符合 SIG 规范对心率测量值特征数据的定义。

+-------------+--------------+-----------------+-------------------------+----------------------------+
| Handle      | UUID         | Permissions     | Value                   | Attribute Type             |
+=============+==============+=================+=========================+============================+
| 0           | `0x2803`     | Read-only       | Properties = Read-only  | Characteristic Declaration |
|             |              |                 +-------------------------+                            |
|             |              |                 | Handle = 1              |                            |
|             |              |                 +-------------------------+                            |
|             |              |                 | UUID = `0x2A37`         |                            |
+-------------+--------------+-----------------+-------------------------+----------------------------+
| 1           | `0x2A37`     | Read-only       | Flags                   | Characteristic Value       |
|             |              |                 +-------------------------+                            |
|             |              |                 | Measurement value       |                            |
+-------------+--------------+-----------------+-------------------------+----------------------------+


特征数据描述符
##################################################################

特征数据描述符起到对特征数据进行补充说明的作用。最常见的特征数据描述符是客户端特征数据配置描述符 (Client Characteristic Configuration Descriptor, CCCD)，下由 CCCD 代指。当特征数据支持由服务器端发起的 :ref:`数据操作 <gatt_data_operation>` （通知或指示）时，必须使用 CCCD 描述相关信息；这是一个可读写属性，用于 GATT 客户端告知服务器是否需要启用通知或指示，写值操作也被称为订阅 (Subscribe) 或取消订阅。

CCCD 的 UUID 是 `0x2902`，属性值中仅含 2 比特信息。第一个比特用于表示通知是否启用，第二个比特用于表示指示是否启用。我们将 CCCD 也添加到属性表中，并为心率测量值特征数据添加指示 (Indicate) 访问权限，就可以得到完整的心率测量值特征数据在属性表中的形态，如下

+-------------+--------------+-----------------+-----------------------------+----------------------------+
| Handle      | UUID         | Permissions     | Value                       | Attribute Type             |
+=============+==============+=================+=============================+============================+
| 0           | `0x2803`     | Read-only       | Properties = Read/Indicate  | Characteristic Declaration |
|             |              |                 +-----------------------------+                            |
|             |              |                 | Handle = 1                  |                            |
|             |              |                 +-----------------------------+                            |
|             |              |                 | UUID = `0x2A37`             |                            |
+-------------+--------------+-----------------+-----------------------------+----------------------------+
| 1           | `0x2A37`     | Read/Indicate   | Flags                       | Characteristic Value       |
|             |              |                 +-----------------------------+                            |
|             |              |                 | Measurement value           |                            |
+-------------+--------------+-----------------+-----------------------------+----------------------------+
| 2           | `0x2902`     | Read/Write      | Notification status         | Characteristic Descriptor  |
|             |              |                 +-----------------------------+                            |
|             |              |                 | Indication status           |                            |
+-------------+--------------+-----------------+-----------------------------+----------------------------+


服务
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

服务的数据结构大致可以分为两部分

.. list-table::
    :align: center
    :widths: 20 80
    :header-rows: 1

    *   -   序号
        -   名称
    *   -   1
        -   服务声明属性 (Service Declaration Attribute)
    *   -   2
        -   特征数据定义属性 (Characteristic Definition Attributes)

在 :ref:`特征数据 <characteristic_attributes>` 中提到的三种特征数据属性都属于特征数据定义属性。也就是说，服务的数据结构在本质上就是一些特征数据属性加上一个服务声明属性。

服务声明属性的 UUID 为 `0x2800`，访问权限为只读，值为标识服务类型的 UUID ，例如 Heart Rate Service 的 UUID 为 `0x180D`，那么其服务声明属性就可以表示为

.. list-table::
    :align: center
    :widths: 10 20 20 20 30
    :header-rows: 1

    *   -   Handle
        -   UUID
        -   Permissions
        -   Value
        -   Attribute Type
    *   -   0
        -   `0x2800`
        -   Read-only
        -   `0x180D`
        -   Service Declaration


属性表示例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _attribute_table:

下面以 :example:`NimBLE_GATT_Server <bluetooth/ble_get_started/nimble/NimBLE_GATT_Server>` 为例，展示一个 GATT 服务器可能的属性表形态。例程中含有两个服务，分别是 Heart Rate Service 和 Automation IO Service ；前者含有一个 Heart Rate Measurement 特征数据，后者含有一个 LED 特征数据。整个 GATT 服务器有属性表如下

+-------------+------------------------------------------+-----------------+-------------------------------------------------+----------------------------+
| Handle      | UUID                                     | Permissions     | Value                                           | Attribute Type             |
+=============+==========================================+=================+=================================================+============================+
| 0           | `0x2800`                                 | Read-only       | UUID = `0x180D`                                 | Service Declaration        |
+-------------+------------------------------------------+-----------------+-------------------------------------------------+----------------------------+
| 1           | `0x2803`                                 | Read-only       | Properties = Read/Indicate                      | Characteristic Declaration |
|             |                                          |                 +-------------------------------------------------+                            |
|             |                                          |                 | Handle = 2                                      |                            |
|             |                                          |                 +-------------------------------------------------+                            |
|             |                                          |                 | UUID = `0x2A37`                                 |                            |
+-------------+------------------------------------------+-----------------+-------------------------------------------------+----------------------------+
| 2           | `0x2A37`                                 | Read/Indicate   | Flags                                           | Characteristic Value       |
|             |                                          |                 +-------------------------------------------------+                            |
|             |                                          |                 | Measurement value                               |                            |
+-------------+------------------------------------------+-----------------+-------------------------------------------------+----------------------------+
| 3           | `0x2902`                                 | Read/Write      | Notification status                             | Characteristic Descriptor  |
|             |                                          |                 +-------------------------------------------------+                            |
|             |                                          |                 | Indication status                               |                            |
+-------------+------------------------------------------+-----------------+-------------------------------------------------+----------------------------+
| 4           | `0x2800`                                 | Read-only       | UUID = `0x1815`                                 | Service Declaration        |
+-------------+------------------------------------------+-----------------+-------------------------------------------------+----------------------------+
| 5           | `0x2803`                                 | Read-only       | Properties = Write-only                         | Characteristic Declaration |
|             |                                          |                 +-------------------------------------------------+                            |
|             |                                          |                 | Handle = 6                                      |                            |
|             |                                          |                 +-------------------------------------------------+                            |
|             |                                          |                 | UUID = `0x00001525-1212-EFDE-1523-785FEABCD123` |                            |
+-------------+------------------------------------------+-----------------+-------------------------------------------------+----------------------------+
| 6           | `0x00001525-1212-EFDE-`                  |Write-only       | LED status                                      |Characteristic Value        |
|             | `1523-785FE`                             |                 |                                                 |                            |
|             | `ABCD123`                                |                 |                                                 |                            |
+-------------+------------------------------------------+-----------------+-------------------------------------------------+----------------------------+

GATT 客户端在与 GATT 服务器初次建立通信时，会从 GATT 服务器拉取属性表中的元信息，从而获取 GATT 服务器上可用的服务以及数据特征。这一过程被称为 *服务发现 (Service Discovery)*。


GATT 数据操作
-----------------------------------------

.. _gatt_data_operation:

数据操作指的是对 GATT 服务器上的特征数据进行访问的操作，主要可以分为以下两类：

1. 由客户端发起的操作
2. 由服务器发起的操作


由客户端发起的操作
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

由客户端发起的操作有以下三种

- **读 (Read)**
    - 从 GATT 服务器上拉取某一特征数据的当前值。
- **写 (Write)**
    - 普通的写操作要求 GATT 服务器在收到客户端的写请求以及对应数据以后，进行确认响应。
- **写（无需响应） (Write without response)**
    - 快速写操作则不需要服务器进行确认响应。


由服务器发起的操作
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

由服务器发起的操作分两种

- **通知 (Notify)**
    - 通知是 GATT 服务器主动向客户端推送数据的操作，不需要客户端回复确认响应。
- **指示 (Indicate)**
    - 与通知相似，区别在于指示需要客户端回复确认，因此数据推送速度比通知慢。

虽然通知和指示都是由服务器发起的操作，但是服务器发起操作的前提是，客户端启用了通知或指示。所以，本质上 GATT 的数据交换过程总是以客户端请求数据开始。


例程实践
-------------------------------------------

在掌握了 GATT 数据交换的相关知识以后，接下来让我们结合 :example:`NimBLE_GATT_Server <bluetooth/ble_get_started/nimble/NimBLE_GATT_Server>` 例程代码，学习如何使用 NimBLE 协议栈构建一个简单的 GATT 服务器，对学到的知识进行实践。


前提条件
^^^^^^^^^^^^^^^

1. 一块 {IDF_TARGET_NAME} 开发板
2. ESP-IDF 开发环境
3. 在手机上安装 nRF Connect for Mobile 应用程序

若你尚未完成 ESP-IDF 开发环境的配置，请参考 :doc:`IDF 快速入门 <../../../get-started/index>`。


动手试试
^^^^^^^^^^^^^^^^^^

请参考 :ref:`BLE 介绍 动手试试 <nimble_gatt_server_practice>` 。


代码详解
---------------------------------------------------


工程结构综述
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:example:`NimBLE_GATT_Server <bluetooth/ble_get_started/nimble/NimBLE_GATT_Server>` 的根目录结构与 :ref:`NimBLE_Connection <nimble_connection_project_structure>` 完全一致。另外，在 `main` 文件夹中引入了与 GATT 服务以及模拟心率生成相关的源代码。


程序行为综述
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

本例程的程序行为与 :ref:`NimBLE_Connection <nimble_connection_project_structure>` 的程序行为基本一致，区别在于本例程添加了 GATT 服务，通过对应的回调函数对 GATT 数据特征的访问进行处理。


入口函数
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _nimble_gatt_server_entry_point:

在 :ref:`NimBLE_Connection <nimble_connection_entry_point>` 的基础上，新增了调用 `gatt_svc_init` 函数对 GATT 服务进行初始化的过程。另外，除了 NimBLE 线程以外，本例新增了 `heart_rate_task` 线程，负责心率测量模拟数据的随机生成以及指示处理，相关代码如下

.. code-block:: C

    static void heart_rate_task(void *param) {
        /* Task entry log */
        ESP_LOGI(TAG, "heart rate task has been started!");

        /* Loop forever */
        while (1) {
            /* Update heart rate value every 1 second */
            update_heart_rate();
            ESP_LOGI(TAG, "heart rate updated to %d", get_heart_rate());

            /* Send heart rate indication if enabled */
            send_heart_rate_indication();

            /* Sleep */
            vTaskDelay(HEART_RATE_TASK_PERIOD);
        }

        /* Clean up at exit */
        vTaskDelete(NULL);
    }

    void app_main(void) {
        ...

        xTaskCreate(heart_rate_task, "Heart Rate", 4*1024, NULL, 5, NULL);
        return;
    }

`heart_rate_task` 线程以 1 Hz 的频率运行，因为 `HEART_RATE_TASK_PERIOD` 被定义为 1000 ms 。每次执行时，线程都会调用 `update_heart_rate` 函数随机生成一个新的心率测量模拟数据，并调用 `send_heart_rate_indication` 处理指示操作。


GATT 服务初始化
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在 `gatt_svc.c` 文件中，有 GATT 服务初始化函数如下

.. code-block:: C

    int gatt_svc_init(void) {
        /* Local variables */
        int rc;

        /* 1. GATT service initialization */
        ble_svc_gatt_init();

        /* 2. Update GATT services counter */
        rc = ble_gatts_count_cfg(gatt_svr_svcs);
        if (rc != 0) {
            return rc;
        }

        /* 3. Add GATT services */
        rc = ble_gatts_add_svcs(gatt_svr_svcs);
        if (rc != 0) {
            return rc;
        }

        return 0;
    }

该函数先调用 `ble_svc_gatt_init` API ，对 GATT Service 进行初始化。需要注意，这里的 GATT Service 是一个特殊的 GATT 服务，服务的 UUID 为 `0x1801` ，用于 GATT 服务器在服务发生变更时（添加或删除 GATT 服务）通知客户端，此时客户端会重新执行服务发现流程，以更新服务信息。

接下来，通过调用 `ble_gatts_count_cfg` 和 `ble_gatts_add_svcs` API ，将 `gatt_svr_svcs` 服务表中的服务以及特征数据添加到 GATT 服务器。


GATT 服务表
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

`gatt_svr_svcs` 服务表是本例程中非常关键的数据结构，定义了本例程的所有服务与特征数据，相关代码如下

.. code-block:: C

    /* Heart rate service */
    static const ble_uuid16_t heart_rate_svc_uuid = BLE_UUID16_INIT(0x180D);

    ...

    static uint16_t heart_rate_chr_val_handle;
    static const ble_uuid16_t heart_rate_chr_uuid = BLE_UUID16_INIT(0x2A37);

    static uint16_t heart_rate_chr_conn_handle = 0;

    ...

    /* Automation IO service */
    static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);
    static uint16_t led_chr_val_handle;
    static const ble_uuid128_t led_chr_uuid =
        BLE_UUID128_INIT(0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 0xde, 0xef,
                        0x12, 0x12, 0x25, 0x15, 0x00, 0x00);

    /* GATT services table */
    static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
        /* Heart rate service */
        {.type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &heart_rate_svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {/* Heart rate characteristic */
                .uuid = &heart_rate_chr_uuid.u,
                .access_cb = heart_rate_chr_access,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_INDICATE,
                .val_handle = &heart_rate_chr_val_handle},
                {
                    0, /* No more characteristics in this service. */
                }}},

        /* Automation IO service */
        {
            .type = BLE_GATT_SVC_TYPE_PRIMARY,
            .uuid = &auto_io_svc_uuid.u,
            .characteristics =
                (struct ble_gatt_chr_def[]){/* LED characteristic */
                                            {.uuid = &led_chr_uuid.u,
                                            .access_cb = led_chr_access,
                                            .flags = BLE_GATT_CHR_F_WRITE,
                                            .val_handle = &led_chr_val_handle},
                                            {0}},
        },

        {
            0, /* No more services. */
        },
    };

`BLE_UUID16_INIT` 和 `BLE_UUID128_INIT` 是 NimBLE 协议栈提供的宏，可以便捷地将 16 或 128 位 UUID 由原始数据转换为 `ble_uuid16_t` 和 `ble_uuid128_t` 类型变量。

`gatt_svr_svcs` 是一个 `ble_gatt_svc_def` 类型的结构体数组。 `ble_gatt_svc_def` 即定义服务的结构体，关键字段为 `type`、 `uuid` 以及 `characteristics`。 `type` 字段用于标识当前服务的主次类型，本例中均为主服务。 `uuid` 字段即服务的 UUID 。 `characteristics` 字段是 `ble_gatt_chr_def` 类型的结构体数组，用于存放对应服务下的特征数据。

`ble_gatt_chr_def` 即定义特征数据的结构体，关键字段为 `uuid`、 `access_cb`、 `flags` 以及 `val_handle`。 `uuid` 字段即特征数据的 UUID 。 `access_cb` 字段用于指向该特征数据的访问回调函数。 `flags` 字段用于标识特征数据的访问权限。 `val_handle` 字段用于指向该特征数据值的变量句柄地址。

需要说明的是，当为特征数据设定了 `BLE_GATT_CHR_F_INDICATE` 标志位时， NimBLE 协议栈会自动为该特征数据添加 CCCD ，所以我们无需手动添加描述符。

结合变量命名，不难发现， `gatt_svr_svcs` 实现了 :ref:`属性表 <attribute_table>` 中的所有属性定义。另外，对于 Heart Rate Measurement 特征数据，其访问通过 `heart_rate_chr_access` 回调函数管理；对于 LED 特征数据，其访问通过 `led_chr_access` 回调函数管理。


特征数据访问管理
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


LED 访问管理
#################################################

LED 特征数据的访问通过 `led_chr_access` 回调函数管理，相关代码如下

.. code-block:: C

    static int led_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg) {
        /* Local variables */
        int rc;

        /* Handle access events */
        /* Note: LED characteristic is write only */
        switch (ctxt->op) {

        /* Write characteristic event */
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            /* Verify connection handle */
            if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
                ESP_LOGI(TAG, "characteristic write; conn_handle=%d attr_handle=%d",
                        conn_handle, attr_handle);
            } else {
                ESP_LOGI(TAG,
                        "characteristic write by nimble stack; attr_handle=%d",
                        attr_handle);
            }

            /* Verify attribute handle */
            if (attr_handle == led_chr_val_handle) {
                /* Verify access buffer length */
                if (ctxt->om->om_len == 1) {
                    /* Turn the LED on or off according to the operation bit */
                    if (ctxt->om->om_data[0]) {
                        led_on();
                        ESP_LOGI(TAG, "led turned on!");
                    } else {
                        led_off();
                        ESP_LOGI(TAG, "led turned off!");
                    }
                } else {
                    goto error;
                }
                return rc;
            }
            goto error;

        /* Unknown event */
        default:
            goto error;
        }

    error:
        ESP_LOGE(TAG,
                "unexpected access operation to led characteristic, opcode: %d",
                ctxt->op);
        return BLE_ATT_ERR_UNLIKELY;
    }

当 GATT 客户端发起对 LED 特征数据的访问时， NimBLE 协议栈将会调用 `led_chr_access` 回调函数，并将句柄信息与访问上下文等信息传入。 `ble_gatt_access_ctxt` 的 `op` 字段用于标识不同的访问事件。由于 LED 是一个只写的特征数据，因此我们仅对 `BLE_GATT_ACCESS_OP_WRITE_CHR` 事件进行处理。

在这个处理分支中，我们先对属性句柄进行验证，确认客户端访问的是 LED 特征数据；随后根据 `ble_gatt_access_ctxt` 的 `om` 字段，验证访问数据的长度；最后根据 `om_data` 中的数据是否为 1 ，对 LED 进行点亮或熄灭操作。

若出现了其他访问事件，则认为是意料外的访问，直接走 `error` 分支返回。


心率测量值读访问管理
#################################################

心率测量值是可读且可指示的特征数据，其中客户端对心率测量值发起的读访问，由 `heart_rate_chr_access` 回调函数管理，相关代码如下

.. code-block:: C

    static int heart_rate_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt, void *arg) {
        /* Local variables */
        int rc;

        /* Handle access events */
        /* Note: Heart rate characteristic is read only */
        switch (ctxt->op) {

        /* Read characteristic event */
        case BLE_GATT_ACCESS_OP_READ_CHR:
            /* Verify connection handle */
            if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
                ESP_LOGI(TAG, "characteristic read; conn_handle=%d attr_handle=%d",
                        conn_handle, attr_handle);
            } else {
                ESP_LOGI(TAG, "characteristic read by nimble stack; attr_handle=%d",
                        attr_handle);
            }

            /* Verify attribute handle */
            if (attr_handle == heart_rate_chr_val_handle) {
                /* Update access buffer value */
                heart_rate_chr_val[1] = get_heart_rate();
                rc = os_mbuf_append(ctxt->om, &heart_rate_chr_val,
                                    sizeof(heart_rate_chr_val));
                return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            goto error;

        /* Unknown event */
        default:
            goto error;
        }

    error:
        ESP_LOGE(
            TAG,
            "unexpected access operation to heart rate characteristic, opcode: %d",
            ctxt->op);
        return BLE_ATT_ERR_UNLIKELY;
    }

和 LED 的访问管理类似的，我们通过 `ble_gatt_access_ctxt` 访问上下文的 `op` 字段判断访问事件，对 `BLE_GATT_ACCESS_OP_READ_CHR` 事件进行处理。

在处理分支中，我们同样先对属性句柄进行验证，确认客户端访问的是心率测量值属性；然后，调用 `get_heart_rate` 接口获取最新的心率测量值，并存到 `heart_rate_chr_val` 数组的测量值区域中；最后，将 `heart_rate_chr_val` 的数据复制到 `ble_gatt_access_ctxt` 访问上下文的 `om` 字段中， NimBLE 协议栈会在当前回调函数结束后，将该字段中的数据发送至客户端，从而实现了对 Heart Rate Measurement 特征数据值的读访问。


心率测量值指示
###############################################

当客户端启用心率测量值的指示时，处理流程相对麻烦一些。首先，客户端启用或禁用心率测量值的指示是 GAP 层的订阅或取消订阅事件，所以我们必须在 `gap_event_handler` 回调函数中增加对订阅事件的处理分支，如下

.. code-block:: C

    static int gap_event_handler(struct ble_gap_event *event, void *arg) {
        ...

        /* Subscribe event */
        case BLE_GAP_EVENT_SUBSCRIBE:
            /* Print subscription info to log */
            ESP_LOGI(TAG,
                    "subscribe event; conn_handle=%d attr_handle=%d "
                    "reason=%d prevn=%d curn=%d previ=%d curi=%d",
                    event->subscribe.conn_handle, event->subscribe.attr_handle,
                    event->subscribe.reason, event->subscribe.prev_notify,
                    event->subscribe.cur_notify, event->subscribe.prev_indicate,
                    event->subscribe.cur_indicate);

            /* GATT subscribe event callback */
            gatt_svr_subscribe_cb(event);
            return rc;
    }

订阅事件为 `BLE_GAP_EVENT_SUBSCRIBE` 。在这个处理分支中，我们不直接对订阅事件进行处理，而是调用 `gatt_svr_subscribe_cb` 回调函数处理订阅事件。这里体现了软件分层设计的思想，因为订阅事件影响的是 GATT 服务器对特征数据的发送行为，与 GAP 层无关，因此应直接将这个事件传递至 GATT 层进行处理。

下面，我们看一下 `gatt_svr_subscribe_cb` 回调函数中都进行哪些操作

.. code-block:: C

    void gatt_svr_subscribe_cb(struct ble_gap_event *event) {
        /* Check connection handle */
        if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d",
                    event->subscribe.conn_handle, event->subscribe.attr_handle);
        } else {
            ESP_LOGI(TAG, "subscribe by nimble stack; attr_handle=%d",
                    event->subscribe.attr_handle);
        }

        /* Check attribute handle */
        if (event->subscribe.attr_handle == heart_rate_chr_val_handle) {
            /* Update heart rate subscription status */
            heart_rate_chr_conn_handle = event->subscribe.conn_handle;
            heart_rate_chr_conn_handle_inited = true;
            heart_rate_ind_status = event->subscribe.cur_indicate;
        }
    }

本例中的回调处理非常简单：判断订阅事件中的属性句柄是否为心率测量值的属性句柄，若是，则保存对应的连接句柄，并更新客户端要求的指示状态。

在 :ref:`入口函数 <nimble_gatt_server_entry_point>` 中提到， `send_heart_rate_indication` 函数以 1 Hz 的频率被 `heart_rate_task` 线程调用。这个函数的实现如下

.. code-block:: C

    void send_heart_rate_indication(void) {
        if (heart_rate_ind_status && heart_rate_chr_conn_handle_inited) {
            ble_gatts_indicate(heart_rate_chr_conn_handle,
                            heart_rate_chr_val_handle);
            ESP_LOGI(TAG, "heart rate indication sent!");
        }
    }

`ble_gatts_indicate` 是 NimBLE 协议栈提供的指示发送 API 。也就是说，当心率测量值的指示状态为真，且对应连接句柄可用的情况下，调用 `send_heart_rate_indication` 函数就会发送一次心率测量值至 GATT 客户端。

简单总结一下，当 GATT 客户端订阅心率测量值时， `gap_event_handler` 将会接收到订阅事件，并将订阅事件传递至 `gatt_svr_subscribe_cb` 回调函数，随后更新心率测量值的订阅状态。在 `heart_rate_task` 线程中，每秒都会检查一次心率测量值的订阅状态，若订阅状态为真，则将心率测量值发送至客户端。

总结
----------------------------

通过本教程，你了解了如何通过服务表创建 GATT 服务以及相应的特征数据，并掌握了 GATT 特征数据的访问管理方式，包括读、写和订阅操作的实现。你可以在 :example:`NimBLE_GATT_Server <bluetooth/ble_get_started/nimble/NimBLE_GATT_Server>` 例程的基础上，开发更加复杂的 GATT 服务应用。
