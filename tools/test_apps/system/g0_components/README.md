| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# "G0"-components-only app

This test application will compile ESP-IDF and this test's main component with G0 components only.
The goal is to make sure that no G0 component depends on G1 or higher component.

This supports all Xtensa and RISC-V based ESP targets.

The purpose of this example is to make sure that any modification to ESP-IDF doesn't violate the G0-G1+ dependency rule.

# Using this test app

Set the target, `esp32c3` for example:
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
