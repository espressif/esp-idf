Chip Revision
=============

Overview
--------

A new chip versioning logic was introduced in new chips. Chips have several eFuse version fields:

- Major wafer version (``WAFER_VERSION_MAJOR`` eFuse)
- Minor wafer version (``WAFER_VERSION_MINOR`` eFuse)
- Ignore maximal revision (``DISABLE_WAFER_VERSION_MAJOR`` eFuse)

The new versioning logic is being introduced to distinguish changes in chips as breaking changes and non-breaking changes. Chips with non-breaking changes can run the same software as the previous chip. The previous chip means that the major version is the same.

If the newly released chip does not have breaking changes, that means it can run the same software as the previous chip, then in that chip we keep the same major version and increment the minor version by 1. Otherwise, if there is a breaking change in the newly released chip, meaning it can not run the same software as the previous chip, then in that chip we increase the major version and set the minor version to 0.

The software supports a number of revisions, from the minimum to the maximum (the min/max configs are defined in Kconfig). If the software is unaware of a new chip (when the chip version is out of range), it will refuse to run on it unless the Ignore maximum revision restrictions bit is set. This bit removes the upper revision limit.

Minimum versions limits the software to only run on a chip revision that is high enough to support some features. Maximum version is the maximum version that is well-supported by current software. When chip version is above the maximum version, software will reject to boot, because it may not work on, or work with risk on the chip.

Adding the major and minor wafer revision make the versioning logic is branchable.

.. note::

    The previous versioning logic was based on a single eFuse version field (``WAFER_VERSION``). This approach makes it impossible to mark chips as breaking or non-breaking changes, and the versioning logic becomes linear.

Using the branched versioning scheme allows us to support more chips in the software without updating the software when a new released compatible chip is used. Thus, the software will be compatible with as many new chip revisions as possible. If the software is no longer compatible with a new chip with breaking changes, the software will abort.

Revisions
---------

.. include:: inc/revisions_{IDF_TARGET_NAME}.rst

Chip Revision ``vX.Y``, where:

- ``X`` means Major wafer version. If it is changed, it means that the current software version is not compatible with this released chip and the software must be updated to use this chip.
- ``Y`` means Minor wafer version. If it is changed that means the current software version is compatible with the released chip, and there is no need to update the software.

The ``vX.Y`` chip version format will be used further instead of the ECO number.

Representing Revision Requirement Of A Binary Image
---------------------------------------------------

The 2nd stage bootloader and the application binary images have the :cpp:type:`esp_image_header_t` header, which stores the revision numbers of the chip on which the software can be run. This header has 3 fields related to revisions:

- ``min_chip_rev`` - Minimal chip MAJOR revision required by image (but for ESP32-C3 it is MINOR revision). Its value is determined by :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN`.
- ``min_chip_rev_full`` - Minimal chip MINOR revision required by image in format: ``major * 100 + minor``. Its value is determined by :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN`.
- ``max_chip_rev_full`` - Maximal chip revision required by image in format: ``major * 100 + minor``. Its value is determined by ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``. It can not be changed by user. Only Espressif can change it when a new version will be supported in IDF.

Chip Revision APIs
------------------

These APIs helps to get chip revision from eFuses:

- :cpp:func:`efuse_hal_chip_revision`. It returns revision in the ``major * 100 + minor`` format.
- :cpp:func:`efuse_hal_get_major_chip_version`. It returns Major revision.
- :cpp:func:`efuse_hal_get_minor_chip_version`. It returns Minor revision.

The following Kconfig definitions (in ``major * 100 + minor`` format) that can help add the chip revision dependency to the code:

- ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN_FULL``
- ``CONFIG_ESP_REV_MIN_FULL``
- ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``
- ``CONFIG_ESP_REV_MAX_FULL``

Maximal And Minimal Revision Restrictions
-----------------------------------------

The order for checking the minimum and maximum revisions:

1. The 1st stage bootloader (ROM bootloader) does not check minimal and maximal revision fields from :cpp:type:`esp_image_header_t` before running the 2nd stage bootloader.

2. The 2nd stage bootloader checks at the initialization phase that bootloader itself can be launched on the chip of this revision. It extracts the minimum revision from the header of the bootloader image and checks against the chip revision from eFuses. If the chip revision is less than the minimum revision, the bootloader refuses to boot up and aborts. The maximum revision is not checked at this phase.

3. Then the 2nd stage bootloader checks the revision requirements of the application. It extracts the minimum and maximum revisions from the header of the application image and checks against the chip revision from eFuses. If the chip revision is less than the minimum revision or higher than the maximum revision, the bootloader refuses to boot up and aborts. However, if the Ignore maximal revision bit is set, the maximum revision constraint can be ignored. The ignore bit is set by the customer themself when there is confirmation that the software is able to work with this chip revision.

4. Further, at the OTA update stage, the running application checks if the new software matches the chip revision. It extracts the minimum and maximum revisions from the header of the new application image and checks against the chip revision from eFuses. It checks for revision matching in the same way that the bootloader does, so that the chip revision is between the min and max revisions (logic of ignoring max revision also applies).

Issues
------

1. If the 2nd stage bootloader is run on the chip revision < minimum revision shown in the image, a reboot occurs. The following message will be printed:

.. code-block:: none

    Image requires chip rev >= v3.0, but chip is v1.0

To resolve this issue:

- make sure the chip you are using is suitable for the software, or use a chip with the required minimum revision or higher.
- update the software with :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` to get it ``<=`` the revision of chip being used

2. If application does not match minimal and maximal chip revisions, a reboot occurs. The following message will be printed:

.. code-block:: none

    Image requires chip rev <= v2.99, but chip is v3.0

To resolve this issue, update the IDF to a newer version that supports the used chip (``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``). Another way to fix this is to set the ``Ignore maximal revision`` bit in eFuse or use a chip that is suitable for the software.

Backward Compatible With Bootloaders Built By Older ESP-IDF Versions
--------------------------------------------------------------------

.. only:: esp32 or esp32c3 or esp32s2 or esp32s3

    The old bootloaders (IDF < 5.0) do not know about Major and Minor wafer version eFuses. They use one single eFuse for this - wafer version.

.. only:: esp32

    The old bootloaders did not read the minor wafer version eFuse, the major version can be only <= 3. So it means that the old bootloader can detect correctly only chip version in range v0.0 - v3.0, where the minor version is always 0.

.. only:: esp32c2

    {IDF_TARGET_NAME} chip support was added in IDF 5.0. The bootloader is able to detect any chip versions in range v0.0 - v3.15.

.. only:: esp32c3

    {IDF_TARGET_NAME} chip support was added in IDF 4.3. The old bootloaders can not read all bits of the wafer version eFuse, it can read only the first 3 low bits. So it means that the old bootloader can not detect chip version correctly. Chips v0.0 - v0.8 will be detected correctly, but other chip versions will be recognized as a version from this range.

.. only:: esp32s2 or esp32s3

    {IDF_TARGET_NAME} chip support was added in IDF 4.2. {IDF_TARGET_NAME} chips have ``rev_min`` in :cpp:type:`esp_image_header_t` header = 0 because ``Minimum Supported ESP32-S2 Revision`` Kconfig option was not introduced, it means that the old bootloader does not check the chip revision. Any app can be loaded by such bootloader in range v0.0 - v3.15.

Please check the chip version using ``esptool chip_id`` command.

API Reference
-------------

.. include-build-file:: inc/efuse_hal.inc
