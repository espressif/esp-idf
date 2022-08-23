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

## CI Test Flow for Unit Test

Unit test uses 3 stages in CI: `build`, `assign_test`, `unit_test`.

### Build Stage:

`build_esp_idf_tests` job will build all UT configs and run script `UnitTestParser.py` to parse test cases form built elf files. Built binary (`tools/unit-test-app/build_<target>_<config>`) and parsed cases (`components/idf_test/unit_test/TestCaseAll.yml`) will be saved as artifacts.

When we add new test case, it will construct a structure to save case data during build. We'll parse the test case from this structure. The description (defined in test case: `TEST_CASE("name", "description")`) is used to extend test case definition. The format of test description is a list of tags:

1. first tag is always group of test cases, it's mandatory
2. the rest tags should be [type=value]. Tags could have default value and omitted value. For example, reset tag default value is "POWERON_RESET", omitted value is "" (do not reset) :
    * "[reset]" equal to [reset=POWERON_RESET]
    * if reset tag doesn't exist, then it equals to [reset=""]
3. the `[leaks]` tag is used to disable the leak checking. A specific maximum memory leakage can be set as follows: `[leaks=500]`. This allows no more than 500 bytes of heap to be leaked. Also there is a special function to set the critical level of leakage not through a tag, just directly in the test code ``test_utils_set_critical_leak_level()``.

The priority of using leakage level is as follows:

1. Setting by tag `[leaks=500]`.
2. Setting by ``test_utils_set_critical_leak_level()`` function.
3. Setting by default leakage in Kconfig ``CONFIG_UNITY_CRITICAL_LEAK_LEVEL_GENERAL``.

Tests marked as `[leaks]` or `[leaks=xxx]` reset the device after completion (or after each stage in multistage tests).

`TagDefinition.yml` defines how we should parse the description. In `TagDefinition.yml`, we declare the tags we are interested in, their default value and omitted value. Parser will parse the properities of test cases according to this file, and add them as test case attributes.

We will build unit-test-app with different sdkconfigs. Some config items requires specific board to run. For example, if `CONFIG_SPIRAM` is enabled, then unit test app must run on board supports PSRAM. `ConfigDependency.yml` is used to define the mapping between sdkconfig items and tags. The tags will be saved as case attributes, used to select jobs and runners. In the previous example, `psram` tag is generated, will only select jobs and runners also contains `psram` tag.

### Assign Test Stage:

`assign_unit_test` job will try to assign all cases to test jobs defined in `.gitlab-ci.yml`, according to test environment and tags. For each job, one config file with same name of test job will be generated in `components/idf_test/unit_test/CIConfigs/`(this folder will be passed to test jobs as artifacts). These config files will tell test jobs which cases it need to run, and pass some extra configs (like if the case will reset) of test case to runner.

Please check related document in tiny-test-fw for details.

### Unit Test Stage:

All jobs in `unit_test` stage will run job according to unit test configs. Then unit test jobs will use tiny-test-fw runner to run the test cases. The test logs will be saved as artifacts.

Unit test jobs will do reset before running each case (because some cases do not cleanup when failed). This makes test cases independent with each other during execution.

## Handle Unit Test CI Issues

### 1. Assign Test Failures

Gitlab CI do not support create jobs at runtime. We must maunally add all jobs to CI config file. To make test running in parallel, we limit the number of cases running on each job. When add new unit test cases, it could exceed the limitation that current unit test jobs support. In this case, assign test job will raise error, remind you to add jobs to `.gitlab-ci.yml`.

```
Too many test cases vs jobs to run. Please add the following jobs to .gitlab-ci.yml with specific tags:
* Add job with: UT_T1_1, ESP32_IDF, psram
* Add job with: UT_T1_1, ESP32_IDF
```

The above is an example of error message in assign test job. In this case, please add the following jobs in `.gitlab-ci.yml`:

```
UT_001_25:
  <<: *unit_test_template
  tags:
    - ESP32_IDF
    - UT_T1_1

UT_004_09:
  <<: *unit_test_template
  tags:
    - ESP32_IDF
    - UT_T1_1
    - psram
```

The naming rule of jobs are `UT` + `job type index` + `job index`. Each combination of tags is a different job type.

### 2. Debugging Failed Cases

First you can check the logs. It's saved as unit test job artifacts. You can download from the test job page.

If you want to reproduce locally, you need to:

1. Download artifacts of `build_esp_idf_tests`. The built binary is in `tools/unit-test-app/build_<target>_<config>` folder.
    * Built binary in CI could be slightly different from locally built binary with the same revision, some cases might only fails with CI built binary.
2. Check the following print in CI job to get the config name: `Running unit test for config: config_name`. Then flash the binary of this config to your board.
3. Run the failed case on your board (refer to Running Unit Tests section).
    * There're some special UT cases (multiple stages case, multiple devices cases) which requires user interaction:
        * You can refer to [unit test document](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/unit-tests.html#running-unit-tests) to run test manually.
        * Or, you can use `tools/unit-test-app/unit_test.py` to run the test cases (see below)

# Testing and debugging on local machine
## Running unit tests on local machine by `unit_test.py`

First, install Python dependencies and export the Python path where the IDF CI Python modules are found:

```bash
bash install.sh --enable-ttfw
source export.sh
export PYTHONPATH=$IDF_PATH/tools/ci/python_packages:$PYTHONPATH
```

Change to the unit test app directory, configure the app as needed and build it in the default "build" directory. For example:

```bash
cd $IDF_PATH/tools/unit-test-app
idf.py ut-apply-config-psram
idf.py build -T vfs
```

(Instead of these steps, you can do whatever is needed to configure & build a unit test app with the tests and config that you need.)

If you want to build exactly the same binary files under the same location as they are in CI pipelines, you may run:

```bash
cd $IDF_PATH
python tools/ci/ci_build_apps.py tools/unit-test-app -v -t $IDF_TARGET --config "configs/*=" --copy-sdkconfig --preserve-all
```

This would build all configs. if you want to build only one config (let's take `psram` as an example), you may use:

```bash
cd $IDF_PATH
python tools/ci/ci_build_apps.py tools/unit-test-app -v -t $IDF_TARGET --config "configs/psram=" --copy-sdkconfig --preserve-all
```

### run a single test case by name

```bash

./unit_test.py "UART can do select()"
```

unit_test.py script will flash the unit test binary from the (default) build directory, then run the test case.

### Run a single test case twice

```bash
./unit_test.py -r 2 "UART can do select()"
```

### run multiple unit test cases

```bash
./unit_test.py "UART can do select()" "concurrent selects work"
```

### run a multi-stage test (type of test and child case numbers are autodetected)

```bash
./unit_test.py "check a time after wakeup from deep sleep"
```

### run a list of different unit tests (one simple and one multi-stage test)

```bash
./unit_test.py "concurrent selects work" "check a time after wakeup from deep sleep"
```

### Use custom environment config file

```bash
./unit_test.py -e /tmp/EnvConfigTemplate.yml "UART can do select()"
```

Note: No sample YAML file is currently available.

### use custom application binary

```bash
./unit_test.py -b /tmp/app.bin "UART can do select()"
```

Note: This option doesn't currently work without an EnvConfigTemplate also supplied, use the default unit-test-app binaries only.

### add some options for unit tests

```bash
./unit_test.py "UART can do select()",timeout:10 "concurrent selects work",config:release,env_tag:UT_T2_1
```

Note: Setting the `config` and `env_tag` values doesn't significantly change anything but the console log output, the same binary is used.
