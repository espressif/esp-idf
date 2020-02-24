# Test Apps

This directory contains a set of ESP-IDF projects to be used as tests only, which aim to exercise various
configuration of components to check completely arbitrary functionality should it be building only, executing under
various conditions or combination with other components, including custom test frameworks.

The test apps are not intended to demonstrate the ESP-IDF functionality in any way.

# Test Apps projects

Test applications are treated the same way as ESP-IDF examples, so each project directory shall contain
* Build recipe in cmake and the main component with app sources
* Configuration files
  - `sdkconfig.ci` - Default configuration for the project
  - `sdkconfig.ci.<CONFIG>` - Other configurations, where `<CONFIG>` indicates name of the configuration
* Test executor in `ttfw_idf` format if the project is intended to also run tests (otherwise the example is build only)
  - test file in the project dir must end with `_test.py`, by should be named  `app_test.py`
  - test cases shall be decorated with `@ttfw_idf.idf_custom_test(env_tag="...")`

# Test Apps layout

The test apps should be grouped into subdirectories by category. Categories are:
* `protocols` contains test of protocol interactions.
* `network` contains system network tests
* `system` contains tests on the internal chip features, debugging and development tools.
* `security` contains tests on the chip security features.

# Test Apps local execution

* Append relevant `sdkconfig.ci.<CONFIG>` to the sdkconfig for the configuration under test
* Run `idf.py menuconfig` to configure local project attributes
* Run `idf.py build` to build the test app
* Run `python app_test.py` to run the test locally
