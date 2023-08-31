Storage Security
================

:link_to_translation:`zh_CN:[中文]`

Overview of Available Resources
-------------------------------

Data privacy is achieved by using the :doc:`../../security/flash-encryption` feature. This mechanism is currently used by FATFS and LittleFS and is recommended for new storage type implementations based on the Partitions API.
NVS storage uses a proprietary :doc:`NVS encryption <nvs_encryption>` implementation.

Workflows focused on overall system security are described in the :doc:`Security Features Enablement Workflows <../../security/security-features-enablement-workflows>`.
Workflows related to the combination of multiple secured storage components in one project are presented in the :example:`Flash Encryption Example <security/flash_encryption>`.

.. list-table:: Relevant storage security examples
    :widths: 25 75
    :header-rows: 0

    * - **Link**
      - **Description**
    * - :example:`nvs_encryption_hmac <security/nvs_encryption_hmac>`
      - Demonstrates NVS encryption with an HMAC-based encryption key protection scheme.
    * - :example:`flash_encryption <security/flash_encryption>`
      - Provides a combined example showing the coexistence of NVS encryption, FATFS encryption, and encrypted custom data access via the Partitions API. Security related workflows for both development and production are also provided.
