The latest support policy for ESP-IDF can be found at [https://github.com/espressif/esp-idf/blob/master/SUPPORT_POLICY.md](https://github.com/espressif/esp-idf/blob/master/SUPPORT_POLICY.md)

Support Period Policy
=====================

* [中文版](./SUPPORT_POLICY_CN.md)

Each ESP-IDF major and minor release (V4.1, V4.2, etc) is supported for 30 months after the initial stable release date.

Supported means that the ESP-IDF team will continue to apply bug fixes, security fixes, etc to the release branch on GitHub, and periodically make new bugfix releases as needed.

Support period is divided into "Service" and "Maintenance" period:

| Period      | Duration     | Recommended for new projects?         |
| -------     | ------------ | ------------------------------------- |
| Service     | 12 months    | Yes                                   |
| Maintenance | 18 months    | No                                    |

During the Service period, bugfixes releases are more frequent. In some cases, support for new features may be added during the Service period (this is reserved for features which are needed to meet particular regulatory requirements or standards for new products, and which carry a very low risk of introducing regressions.)

During the Maintenance period, the version is still supported but only bugfixes for high severity issues or security issues will be applied.

Using an “In Service” version is recommended when starting a new project.

Users are encouraged to upgrade all projects to a newer ESP-IDF release before the support period finishes and the release becomes End of Life (EOL). It is our policy to not continue fixing bugs in End of Life releases.

Pre-release versions (betas, previews, `-rc` and `-dev` versions, etc) are not covered by any support period. Sometimes a particular feature is marked as "Preview" in a release, which means it is also not covered by the support period.

ESP-IDF should be used in an up-to-date software environment. The operating system and other third-party tools should be supported by their maintainers. ESP-IDF cannot keep compatibility with unsupported third-party tools.

The ESP-IDF Programming Guide has information about the [different versions of ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/versions.html) (major, minor, bugfix, etc).

Example
-------

ESP-IDF V3.3 was released in September 2019. It was supported for 30 months until February 2022.

- The first V3.3 release was `v3.3` in September 2019.
- The ESP-IDF team continues to backport bug fixes, security fixes, etc to the release branch `release/v3.3`.
- Periodically stable bugfix releases are created from the release branch. For example `v3.3.1`, `v3.3.2`, etc. Users are encouraged to always update to the latest bugfix release.
- V3.3 bugfix releases continue until February 2022, when all V3.3.x releases become End of Life.

Existing Releases
-----------------

ESP-IDF release V4.1 and all newer releases will follow this support period policy. The support period for each release will be announced when the release is made.

For releases made before the current support period policy was announced, the original support periods apply:

* ESP-IDF V4.0.x will be supported until October 2021
* ESP-IDF V3.3.x will be supported until February 2022
* ESP-IDF versions before V3.3 are already End of Life.

Policy History
--------------

* September 2019. This policy splits ESP-IDF releases into Standard and Long Term Support.
* July 2020. All releases from now will have the same support period, which is equal to the previous Long Term Support period. Added “In Service” period, during which versions will receive more updates.
