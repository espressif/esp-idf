存储
====

:link_to_translation:`en:[English]`

NVS 加密
--------

- 在集成 HMAC 外设 (``SOC_HMAC_SUPPORTED``) 的 SoC 上，启用 :doc:`../../../security/flash-encryption` 时将不再自动启用 :doc:`../../../api-reference/storage/nvs_encryption`。
- 因此需显式启用 NVS 加密，并按照需要选择基于 flash 加密或基于 HMAC 外设的方案。即使未启用 flash 加密，也可选择基于 HMAC 外设的方案 (:ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME`)。
- 启用 flash 加密后，未集成 HMAC 外设的 SoC 仍会自动启用 NVS 加密。
