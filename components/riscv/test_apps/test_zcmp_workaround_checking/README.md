| Supported Targets | ESP32-C5 | ESP32-C61 | ESP32-H4 | ESP32-P4 |
| ----------------- | -------- | --------- | -------- | -------- |

Build-only check for the post-link ZCMP workaround validator.

CI builds the default app on every target with ``SOC_CPU_ZCMP_WORKAROUND``
(valid mintthresh workaround + ZCMP). No flash; a successful link is the pass
criterion.

Negative coverage lives in ``tools/test_build_system`` (cmake v1/v2): build with
``-DTEST_INVALID_WORKAROUND=1`` must fail with ``ZCMP workaround violation``.
