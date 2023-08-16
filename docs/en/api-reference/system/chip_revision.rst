Chip Revision
=============

Overview
--------

{IDF_TARGET_NAME} may have different revisions. These revisions mainly fix some issues, sometimes also bring new features to the chip. :ref:`versioning scheme` describes the versioning of these chip revisions, and the APIs to read the versions at runtime.

There are some considerations of compatibility among application, ESP-IDF version, and chip revisions.

- Application may depends on some fixes/features provided by a chip revision.
- When using updated version of hardware, sometimes it's not compatible with earlier version of ESP-IDF.

:ref:`revision_limitation` describes how the application can specify its requirement and the way ESP-IDF checks the compatibility. After that, there is troubleshooting information for this mechanism.

.. _versioning scheme:

Versioning Scheme
-----------------

A new chip versioning logic was introduced in new chips. Chip Revision ``vX.Y``, where:

- ``X`` means Major wafer version. If it is changed, it means that the current software version is not compatible with this released chip and the software must be updated to use this chip.

- ``Y`` means Minor wafer version. If it is changed that means the current software version is compatible with the released chip, and there is no need to update the software.

The ``vX.Y`` chip version format is used further instead of the ECO number.

If the newly released chip does not have breaking changes, that means it can run the same software as the previous chip, then in that chip we keep the same major version and increment the minor version by 1. Otherwise, if there is a breaking change in the newly released chip, meaning it can not run the same software as the previous chip, then in that chip we increase the major version and set the minor version to 0.

The new versioning logic is being introduced to indicate the derivation relationship of chip revisions, and distinguish changes in chips as breaking changes and non-breaking changes.

Software is supposed to execute on a future chip revision with the same logic as the chip's nearest previous revision. This way user can directly port their compiled binaries to newer MINOR chip revisions without upgrading their ESP-IDF version and re-compile the whole project.

When a binary is executed on a chip revision of unexpected MAJOR revision, the software is also able to report issues according to the MAJOR revision.

The major and minor versioning scheme also allows hardware changes to be branchable.

EFuse Bits for Chip Revisions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Chips have several eFuse version fields:

- Major wafer version (``WAFER_VERSION_MAJOR`` eFuse)
- Minor wafer version (``WAFER_VERSION_MINOR`` eFuse)
- Ignore maximum revision (``DISABLE_WAFER_VERSION_MAJOR`` eFuse). See :ref:`revision_limitation` on how this is used.

.. note::

    The previous versioning logic was based on a single eFuse version field (``WAFER_VERSION``). This approach makes it impossible to mark chips as breaking or non-breaking changes, and the versioning logic becomes linear.


Chip Revision APIs
^^^^^^^^^^^^^^^^^^

These APIs helps to get chip revision from eFuses:

- :cpp:func:`efuse_hal_chip_revision`. It returns revision in the ``major * 100 + minor`` format.
- :cpp:func:`efuse_hal_get_major_chip_version`. It returns Major revision.
- :cpp:func:`efuse_hal_get_minor_chip_version`. It returns Minor revision.

The following Kconfig definitions (in ``major * 100 + minor`` format) that can help add the chip revision dependency to the code:

- ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN_FULL``
- ``CONFIG_ESP_REV_MIN_FULL``
- ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``
- ``CONFIG_ESP_REV_MAX_FULL``


.. _revision_limitation:

Compatibility Checks of ESP-IDF
-------------------------------

The application supports a number of chip revisions, from the minimum to the maximum (the min/max configs are defined in Kconfig).

Minimum version, selected by Kconfig option :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN`, limits the software to only run on a chip revision that is high enough to support some features or include some bugfixes.

Maximum version, determined by the version of ESP-IDF used, is the maximum chip revision that is well-supported by current ESP-IDF. It's usually ``maxinum supported MAJOR version + 99``. When chip revision is above the maximum version, it means that ESP-IDF is unaware of the new chip revision. In this case, software will reject to boot, unless the Ignore maximum revision restrictions bit is set (which bypasses the maximum revision limitation). However, the software may not work on, or work with risk on the chip.

Below is the information about troubleshooting when the chip revision fails the compatibility check. Then there are technical details of the checking and software behavior on earlier version of ESP-IDF.

Troubleshooting
^^^^^^^^^^^^^^^

1. If the 2nd stage bootloader is run on the chip revision < minimum revision shown in the image, a reboot occurs. The following message will be printed:

.. code-block:: none

    Image requires chip rev >= v3.0, but chip is v1.0

To resolve this issue:

- make sure the chip you are using is suitable for the software, or use a chip with the required minimum revision or higher.
- update the software with :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` to get it ``<=`` the revision of chip being used

2. If application does not match minimum and maximum chip revisions, a reboot occurs. The following message will be printed:

.. code-block:: none

    Image requires chip rev <= v2.99, but chip is v3.0

To resolve this issue, update the ESP-IDF to a newer version that supports the used chip (``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``). Another way to fix this is to set the ``Ignore maximal revision`` bit in eFuse or use a chip that is suitable for the software.

Representing Revision Requirement of a Binary Image
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The 2nd stage bootloader and the application binary images have the :cpp:type:`esp_image_header_t` header, which stores the revision numbers of the chip on which the software can be run. This header has 3 fields related to revisions:

- ``min_chip_rev`` - Minumum chip MAJOR revision required by image (but for ESP32-C3 it is MINOR revision). Its value is determined by :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN`.
- ``min_chip_rev_full`` - Minimum chip MINOR revision required by image in format: ``major * 100 + minor``. Its value is determined by :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN`.
- ``max_chip_rev_full`` - Maximum chip revision required by image in format: ``major * 100 + minor``. Its value is determined by ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``. It can not be changed by user. Only Espressif can change it when a new version will be supported in ESP-IDF.

Maximum And Minumum Revision Restrictions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The order for checking the minimum and maximum revisions:

1. The 1st stage bootloader (ROM bootloader) does not check minimum and maximum revision fields from :cpp:type:`esp_image_header_t` before running the 2nd stage bootloader.

2. The 2nd stage bootloader checks at the initialization phase that bootloader itself can be launched on the chip of this revision. It extracts the minimum revision from the header of the bootloader image and checks against the chip revision from eFuses. If the chip revision is less than the minimum revision, the bootloader refuses to boot up and aborts. The maximum revision is not checked at this phase.

3. Then the 2nd stage bootloader checks the revision requirements of the application. It extracts the minimum and maximum revisions from the header of the application image and checks against the chip revision from eFuses. If the chip revision is less than the minimum revision or higher than the maximum revision, the bootloader refuses to boot up and aborts. However, if the Ignore maximum revision bit is set, the maximum revision constraint can be ignored. The ignore bit is set by the customer themself when there is confirmation that the software is able to work with this chip revision.

4. Further, at the OTA update stage, the running application checks if the new software matches the chip revision. It extracts the minimum and maximum revisions from the header of the new application image and checks against the chip revision from eFuses. It checks for revision matching in the same way that the bootloader does, so that the chip revision is between the min and max revisions (logic of ignoring max revision also applies).

Backward Compatible With Bootloaders Built By Older ESP-IDF Versions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32 or esp32c3 or esp32s2 or esp32s3

    The old bootloaders (ESP-IDF < v5.0) do not know about Major and Minor wafer version eFuses. They use one single eFuse for this - wafer version.

.. only:: esp32

    The old bootloaders did not read the minor wafer version eFuse, the major version can be only <= v3. So it means that the old bootloader can detect correctly only chip version in range v0.0 - v3.0, where the minor version is always set to 0.

.. only:: esp32c2

    {IDF_TARGET_NAME} chip support was added in ESP-IDF v5.0. The bootloader is able to detect any chip versions in range v0.0 - v3.15.

.. only:: esp32c3

    {IDF_TARGET_NAME} chip support was added in ESP-IDF v4.3. The old bootloaders cannot read all bits of the wafer version eFuse, it can read only the first 3 low bits. So it means that the old bootloader cannot detect chip version correctly. Chips v0.0 - v0.8 are detected correctly, but other chip versions will be recognized as a version from this range.

.. only:: esp32s2 or esp32s3

    {IDF_TARGET_NAME} chip support was added in ESP-IDF v4.2. {IDF_TARGET_NAME} chips have ``rev_min`` in :cpp:type:`esp_image_header_t` header = 0 because ``Minimum Supported ESP32-S2 Revision`` Kconfig option was not introduced, which means that the old bootloader does not check the chip revision. Any app can be loaded by such bootloader in range v0.0 - v3.15.

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
