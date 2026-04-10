| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Using the component manager for downloading dependencies

This example demonstrates how to use [IDF Component Manager](https://pypi.org/project/idf-component-manager/) for downloading dependencies from [ESP Component Registry](https://components.espressif.com). More details and use cases of IDF Component Manager can be found in the programming guide under `API Guides` -> `Tools` -> `IDF Component Manager`.

## How to use the example
### Hardware Required

This example is designed to work with any commonly available development kit.

### Build and Flash

Run `idf.py reconfigure` to configure this project. During CMake execution the component manager will process data from the manifest file `./main/idf_component.yml` where 2 dependencies are defined:

- `idf: ">=6.0"` - Specifies required version of ESP-IDF.
- `example/cmp: ">=3.3.3"` - Defines dependency on [example/cmp](https://components.espressif.com/component/example/cmp) component that is used by the main component.

CMake Output:
```
...
--  IDF: Component manager round 1...
NOTICE: Dependencies lock doesn't exist, solving dependencies.
...NOTICE: Updating lock file at /Users/sudeepmohanty/esp/esp-idf/examples/build_system/cmakev2/features/component_manager/dependencies.lock
NOTICE: Processing 2 dependencies:
NOTICE: [1/2] example/cmp (3.3.9~1)
NOTICE: [2/2] idf (6.1.0)
...
```

Content of the `./managed_components` directory after successful build:
```
▶ find  ./managed_components
./managed_components
./managed_components/example__cmp
./managed_components/example__cmp/CMakeLists.txt
./managed_components/example__cmp/LICENSE
./managed_components/example__cmp/changelog.md
./managed_components/example__cmp/idf_component.yml
./managed_components/example__cmp/include
./managed_components/example__cmp/include/cmp.h
./managed_components/example__cmp/README.md
./managed_components/example__cmp/examples
./managed_components/example__cmp/examples/cmp_ex
./managed_components/example__cmp/examples/cmp_ex/CMakeLists.txt
./managed_components/example__cmp/examples/cmp_ex/README.md
./managed_components/example__cmp/examples/cmp_ex/main
./managed_components/example__cmp/examples/cmp_ex/main/CMakeLists.txt
./managed_components/example__cmp/examples/cmp_ex/main/idf_component.yml
./managed_components/example__cmp/examples/cmp_ex/main/cmp_ex.c
./managed_components/example__cmp/cmp.c
./managed_components/example__cmp/.component_hash
```

Flash the project and run the serial monitor to view the output:

```
idf.py -p PORT flash monitor
```

### Example Output

The example outputs a line from the `cmp_hello` function from the component downloaded by the component manager.

```
Hello from example component!
```
