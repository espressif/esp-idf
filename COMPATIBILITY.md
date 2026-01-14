# Compatibility Between ESP-IDF Releases and Revisions of Espressif SoCs

* [中文版](./COMPATIBILITY_CN.md)

This document describes the ESP-IDF support and compatibility for Espressif SoC series and revisions. The compatibility documents on release branches may be out-of-date. Please check the [Compatibility file on master](https://github.com/espressif/esp-idf/blob/master/COMPATIBILITY.md) or ESP-IDF Release Notes for the latest chip support updates.

## SoC Series Support

Before official support, each SoC series typically provides `preview` versions on specific branches or tags for early development. Please note that preview support for these chips will end after the official release version is published, and will not be maintained afterwards. For mass production, please use the officially released versions.

After the first chip of a series completes support and passes testing, ESP-IDF will release the first official support version for that series. Chips of this series must run on this official version or higher.

## Chip Revision Support

After a chip series is officially supported, Espressif continues to release new chip revisions to improve performance. However, some performance improvements in new chip revisions depend on special software support, and sometimes new chip revisions must run under specific software versions to function normally.

See [Compatibility Advisory for Chip Revision Numbering Scheme](https://www.espressif.com/sites/default/files/advisory_downloads/AR2022-005%20Compatibility%20Advisory%20for%20Chip%20Revision%20Numbering%20%20Scheme.pdf) on the versioning of Espressif SoC revisions.

You can run `esptool chip-id` to detect the chip series and revision (use `esptool.py chip_id` for ESP-IDF v5.5 and older). See [SoC Errata](https://www.espressif.com/en/support/documents/technical-documents?keys=errata) for more on distinguishing chip revisions and revision improvements. Run `idf.py --version` to check the current ESP-IDF version.

There are `Recommended` and `Required` versions of ESP-IDF for each chip revision:

- `Recommended`: indicates the earliest ESP-IDF release version that currently gives the most complete support for utilizing all performance improvements of the chip revision. Using this version, software can maximize the benefits of the chip revision improvements. Running binary compiled with ESP-IDF below the `Recommended` version of a chip revision, software may not benefit from some of the bugfix/features provided by the chip revision. The chip will have almost the same behavior as its previous revision. If a chip revision involves many software updates, they may be released across multiple versions, so the recommended version may be updated periodically.

- `Required`: indicates the minimum ESP-IDF version required for the chip revision to run normally. Running binary compiled below the `Required` version, the binary may have unpredictable behavior.

Though the software can make use of all the features of a chip revision, if its version is higher than the `Recommended` version of the chip, it is still recommended to use the latest bugfix version of the release branch. The latest bugfix version fixes a number of issues and helps improve product stability.

For example, if we have a chip, whose `Required`/`Recommended` version of `release/v5.1` branch is `v5.1.2`/`v5.1.4`, and the latest release on that branch is `v5.1.6`. Then the chip will not boot up with ESP-IDF `v5.1`-`v5.1.1` or will have unpredictable behavior, and application may not make use of all benefits of the chip, when running with ESP-IDF `v5.1.2` or `v5.1.3`. Though `v5.1.4` will supports the chip revision, it is still recommended to upgrade ESP-IDF to `v5.1.6`.

## Detailed ESP-IDF Support for Chip Revisions

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

### ESP32-P4

#### v1.0, v1.3

Supported since ESP-IDF v5.3.

### ESP32-C5

#### v1.0

Supported since ESP-IDF v5.5.2.

#### v1.2

| Release branch         | Recommended | Required |
|------------------------|-------------|----------|
| release/v5.5 and above | v5.5.2+     | v5.5.2   |

### ESP32-C61

#### v1.0

Supported since ESP-IDF v5.5.2.

#### v1.1

| Release branch         | Recommended | Required |
|------------------------|-------------|----------|
| release/v5.5 and above | v5.5.2+     | v5.5.2   |

### ESP32-H4

Preview support since ESP-IDF v6.0.

## What If the ESP-IDF Version Is Lower than the `Required` Version?

Latest ESP-IDF versions can prevent from downloading to, or even execute binaries on unsupported chips. ESP-IDF of versions v4.4.5+, v5.0.1+, v5.1 and above have both esptool download check and bootloader loading check against the chip revision. While ESP-IDF v4.3.5 has only esptool downloading check.

For earlier ESP-IDF versions without such checking, which is incompatible with the given chip revision, the chips running such versions will have unpredictable behavior.
