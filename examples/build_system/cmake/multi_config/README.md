| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Multiple Build Configurations Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to build multiple configurations of a single application. This can be useful in the following cases:

* Building binaries for multiple similar products from single codebase
* Building the application for development or production hardware
* Optimizing the application differently for development and production

This example contains three build configurations:

* Development configuration, described by `sdkconfig.defaults` file. This configuration is used by default if the application is built using `idf.py build`.
* Production configuration for product 1 ("Blinky Smart Light"), described in `sdkconfig.prod1` file. This configuration is not built by default, however it can be built as shown in the next section. It is used together `sdkconfig.prod_common`, common configuration file for all products.
* Production configuration for product 2 ("Blinky Smart Switch"), described in `sdkconfig.prod2` file. Differs from `prod1` configuration only in product name.

For each configuration, a few configuration options are set:

* Project-specific Kconfig options, `CONFIG_EXAMPLE_PRODUCT_NAME` and `CONFIG_EXAMPLE_FUNC_IMPL`. These options are declared in [component Kconfig.projbuild](main/Kconfig.projbuild). These are used to demonstrate how to create and set project-specific options. These options are set differently in `sdkconfig.defaults` and `sdkconfig.prod_common` files.
  - `CONFIG_EXAMPLE_PRODUCT_NAME` is a simple `string` option. It is used to set the product name.
  - `CONFIG_EXAMPLE_FUNC_IMPL` is a `choice` option. It is used to select which of the two source files, [func_dev.c](main/func_dev.c) or [func_prod.c](main/func_prod.c), is compiled and linked. See [component CMakeLists.txt file](main/CMakeLists.txt) for related logic.
* ESP-IDF configuration options, `CONFIG_COMPILER_OPTIMIZATION_SIZE`, `CONFIG_BOOTLOADER_LOG_LEVEL_NONE`, `CONFIG_LOG_DEFAULT_LEVEL_NONE` are set in `sdkconfig.prod_common` to illustrate a typical production configuration where log messages are disabled and optimization for size is used.

## How to Use Example

### Development build

To build the development configuration (specified in `sdkconfig.defaults`), specify it using --preset argument:

```
idf.py --preset default build
```

To flash the project and see the output, run:

```
idf.py --preset default -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

### Production build

To build one of the Production configurations, specify the name using idf.py --preset argument:

```
idf.py --preset prod1 build
```

To flash the project and see the output, run:

```
idf.py --preset prod1 -p PORT flash monitor
```

To build and run the app with `prod2` configuration, repeat the steps above, replacing `prod1` with `prod2`.

### Specifying the preset for multiple commands

To avoid having to specify `--preset` argument every time you run `idf.py`, you can set `IDF_PRESET` environment variable:

For UNIX-like systems (Linux, macOS):
```shell
export IDF_PRESET=prod1
```

For Windows (PowerShell):
```powershell
$ENV:IDF_PRESET='prod1'
```

For Windows (cmd.exe):
```shell
set IDF_PRESET=prod1
```

Then subsequent commands will work with `prod1` configuration:
```shell
idf.py build
idf.py flash monitor
```

### Combining multiple files in `SDKCONFIG_DEFAULTS`

`SDKCONFIG_DEFAULTS` build system variable selects the file which contains the default app configuration, used when no `sdkconfig` file is present. If not specified, `SDKCONFIG_DEFAULTS` is set to `"sdkconfig.defaults"`.

`SDKCONFIG_DEFAULTS` can be set to a different name from the command line, using `-D` flag of `idf.py`, as shown above. It can also be set from the project CMakeLists.txt file, before `project.cmake` is included.

It is possible to specify multiple files in this variable, separating them with semicolons. In the example given in the previous section, this is used to create a common config file for production builds and product-specific config files:

* product 1: `sdkconfig.prod_common;sdkconfig.prod1`
* product 2: `sdkconfig.prod_common;sdkconfig.prod2`

This way the common options do not need to be repeated in each of `sdkconfig.prodN` files.

### Generated `sdkconfig` file

In this example, `sdkconfig` file is placed into the build directory, instead of the project root directory as it is done by default. This allows development and production builds to exist side by side. The location of `sdkconfig` file is set using `SDKCONFIG` variable in [project CMakeLists.txt](CMakeLists.txt) file.

## Example Output

### Development build output

```
I (310) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
This app is built for running on: Blinky Development Board
func() from func_dev.c (Development) called.
See README.md for building and running other app configurations.
```

### Production build output

When building with `-DSDKCONFIG_DEFAULTS="sdkconfig.prod_common;sdkconfig.prod1"` option:

```
This app is built for running on: Blinky Smart Light
func() from func_prod.c (Production) called.
See README.md for building and running other app configurations.
```

When building with `-DSDKCONFIG_DEFAULTS="sdkconfig.prod_common;sdkconfig.prod2"` option:

```
This app is built for running on: Blinky Smart Switch
func() from func_prod.c (Production) called.
See README.md for building and running other app configurations.
```

