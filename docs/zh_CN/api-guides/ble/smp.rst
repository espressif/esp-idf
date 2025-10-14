SMP
=======

:link_to_translation:`en:[English]`

在低功耗蓝牙中，安全管理协议（Security Manager Protocol, SMP）作为 GAP 模块的一部分实现。SMP 用于生成加密密钥和身份密钥，定义设备配对和密钥分发协议，并确保协议栈中的设备可以安全地进行通信。此过程要求在数据链路层建立连接，并符合特定的安全标准。

GAP SMP 允许两台设备通过设置适当的安全级别在数据链路层加密通信，这些安全级别的详细信息可参考《蓝牙核心规范》的 SMP 章节。在介绍低功耗蓝牙安全管理模块的具体实现过程之前，需要先明确以下几个概念：

- **配对 (Pairing):** 两个设备同意双方建立一定级别的安全性连接。

- **绑定 (Bonding):** 至少有一台设备向另一台设备发送安全相关信息（例如长期密钥（LTK）、连接签名解析密钥（CSRK）或身份解析密钥（IRK）），以供将来连接时使用。成功配对后，若支持绑定，则会进行密钥分发；否则，不会交换绑定信息。配对可能发生而不一定需要绑定。在配对过程中，设备会交换属性，以确定对方是否支持绑定。如果双方都不支持绑定，则不会存储对方的安全信息。

- **认证 (Authentication):** 此过程验证连接设备的身份，确保其不是第三方攻击者。不认证(Unauthentication) 并不意味着链路不安全。当两台设备确认链路加密密钥具有安全属性时，两个设备之间的关系称为认证。

  在配对过程中，短期密钥 (STK) 认证会生成一个密钥词。对于具有输入/输出和带外 (OOB) 功能的设备，所有生成和交换的密钥都具备认证 (MITM) 保护属性。在这种情况下，更长的 PIN 码或 OOB 密钥能够增强安全性。相反，如果使用 Just Works 方法，生成和交换的密钥则不具备 MITM 保护。

- **授权 (Authorization):** 指应用层授予执行某些操作的权限。一些应用可能需要额外的授权，只有在用户明确授权后才能继续操作。如果未获得授权，相关操作将失败。


低功耗蓝牙加密
-------------------

低功耗蓝牙的加密可通过以下两种方式实现：

- **初始加密（未绑定）：** 当两个低功耗蓝牙设备之间未进行过绑定时，加密将在配对过程中进行。是否绑定取决于配对配置。
- **重连接加密（已绑定）：** 如果两个低功耗蓝牙设备已经绑定，则可使用之前交换的密钥进行加密。在这种情况下，其中一台设备会触发加密请求，并使用绑定过程中存储的密钥完成加密。

Just Works 模式下，Master 主动发起加密请求的流程图如下 （*点击图片放大*）：


.. seqdiag::
    :caption: Just Works 模式下的加密流程图
    :align: center
    :scale: 200%

    seqdiag just-works-encrypt {
        activation = none;
        edge_length = 160;
        span_height = 20;
        default_shape = roundedbox;
        default_fontsize = 12;

        "Input\n[Master]";
        "API\n[Master]";
        "SMP\n[Master]";
        "SMP\n[Slave]";
        "API\n[Slave]";
        "Output\n[Slave]";

        "Input\n[Master]" -> "API\n[Master]" [label="esp_ble_set_encryption"];
        "API\n[Master]" -> "SMP\n[Master]";
        "SMP\n[Master]" -> "SMP\n[Slave]" [label="smp_pair_req"];
        "SMP\n[Slave]" -> "API\n[Slave]";
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_sec_req_evt"];
        "API\n[Slave]" <- "Output\n[Slave]" [label="esp_ble_gap_security_rsp"];
        "SMP\n[Slave]" <- "API\n[Slave]";
        "SMP\n[Master]" <- "SMP\n[Slave]" [label="smp_pair_rsq"];
        "SMP\n[Master]" -> "SMP\n[Slave]" [label="[ENCRYPT]", color=blue];
        "SMP\n[Master]" <- "SMP\n[Slave]" [label="[ENCRYPT]", color=blue];
        "API\n[Master]" <- "SMP\n[Master]";
        "SMP\n[Slave]" -> "API\n[Slave]";
        "Input\n[Master]" <- "API\n[Master]" [label="esp_gap_ble_auth_cmpl_evt"];
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_auth_cmpl_evt"];
    }


Passkey Entry 模式下，Master 主动发起加密请求的流程图如下 （*点击图片放大*）：


.. seqdiag::
    :caption: Passkey Entry 模式下的加密流程图
    :align: center
    :scale: 200%

    seqdiag passkey-entry-encrypt {
        activation = none;
        edge_length = 160;
        span_height = 20;
        default_shape = roundedbox;
        default_fontsize = 12;

        "Input\n[Master]";
        "API\n[Master]";
        "SMP\n[Master]";
        "SMP\n[Slave]";
        "API\n[Slave]";
        "Output\n[Slave]";

        "Input\n[Master]" -> "API\n[Master]" [label="esp_ble_set_encryption"];
        "API\n[Master]" -> "SMP\n[Master]";
        "SMP\n[Master]" -> "SMP\n[Slave]" [label="smp_pair_req"];
        "SMP\n[Slave]" -> "API\n[Slave]";
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_sec_req_evt"];
        "API\n[Slave]" <- "Output\n[Slave]" [label="esp_ble_gap_security_rsp"];
        "SMP\n[Slave]" <- "API\n[Slave]";
        "SMP\n[Master]" <- "SMP\n[Slave]" [label="smp_pair_rsq"];
        "Input\n[Master]" <- "API\n[Master]" [label="esp_gap_ble_passkey_notify_evt displays the passkey to the user, the peer device should enter this key and send it to the SMP stack"];
        "API\n[Slave]" <- "Output\n[Slave]" [label="esp_ble_passkey_reply send the passkey to the SMP stack which is displayed in the peer device"];
        "SMP\n[Master]" -> "SMP\n[Slave]" [label="[ENCRYPT]", color=blue];
        "SMP\n[Master]" <- "SMP\n[Slave]" [label="[ENCRYPT]", color=blue];
        "API\n[Master]" <- "SMP\n[Master]";
        "SMP\n[Slave]" -> "API\n[Slave]";
        "Input\n[Master]" <- "API\n[Master]" [label="esp_gap_ble_auth_cmpl_evt"];
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_auth_cmpl_evt"];
    }


低功耗蓝牙绑定
-----------------

两个低功耗蓝牙设备的绑定通过调用 GAP API 实现。根据蓝牙核心规范，绑定的目的是使得经过 SMP 加密的两台低功耗蓝牙设备在重新连接时能够使用相同的密钥进行链路加密，从而简化重连过程。在配对过程中，设备交换加密密钥并将其存储，以便长期使用。

绑定状态图如下 （*点击图片放大*）：


.. seqdiag::
    :caption: 低功耗蓝牙设备的绑定流程图
    :align: center
    :scale: 200%

    seqdiag ble_bonding {
        activation = none;
        edge_length = 160;
        span_height = 20;
        default_shape = roundedbox;
        default_fontsize = 12;

        "Input\n[Master]";
        "API\n[Master]";
        "SMP\n[Master]";
        "LLC\n[Master]";
        "LLC\n[Slave]";
        "SMP\n[Slave]";
        "API\n[Slave]";
        "Output\n[Slave]";

        "Input\n[Master]" -> "API\n[Master]" [label="esp_ble_set_encryption"];
        "API\n[Master]" -> "SMP\n[Master]" [label="continues if bonding is completed with the peer device"];
        "SMP\n[Master]" -> "LLC\n[Master]" [label="llc_start_enc_cmd"];
        "LLC\n[Master]" -> "LLC\n[Slave]";
        "LLC\n[Slave]"  -> "SMP\n[Slave]" [label="llc_ltk_req_evt"];
        "LLC\n[Slave]"  <- "SMP\n[Slave]" [label="llc_ltk_req_reply_cmd"];
        "LLC\n[Master]" -> "LLC\n[Slave]" [label="[ENCRYPT]", color=blue];
        "LLC\n[Master]" <- "LLC\n[Slave]" [label="[ENCRYPT]", color=blue];
        "SMP\n[Master]" <- "LLC\n[Master]" [label="llc_enc_change_evt"];
        "LLC\n[Slave]"  -> "SMP\n[Slave]" [label="llc_enc_change_evt"];
        "API\n[Master]" <- "SMP\n[Master]";
        "SMP\n[Slave]" -> "API\n[Slave]";
        "Input\n[Master]" <- "API\n[Master]" [label="esp_gap_ble_auth_cmpl_evt"];
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_auth_cmpl_evt"];
    }


.. note::

   绑定必须由主设备在连接建立后发起。


安全管理实现机制
-----------------

低功耗蓝牙安全管理是采用调用低功耗蓝牙 GAP 的 API 相关的加密函数，注册低功耗蓝牙 GAP 回调，并通过事件返回获取当前的加密状态。
