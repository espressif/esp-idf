Chip Revision
=============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

{IDF_TARGET_NAME} may have different revisions. These revisions mainly fix some issues, and sometimes also bring new features to the chip. :ref:`versioning scheme` describes the versioning of these chip revisions, and the APIs to read the versions at runtime.

There are some considerations of compatibility among application, ESP-IDF version, and chip revisions:

- Applications may depend on some fixes/features provided by a chip revision.
- When using updated version of hardware, the hardware may be incompatible with earlier versions of ESP-IDF.

:ref:`revision_limitation` describes how the application can specify its chip revision requirements, and the way ESP-IDF checks the compatibility. After that, there is troubleshooting information for this mechanism.

.. _versioning scheme:

Versioning Scheme
-----------------

A chip's revision number is typically expressed as ``vX.Y``, where:

- ``X`` means a **Major** wafer version. If it is changed, it means that the current software version is not compatible with this released chip and the software must be updated to use this chip.

- ``Y`` means a **Minor** wafer version. If it is changed that means the current software version is compatible with the released chip, and there is no need to update the software.

If a newly released chip does not contain breaking changes, the chip can run the same software as the previous chip. As such, the new chip's revision number will only increment the minor version while keeping the major version the same (e.g., ``v1.1`` to ``v1.2``).

Conversely, if a newly released chip contains breaking changes, the chip **cannot** run the same software as the previous chip. As such, the new chip's revision number will increment the major version and set the minor version to 0 (e.g., ``v1.1`` to ``v2.0``).

This versioning scheme was selected to indicate the derivation relationship of chip revisions, and clearly distinguish changes in chips between breaking changes and non-breaking changes.

ESP-IDF is designed to execute seamlessly on future chip minor revisions with the same logic as the chip's nearest previous minor revision. Thus，users can directly port their compiled binaries to newer MINOR chip revisions without upgrading their ESP-IDF version and re-compile the whole project.

When a binary is executed on a chip revision of unexpected MAJOR revision, the software is also able to report issues according to the MAJOR revision. The major and minor versioning scheme also allows hardware changes to be branchable.

.. note::

    The current chip revision scheme using major and minor versions was introduced from ESP-IDF v5.0 onwards. Thus bootloaders built using earlier versions of ESP-IDF will still use the legacy chip revision scheme of wafer versions.

EFuse Bits for Chip Revisions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Chips have several eFuse version fields:

- Major wafer version (``WAFER_VERSION_MAJOR`` eFuse)
- Minor wafer version (``WAFER_VERSION_MINOR`` eFuse)
- Ignore maximum wafer revision (``DISABLE_WAFER_VERSION_MAJOR`` eFuse). See :ref:`revision_limitation` on how this is used.

.. note::

    The previous versioning logic was based on a single eFuse version field (``WAFER_VERSION``). This approach makes it impossible to mark chips as breaking or non-breaking changes, and the versioning logic becomes linear.

EFuse Bits for eFuse Block Revisions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

EFuse block has version fields:

- Major efuse block version (``BLK_VERSION_MAJOR`` eFuse)
- Minor efuse block version (``BLK_VERSION_MINOR`` eFuse)
- Ignore maximum efuse block revision (``DISABLE_BLK_VERSION_MAJOR`` eFuse). See :ref:`revision_limitation` on how this is used.

Chip Revision APIs
^^^^^^^^^^^^^^^^^^

These APIs helps to get chip revision from eFuses:

- :cpp:func:`efuse_hal_chip_revision`. It returns revision in the ``major * 100 + minor`` format.
- :cpp:func:`efuse_hal_get_major_chip_version`. It returns Major revision of wafer.
- :cpp:func:`efuse_hal_get_minor_chip_version`. It returns Minor revision of wafer.

The following Kconfig definitions (in ``major * 100 + minor`` format) that can help add the chip revision dependency to the code:

- ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN_FULL``
- ``CONFIG_ESP_REV_MIN_FULL``
- ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``
- ``CONFIG_ESP_REV_MAX_FULL``

EFuse Block Revision APIs
^^^^^^^^^^^^^^^^^^^^^^^^^

These APIs helps to get eFuse block revision from eFuses:

- :cpp:func:`efuse_hal_blk_version`. It returns revision in the ``major * 100 + minor`` format.
- :cpp:func:`efuse_ll_get_blk_version_major`. It returns Major revision of eFuse block.
- :cpp:func:`efuse_ll_get_blk_version_minor`. It returns Minor revision of eFuse block.

The following Kconfig definitions (in ``major * 100 + minor`` format) that can help add the eFuse block revision dependency to the code:

- ``CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL``
- ``CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL``

.. _revision_limitation:

Compatibility Checks of ESP-IDF
-------------------------------

When building an application that needs to support multiple revisions of a particular chip, the minimum and maximum chip revision numbers supported by the build are specified via Kconfig.

The minimum chip revision can be configured via the :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` option. Specifying the minimum chip revision will limit the software to only run on a chip revisions that are high enough to support some features or bugfixes.

The maximum chip revision cannot be configured and is automatically determined by the current ESP-IDF version being used. ESP-IDF will refuse to boot any chip revision exceeding the maximum chip revision. Given that it is impossible for a particular ESP-IDF version to foresee all future chip revisions, the maximum chip revision is usually set to ``maximum supported MAJOR version + 99``. The "Ignore Maximum Revision" eFuse can be set to bypass the maximum revision limitation. However, the software is not guaranteed to work if the maximum revision is ignored.

The eFuse block revision is similar to the chip revision, but it mainly affects the coefficients that are specified in the eFuse (e.g. ADC calibration coefficients).

Below is the information about troubleshooting when the chip revision fails the compatibility check. Then there are technical details of the checking and software behavior on earlier version of ESP-IDF.

Troubleshooting
^^^^^^^^^^^^^^^

1. If the 2nd stage bootloader is run on a chip revision smaller than minimum revision specified in the image (i.e., the application), a reboot occurs. The following message will be printed:

.. code-block:: none

    Image requires chip rev >= v3.0, but chip is v1.0

To resolve this issue,

- Use a chip with the required minimum revision or higher.
- Lower the :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` value and rebuild the image so that it is compatible with the chip revision being used.

2. If application does not match minimum and maximum chip revisions, a reboot occurs. The following message will be printed:

.. code-block:: none

    Image requires chip rev <= v2.99, but chip is v3.0

To resolve this issue, update ESP-IDF to a newer version that supports the chip's revision (``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``). Alternatively, set the ``Ignore maximal revision`` bit in eFuse or use a chip revision that is compatible with the current version of ESP-IDF.

Representing Revision Requirements of a Binary Image
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For the chip revision, the 2nd stage bootloader and the application binary images contain the :cpp:type:`esp_image_header_t` header, which stores information specifying the chip revisions that the image is permitted to run on. This header has 3 fields related to the chip revisions:

- ``min_chip_rev`` - Minimum chip MAJOR revision required by image (but for ESP32-C3 it is MINOR revision). Its value is determined by :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN`.
- ``min_chip_rev_full`` - Minimum chip MINOR revision required by image in format: ``major * 100 + minor``. Its value is determined by :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN`.
- ``max_chip_rev_full`` - Maximum chip revision required by image in format: ``major * 100 + minor``. Its value is determined by ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``. It can not be changed by user. Only Espressif can change it when a new version will be supported in ESP-IDF.

For the eFuse revision, the requirements are stored in :cpp:type:`esp_app_desc_t`, which is contained in the application binary image. We only check the application image because the eFuse block revision mostly affects the ADC calibration, which does not really matter in the bootloader. There are 2 fields related to eFuse block revisions:

- ``min_efuse_blk_rev_full`` - Minimum eFuse block MINOR revision required by image in format: ``major * 100 + minor``. Its value is determined by ``CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL``.
- ``max_efuse_blk_rev_full`` - Maximum eFuse block MINOR revision required by image in format: ``major * 100 + minor``. Its value is determined by ``CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL``. It reflects whether the current IDF version supports this efuse block format or not, and should not be changed by the user.

Maximum And Minimum Revision Restrictions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The order for checking the minimum and maximum revisions during application boot up is as follows:

1. The 1st stage bootloader (ROM bootloader) does not check minimum and maximum revision fields from :cpp:type:`esp_image_header_t` before running the 2nd stage bootloader.

2. The initialization phase of the  2nd stage bootloader checks that the 2nd stage bootloader itself can be launched on the chip of this revision. It extracts the minimum revision from the header of the bootloader image and checks against the chip revision from eFuses. If the chip revision is less than the minimum revision, the bootloader refuses to boot up and aborts. The maximum revision is not checked at this phase.

3. Then the 2nd stage bootloader checks the revision requirements of the application. It extracts the minimum and maximum revisions of the chip from the application image header, and the eFuse block from the segment header. Then the bootloader checks these versions against the chip and eFuse block revision from eFuses. If the these revisions are less than their minimum revision or higher than the maximum revision, the bootloader refuses to boot up and aborts. However, if the ignore maximum revision bit is set, the maximum revision constraint can be ignored. The ignore bits are set by the customer themselves when there is confirmation that the software is able to work with this chip revision or eFuse block revision.

4. Furthermore, at the OTA update stage, the running application checks if the new software matches the chip revision and eFuse block revision. It extracts the minimum and maximum chip revisions from the header of the new application image and the eFuse block constraints from the application description to check against the these revisions from eFuses. It checks for revisions matching in the same way that the bootloader does, so that the chip and eFuse block revisions are between their min and max revisions (logic of ignoring max revision also applies).

Backward Compatibility with Bootloaders Built by Older ESP-IDF Versions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32 or esp32c3 or esp32s2 or esp32s3

    The old bootloaders (ESP-IDF < v5.0) do not know about Major and Minor wafer version eFuses. They use one single eFuse for this - wafer version.

.. only:: esp32

    The old bootloaders did not read the minor wafer version eFuse, and the major version can be only lower than or equal to v3. This means that the old bootloader can detect correctly only chip version in range ``v0.0`` to ``v3.0``, where the minor version is always set to ``0``.

.. only:: esp32c2

    {IDF_TARGET_NAME} chip support was added in ESP-IDF v5.0. The bootloader is able to detect any chip versions in range ``v0.0`` to ``v3.15``.

.. only:: esp32c3

    {IDF_TARGET_NAME} chip support was added in ESP-IDF v4.3. The old bootloaders cannot read all bits of the wafer version eFuse, it can read only the first 3 least significant bits. This means that the old bootloader cannot detect chip version correctly. Chips ``v0.0`` to ``v0.8`` are detected correctly, but other chip versions will be recognized as a version from this range.

.. only:: esp32s2 or esp32s3

    {IDF_TARGET_NAME} chip support was added in ESP-IDF v4.2. {IDF_TARGET_NAME} chips have ``rev_min`` in :cpp:type:`esp_image_header_t` header set to ``0`` because ``Minimum Supported ESP32-S2 Revision`` Kconfig option was not introduced, which means that the old bootloader does not check the chip revision. Any app can be loaded by such bootloader in range ``v0.0`` to ``v3.15``.

Please check the chip version using ``esptool chip_id`` command.

References
----------

- `Compatibility Advisory for Chip Revision Numbering Scheme <https://www.espressif.com.cn/sites/default/files/advisory_downloads/AR2022-005%20Compatibility%20Advisory%20for%20Chip%20Revision%20Numbering%20%20Scheme.pdf>`_
- `Compatibility Between ESP-IDF Releases and Revisions of Espressif SoCs <https://github.com/espressif/esp-idf/blob/master/COMPATIBILITY.md>`_
- `SoC Errata <https://www.espressif.com.cn/en/support/documents/technical-documents?keys=errata>`_
- :doc:`/versions`

API Reference
-------------

.. include-build-file:: inc/efuse_hal.inc
