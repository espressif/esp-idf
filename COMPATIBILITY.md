# Compatibility Between ESP-IDF Releases and Revisions of Espressif SoCs

* [中文版](./COMPATIBILITY_CN.md)

Espressif keeps improving the performance of its SoCs by providing new chip revisions. However, some of the improvements require special software support. Some of the software supports are even mandatory for the chip revisions to run normally.

This document describes the compatibility between ESP-IDF releases and Espressif SoC revisions.

NOTE: This document on release branches may be out-of-date. Check the [Compatibility file on master](https://github.com/espressif/esp-idf/blob/master/COMPATIBILITY.md) for the most accurate information.

See [Compatibility Advisory for Chip Revision Numbering Scheme](https://www.espressif.com/sites/default/files/advisory_downloads/AR2022-005%20Compatibility%20Advisory%20for%20Chip%20Revision%20Numbering%20%20Scheme.pdf) on the versioning of Espressif SoC revisions.

You can run `esptool chip_id` to detect the series and revision of an SoC. See [SoC Errata](https://www.espressif.com/en/support/documents/technical-documents?keys=errata) for more on how to distinguish between chip revisions, and the improvements provided by chip revisions. And run `idf.py --version` to know the version of current ESP-IDF.

## ESP-IDF Support for Different Chip Revisions

The sections below show the requirements to ESP-IDF version of chip revisions. Each chip revision corresponds to specific `Recommended` and `Required` versions of ESP-IDF:

- `Recommended`: shows from which version of ESP-IDF you can make use of all the improvements of the chip revision. Running binary compiled with ESP-IDF below the `Recommended` version of a chip revision, software may not benefit from the bugfix/features provided by the chip revision. The chip will have almost the same behavior as its previous revision.

- `Required`: shows the minimum version required to run the chip revision normally. Running binary compiled below the `Required` version, the binary may have unpredictable behavior.

Though the software can make use of all the features of a chip revision, if its version is higher than the `Recommended` version of the chip, it is still recommended to use the latest bugfix version of the release branch. The latest bugfix version fixes a number of issues and helps improve product stability.

For example, if we have a chip, whose `Required`/`Recommended` version of `release/v5.1` branch is `v5.1.2`/`v5.1.4`, and the latest release on that branch is `v5.1.6`. Then the chip will not boot up with ESP-IDF `v5.1`-`v5.1.1` or will have unpredictable behavior, and application may not make use of all benefits of the chip, when running with ESP-IDF `v5.1.2` or `v5.1.3`. Though `v5.1.4` well supports the chip revision, it is still recommended to upgrade ESP-IDF to `v5.1.6`.

### ESP32

#### v0.0, v1.0, v3.0

Supported since initial version of ESP-IDF.

#### v1.1

To be added.

#### v2.0

To be added.

#### v3.1

To be added.

### ESP32-S2

#### v0.0

Supported since ESP-IDF v4.2.

#### v1.0

| Release branch         | Recommended | Required |
|------------------------|-------------|----------|
| release/v4.2           | v4.2.3+     | v4.2     |
| release/v4.3           | v4.3.3+     | v4.3     |
| release/v4.4           | v4.4.6+     | v4.4     |
| release/v5.0           | v5.0.4+     | v5.0     |
| release/v5.1           | v5.1.2+     | v5.1     |
| release/v5.2 and above | v5.2+       | v5.2     |

### ESP32-C3

#### v0.2 - v0.4

Supported since ESP-IDF v4.3.

#### v1.1

| Release branch         | Recommended | Required |
|------------------------|-------------|----------|
| release/v4.2           | EOL         | EOL      |
| release/v4.3           | v4.3.7+     | v4.3.7   |
| release/v4.4           | v4.4.7+     | v4.4.7   |
| release/v5.0           | v5.0.5+     | v5.0.5   |
| release/v5.1           | v5.1.3+     | v5.1.3   |
| release/v5.2 and above | v5.2+       | v5.2     |

### ESP32-S3

#### v0.1, v0.2

Supported since ESP-IDF v4.4.

### ESP32-C2 & ESP8684

#### v1.0, v1.1

Supported since ESP-IDF v5.0.

#### v1.2

| Release branch         | Recommended | Required |
|------------------------|-------------|----------|
| release/v5.0           | v5.0.7+     | v5.0     |
| release/v5.1           | v5.1.4+     | v5.1     |
| release/v5.2           | v5.2.2+     | v5.2     |
| release/v5.3 and above | v5.3+       | v5.3     |

#### v2.0

| Release branch         | Recommended | Required |
|------------------------|-------------|----------|
| release/v5.0           | v5.0.8+     | v5.0.8   |
| release/v5.1           | v5.1.5+     | v5.1.5*  |
| release/v5.2           | v5.2.4+     | v5.2.4   |
| release/v5.3           | v5.3.2+     | v5.3.2*  |
| release/v5.4 and above | v5.4+       | v5.4     |

Note: IDF v5.1.5 and v5.3.2 are compatible with C2 v2.0. However the chip revision check hasn't been updated on these releases. Enable `ESP32C2_REV2_DEVELOPMENT` config to bypass the outdated check.

### ESP32-C6

#### v0.0, v0.1

Supported since ESP-IDF v5.1.

#### v0.2

| Release branch         | Recommended | Required |
|------------------------|-------------|----------|
| release/v5.1           | v5.1.5+     | v5.1     |
| release/v5.2           | v5.2.4+     | v5.2     |
| release/v5.3           | v5.3.2+     | v5.3     |
| release/v5.4 and above | v5.4+       | v5.4     |

### ESP32-H2

#### v0.1, v0.2

Supported since ESP-IDF v5.1.

#### v1.2

| Release branch         | Recommended | Required |
|------------------------|-------------|----------|
| release/v5.1           | v5.1.6+     | v5.1.6   |
| release/v5.2           | v5.2.5+     | v5.2.5   |
| release/v5.3           | v5.3.3+     | v5.3.3   |
| release/v5.4           | v5.4.1+     | v5.4.1   |
| release/v5.5 and above | v5.5+       | v5.5     |

## What If the ESP-IDF Version Is Lower than the `Required` Version?

Latest ESP-IDF versions can prevent from downloading to, or even execute binaries on unsupported chips. ESP-IDF of versions v4.4.5+, v5.0.1+, v5.1 and above have both esptool download check and bootloader loading check against the chip revision. While ESP-IDF v4.3.5 has only esptool downloading check.

For earlier ESP-IDF versions without such checking, which is incompatible with the given chip revision, the chips running such versions will have unpredictable behavior.
