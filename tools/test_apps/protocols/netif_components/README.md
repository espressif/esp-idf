| Supported Targets | ESP32 |
| ----------------- | ----- |

# Networking component dependency

This test application checks the list of components included into the build when:
* `esp_netif` component is added to the build.
* `lwip`  component is added to the build.

The test checks that driver components are not included in the build

# Using this test app

* Remove the build artifacts `idf.py fullclean`
* Choose the component to test (either `esp_netif` or `lwip`)
* Build the project `idf.py build`

# Troubleshooting

If you get a build error in this example, please check there's no dependency from the tested component (either `esp_netif` or `lwip`) to any defined driver component.

Please open the project `CMakeLists.txt` to view the expected dependencies and driver's components that must not be included in the list:
* CMake variable `driver_components` contains list of driver's components
* CMake variable `expected_build_components` contains list expected dependecies

Note that this project creates `component_deps.dot`, a simpified dependecy graph that could be used to display and troubleshoot component dependencies.

Note that this test is executed for one target only in CI (ESP32), but shall work correctly for all IDF supported targets.
