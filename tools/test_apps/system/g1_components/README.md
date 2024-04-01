| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# "G1"-components-only app

This test application checks the list of components included into the build when "G1" components are added to the build. If G1 components don't have any dependencies outside of G1, then only G1 components themselves should be built.

Currently, this is not the case, and many other components are added to the build. See `extra_components_which_shouldnt_be_included` list inside CMakeLists.txt.

The purpose of this example is to:

- Document which extra components are known to be included into G1 build.
- Fail the build if additional components are added to G1 build, preventing new dependencies outside of G1 from being added to G1 components.

Once all the unwanted dependencies of G1 components are removed, the `extra_components_which_shouldnt_be_included` list should become empty.

Please note, if an extra component B is already added to the build as a dependency of component A (A -> B), the build of this example will not fail when a new dependency onto the same component is added (C -> B). It will only fail when a new component is added to the build. Diff-ing `build/component_deps.dot` (see below) against a known-good one could be an alternative, but it will likely be quite fragile.

# Using this test app

To check G1 components using this app, run:
```bash
idf.py reconfigure
```
(optionally, set IDF_TARGET to check this for a different target than the default one)

# Troubleshooting

If you get a build error in this example,

```
Unexpected components list in G1 build. Expected: <list>. Actual: <list>.
```

it means that the list of extra components added to G1 build has changed compared to `extra_components_which_shouldnt_be_included`.

- If "Expected" contains more components than "Actual" — great! You have simplified the build dependency graph. Identify the component which is no longer added to the build and remove it from `extra_components_which_shouldnt_be_included`.
- If "Actual" contains more components than "Expected" — not so great. It means that some change has caused additional component to be "pulled into" the build. If you don't know which additional component was added, check `component_deps.dot` file in the build directory (see below).

Please try very hard not to increase the number of components in `extra_components_which_shouldnt_be_included`.

# Component dependencies graph (`component_deps.dot`)

When this project is configured, `component_deps.dot` file in the build directory is generated. This file contains a Graphviz graph showing the component dependencies. You can visualize this graph (using `dot` tool or online at https://dreampuf.github.io/GraphvizOnline/) to see why an extra component got added. You can also build the project for the base branch, to compare the graph to a known good one.
