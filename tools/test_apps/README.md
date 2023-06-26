# Test Apps

This directory contains a set of ESP-IDF projects to be used as tests only, which aim to exercise various
configuration of components to check completely arbitrary functionality should it be building only, executing under
various conditions or combination with other components, including custom test frameworks.

The test apps are not intended to demonstrate the ESP-IDF functionality in any way.

## Test Apps projects

Test applications are treated the same way as ESP-IDF examples, so each project directory shall contain

- Build recipe in cmake and the main component with app sources
- Configuration files, `sdkconfig.ci` and similar (see below)
- Python test scripts, `pytest_....py` (optional)

## Test Apps layout

The test apps should be grouped into subdirectories by category. Categories are:

- `protocols` contains test of protocol interactions.
- `network` contains system network tests
- `system` contains tests on the internal chip features, debugging and development tools.
- `security` contains tests on the chip security features.

## CI Behavior

### Configuration Files

For each project in test_apps (and also examples):

- If a file `sdkconfig.ci` exists then it's built as the `default` CI config.
- If any additional files `sdkconfig.ci.<CONFIG>` exist then these are built as alternative configs, with the specified `<CONFIG>` name.

The CI system expects to see at least a "default" config, so add `sdkconfig.ci` before adding any `sdkconfig.ci.CONFIG` files.

- By default, every CI configurations is built for every target SoC (an `m * n` configuration matrix). However if any `sdkconfig.ci.*` file contains a line of the form `CONFIG_IDF_TARGET="targetname"` then that CI config is only built for that one target. This only works in `sdkconfig.ci.CONFIG`, not in the default `sdkconfig.ci`.
- Each configuration is also built with the contents of any `sdkconfig.defaults` file or a file named `sdkconfig.defaults.<TARGET>` appended. (Same as a normal ESP-IDF project build.)

## Test Apps local execution

Some of the examples have `pytest_....py` scripts that are using the `pytest` as the test framework. For detailed information, please refer to the "Run the Tests Locally" Section under [ESP-IDF tests in Pytest documentation](../../docs/en/contribute/esp-idf-tests-with-pytest.rst)
