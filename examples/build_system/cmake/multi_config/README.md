| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

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

In this example, Development configuration is specified in `sdkconfig.defaults`, so it is the default one. You can build the project as usual:

```
idf.py build
```

To flash the project and see the output, run:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

### Production build

To build one of the Production configurations, specify a different build directory and `SDKCONFIG_DEFAULTS` file. For example, to build `prod1` configuration:

```
idf.py -B build_prod1 -D SDKCONFIG_DEFAULTS="sdkconfig.prod_common;sdkconfig.prod1" build
```

* `-B build_prod1` sets the build directory to `build_prod1`
* `-D SDKCONFIG_DEFAULTS="sdkconfig.prod_common;sdkconfig.prod1"` selects `sdkconfig.prod_common` and `sdkconfig.prod1` files to be used for creating app configuration (sdkconfig), instead of the usual `sdkconfig.defaults`. See the section below on how these two default configuration files are combined.

To flash the project and see the output, run:

```
idf.py -B build_prod1 -p PORT flash monitor
```

Note that it is not necessary to repeat `-D SDKCONFIG_DEFAULTS=...` option once the build directory has been created and `sdkconfig` file generated. For example, to build the project again, run:

```
idf.py -B build_prod1 build
```

To build and run the app with `prod2` configuration, repeat the steps above, replacing `prod1` with `prod2`.

### Combining multiple files in `SDKCONFIG_DEFAULTS`

`SDKCONFIG_DEFAULTS` build system variable selects the file which contains the default app configuration, used when no `sdkconfig` file is present. If not specified, `SDKCONFIG_DEFAULTS` is set to `"sdkconfig.defaults"`.

`SDKCONFIG_DEFAULTS` can be set to a different name from the command line, using `-D` flag of `idf.py`, as shown above. It can also be set from the project CMakeLists.txt file, before `project.cmake` is included.

It is possible to specify multiple files in this variable, separating them with semicolons. In the example given in the previous section, this is used to create a common config file for production builds and product-specific config files:

* product 1: `sdkconfig.prod_common;sdkconfig.prod1`
* product 2: `sdkconfig.prod_common;sdkconfig.prod2`

This way the common options do not need to be repeated in each of `sdkconfig.prodN` files.

### Create configuration profile files via @filename

You can further enhance your build process by using configuration profile files. These profile files contain arguments that streamline the build process for specific scenarios. Let's have our example profile files: 

- [profiles/prod](profiles/prod)
- [profiles/debug](profiles/debug)

You can use these profile files to quickly set up the build environment with specific configurations.

- To build with the production profile: `idf.py @profiles/prod build`
- To build with the debug profile: `idf.py @profiles/debug build`

This approach simplifies the process of specifying complex command-line arguments and allows for greater flexibility in managing different build scenarios.

Moreover, you can combine arguments from a profile file with additional command line arguments. Anywhere on the idf.py command line, you can specify a file as @filename.txt to read one or more arguments from the text file. Arguments in the file can be separated by newlines or spaces and are expanded exactly as if they had appeared in that order on the idf.py command line.

For example using [cutom_flash.txt](custom_flash.txt), you can expand the command: `idf.py -B build_production @custom_flash.txt monitor`

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

