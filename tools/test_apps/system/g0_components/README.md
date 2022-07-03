| Supported Targets | ESP32-C3 | ESP32-C2 | ESP32-H2 |
| ----------------- | -------- | -------- | -------- |

All Xtensa based targets (ESP32, ESP32-S2, ESP32-S3) are currently not supported by this test, because their components having dependencies on G1+ components.

# "G0"-components-only app

This test application will compile ESP-IDF and this test's main component with G0 components only. The goal is to make sure that no G0 component depends
on G1 or higher component.

Currently, this test only supports RISC-V based targets as Xtensa ones still have some G0 components depending on G1+ components.

Compiling this test with an Xtensa based target will result in a CMake error, showing all the non-G0 components included in the build file generation.

The purpose of this example is to make sure that any modification to ESP-IDF doesn't violate the G0-G1+ dependency rule.

# Using this test app

Set the target to a RISC-V based, `esp32c3` for example:
```bash
idf.py set-target esp32c3
```

Then, trigger the build:
```bash
idf.py build
```

Build should be successful if there is no dependency problem between G0 and upper layers. 

# Component dependencies graph (`component_deps.dot`)

When this project is configured, `component_deps.dot` file in the build directory is generated. This file contains a Graphviz graph showing the component dependencies. You can visualize this graph (using `dot` tool or online at https://dreampuf.github.io/GraphvizOnline/) to see why an extra component got added. You can also build the project for the base branch, to compare the graph to a known good one.
