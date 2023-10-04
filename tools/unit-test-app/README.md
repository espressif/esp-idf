# Unit Test App

ESP-IDF unit tests are run using Unit Test App. The app can be built with the unit tests for a specific component. Unit tests are in `test` subdirectories of respective components.

# Building Unit Test App

## CMake

* Follow the setup instructions in the top-level esp-idf README.
* Set IDF_PATH environment variable to point to the path to the esp-idf top-level directory.
* Change into `tools/unit-test-app` directory
* `idf.py menuconfig` to configure the Unit Test App.
* `idf.py -T <component> -T <component> ... build` with `component` set to names of the components to be included in the test app. Or `idf.py -T all build` to build the test app with all the tests for components having `test` subdirectory.
* Follow the printed instructions to flash, or run `idf.py -p PORT flash`.
* Unit test have a few preset sdkconfigs. It provides command `idf.py ut-clean-config_name` and `idf.py ut-build-config_name` (where `config_name` is the file name under `unit-test-app/configs` folder) to build with preset configs. For example, you can use `idf.py -T all ut-build-default` to build with config file `unit-test-app/configs/default`. Built binary for this config will be copied to `unit-test-app/output/config_name` folder.
* You may extract the test cases presented in the built elf file by calling `ElfUnitTestParser.py <your_elf>`.

# Flash Size

The unit test partition table assumes a 4MB flash size. When testing `-T all`, this additional factory app partition size is required.

If building unit tests to run on a smaller flash size, edit `partition_table_unit_tests_app.csv` and use `-T <component> <component> ...` or instead of `-T all` tests don't fit in a smaller factory app partition (exact size will depend on configured options).

# Running Unit Tests

The unit test loader will prompt by showing a menu of available tests to run:

* Type a number to run a single test.
* `*` to run all tests.
* `[tagname]` to run tests with "tag"
* `![tagname]` to run tests without "tag" (`![ignore]` is very useful as it runs all CI-enabled tests.)
* `"test name here"` to run test with given name

# Testing Unit Tests with CI

For now we prefer to use component-based unit test to test in CI. Running unit-test-app in CI is being deprecated.
