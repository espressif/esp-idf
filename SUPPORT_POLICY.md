The latest support policy for ESP-IDF can be found at [https://github.com/espressif/esp-idf/blob/master/SUPPORT_POLICY.md](https://github.com/espressif/esp-idf/blob/master/SUPPORT_POLICY.md)

Support Period Policy
=====================

* [中文版](./SUPPORT_POLICY_CN.md)

Each ESP-IDF major and minor release (V4.0, V4.1, etc) is supported for
18 months after the initial stable release date.

Supported means that the ESP-IDF team will continue to apply bug fixes,
security fixes, etc to the release branch on GitHub, and periodically
make new bugfix releases as needed.

Users are encouraged to upgrade to a newer ESP-IDF release before the
support period finishes and the release becomes End of Life (EOL). It is
our policy to not continue fixing bugs in End of Life releases.

Pre-release versions (betas, previews, `-rc` and `-dev` versions, etc)
are not covered by any support period. Sometimes a particular feature is
marked as \"Preview\" in a release, which means it is also not covered
by the support period.

The ESP-IDF Programming Guide has information about the
[different versions of ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/versions.html)
(major, minor, bugfix, etc).

Long Term Support releases
--------------------------

Some releases (starting with ESP-IDF V3.3) are designated Long Term
Support (LTS). LTS releases are supported for 30 months (2.5 years)
after the initial stable release date.

A new LTS release will be made at least every 18 months. This means
there will always be a period of at least 12 months to upgrade from the
previous LTS release to the following LTS release.

Example
-------

ESP-IDF V3.3 was released in September 2019 and is a Long Term Support
(LTS) release, meaning it will be supported for 30 months until February
2022.

- The first V3.3 release was `v3.3` in September 2019.
- The ESP-IDF team continues to backport bug fixes, security fixes,
  etc to the release branch `release/v3.3`.
- Periodically stable bugfix releases are created from the release
  branch. For example `v3.3.1`, `v3.3.2`, etc. Users are encouraged to
  always update to the latest bugfix release.
- V3.3 bugfix releases continue until February 2022, when all V3.3.x
  releases become End of Life.

Existing Releases
-----------------

ESP-IDF release V3.3 and all newer releases will follow this support
period policy.  The support period for each release will be announced
when the release is made.

For releases made before the support period policy was announced,
the following support periods apply:

- ESP-IDF V3.1.x and V3.2.x will both be supported until October 2020.
- ESP-IDF V3.0.9 (planned for October 2019) will be the last V3.0
  bugfix release. ESP-IDF V3.0.x is End of Life from October 2019.
- ESP-IDF versions before V3.0 are already End of Life.
