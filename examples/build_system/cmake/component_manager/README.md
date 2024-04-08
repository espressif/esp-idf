| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Using the component manager for downloading dependencies

This example demonstrates how to use [IDF Component Manager](https://pypi.org/project/idf-component-manager/) for downloading dependencies from [the component registry](https://components.espressif.com). More details and use cases of IDF Component Manager can be found in the programming guide under `API Guides` -> `Tools` -> `IDF Component Manager`.

## How to use the example
### Hardware Required

This example is designed to work with any commonly available development kit.

### Build and Flash

Run `idf.py reconfigure` to configure this project. During CMake execution the component manager will process data from the manifest file `./main/idf_component.yml` where 2 dependencies are defined:

- `idf: ">=4.1"` - Specifies required version of ESP-IDF.
- `example/cmp: ">=3.3.3"` - Defines dependency on [example/cmp](https://components.espressif.com/component/example/cmp) component that is used by the main component.

CMake Output:
```
...
Solving dependencies requirements
Updating lock file at /home/user/esp-idf/examples/build_system/cmake/component_manager/dependencies.lock
Processing 2 dependencies:
[1/2] example/cmp
[2/2] idf
...
```

Content of the `./managed_components` directory after successful build:
```
> find  ./managed_components
./managed_components
./managed_components/example__cmp
./managed_components/example__cmp/include
./managed_components/example__cmp/include/cmp.h
./managed_components/example__cmp/LICENSE
./managed_components/example__cmp/README.md
./managed_components/example__cmp/CMakeLists.txt
./managed_components/example__cmp/changelog.md
./managed_components/example__cmp/cmp.c
./managed_components/example__cmp/idf_component.yml
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
