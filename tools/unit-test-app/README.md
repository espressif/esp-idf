# Unit Test App

ESP-IDF unit tests are run using Unit Test App. The app can be built with the unit tests for a specific component. Unit tests are in `test` subdirectories of respective components.

# Building Unit Test App

* Follow the setup instructions in the top-level esp-idf README.
* Set IDF_PATH environment variable to point to the path to the esp-idf top-level directory.
* Change into `tools/unit-test-app` directory
* `make menuconfig` to configure the Unit Test App.
* `make TEST_COMPONENTS=` with `TEST_COMPONENTS` set to names of the components to be included in the test app. Or `make TESTS_ALL=1` to build the test app with all the tests for components having `test` subdirectory.
* Follow the printed instructions to flash, or run `make flash`.

# Running Unit Tests

The unit test loader will prompt by showing a menu of available tests to run:

* Type a number to run a single test.
* `*` to run all tests.
* `[tagname]` to run tests with "tag"
* `![tagname]` to run tests without "tag" (`![ignore]` is very useful as it runs all CI-enabled tests.)
* `"test name here"` to run test with given name
