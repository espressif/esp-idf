| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Link Time Plugins Registration

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates features of ESP-IDF build system related to link time registration of plugins. Link time registration of plugins is often used to add multiple implementations of a certain feature without having to make the application aware of all these implementations. With this approach, adding a new implementation is often as simple as adding a new source file or a new component. Aside from plugins, link time registration may be used for other purposes, such as automatic registration of test cases.

# Overview of link time registration

When using link time registration, there are typically two challenges: getting the plugin code linked into the application and enumerating the plugins at run time. The following sections explain these problems and the solutions to them.

## Ensuring that the plugin code is included into the executable

When GNU linker (ld) links a static library, it considers each object file in the library separately. The object file is ignored if it doesn't resolve any unresolved references known to the linker at that moment. With link-time plugin registration this is typically the case — the application doesn't explicitly reference any plugins, so the linker sees no reason to include the respective object files into the executable.

Aside from adding an explicit reference from the application to the plugin object file, there are two common ways to resolve this issue:

1. Link the object file of the plugin directly to the executable, and not via a static library.
2. Instruct the linker to include every object file of a library into the executable, even those which don't resolve any references from the rest of the application. For GNU ld this can be achieved by surrounding the library on the linker command line with `-Wl,--whole-archive` and `-Wl,--no-whole-archive` flags.

ESP-IDF build system implements the 2nd approach by providing a `WHOLE_ARCHIVE` component property. It can be set in component CMakeLists.txt in two ways. One is to add `WHOLE_ARCHIVE` option when calling `idf_component_register`:

```cmake
idf_component_register(SRCS file.c
                       WHOLE_ARCHIVE)
```

Another is to call `idf_component_set_property` after registering the component:
```cmake
idf_component_set_property(${COMPONENT_NAME} WHOLE_ARCHIVE TRUE)
```

This will instruct the build system to surround the component library with whole-archive flags on the linker command line, ensuring that all object files from the library get included into the final application.

Note that the linker also performs "garbage collection" at the end of the linking process, eliminating all functions and global variables which are not referenced anywhere. This is addressed in the current example using `__attribute__((constructor))` function attribute (for dynamic registration) and `KEEP()` linker fragment flag (for static registration).

## Registering and enumerating the plugins

To make use of the plugins linked into the application, the application must somehow enumerate them. There are 2 common ways to register and enumerate the plugins: dynamic and static. This example demonstrates both approaches.

### Dynamic registration (or self-registration)

With this approach, each plugin module has a function with `__attribute__((constructor))` attribute (in C) or a static global object with a non-trivial constructor (in C++). Startup code calls all constructor functions before the application entry point (`app_main`) is executed. Plugin constructor functions then register themselves by calling a function defined in the application. As an example, this registration function can, add structures describing the plugins into a linked list.

### Static registration

This approach relies on plugin description structures being collected into an array at link time.

For each plugin, a structure describing the plugin (or a pointer to it) is placed into some special input section using `__attribute((section(".plugins_desc")))`. Using the linker script generator features in ESP-IDF, all entries from this input section can be gathered into a continuous array, surrounded by some symbols (e.g. `_plugins_array_start`, `_plugins_array_end`). At run time, the application casts the `&_plugins_array_start` pointer to the plugin description structure pointer and then iterates over structures collected from all plugins.

## Example code overview

This example contains 4 components:

* `main` — Only calls two sample functions defined in `plugins` component.
* `plugins` — The main part of the plugin system.

   For dynamic registration, it provides an API which plugin components call to register themselves (`example_plugin_register`).

   It also provides two sample functions for the application:
   - `example_plugins_list`: prints the list of registered plugins. This function demonstrates static registration.
   - `example_plugins_greet`: calls a function defined by each plugin with a given argument. This function demonstrates working with dynamically registered plugins.
* `plugin_hello` and `plugin_nihao` — two almost identical components, each provides one plugin.

  Note that multiple plugins may also be defined in the same component.

Please refer to the comments in the example code for a more detailed description.

## How to use example

### Hardware Required

This example runs on any ESP development board, no special hardware is required.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
Nihao plugin performing self-registration...
Successfully registered plugin 'Nihao'
Hello plugin performing self-registration...
Successfully registered plugin 'Hello'
I (325) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
List of plugins:
- Plugin 'Hello', function greet=0x400d4f40
0x400d4f40: plugin_hello_greet at /home/user/esp/esp-idf/examples/build_system/cmake/plugins/build/../components/plugin_hello/plugin_hello.c:14

- Plugin 'Nihao', function greet=0x400d4f70
0x400d4f70: plugin_nihao_greet at /home/user/esp/esp-idf/examples/build_system/cmake/plugins/build/../components/plugin_nihao/plugin_nihao.c:14

Calling greet function of plugin 'Hello'...
Hello, World!
Done with greet function of plugin 'Hello'.
Calling greet function of plugin 'Nihao'...
你好 World!
Done with greet function of plugin 'Nihao'.
```

## Troubleshooting

When implementing the approaches described in this example, the following issues may occur:

* Plugin self-registration function (constructor function) is never called. To troubleshoot this, check the application .map file — it is generated in the `build` directory of the project. Look for the object file where the constructor function is defined. If the object file and the constructor function are missing, it means that the object file was discarded. Double-check that the `WHOLE_ARCHIVE` property of the component is set correctly. Verify that on the linker command line, the component library is surrounded by `-Wl,--whole-archive`, `-Wl,--no-whole-archive`. To see the linker command line, build the project with verbose (-v) flag.
* With static registration, the plugin description structure is missing from the link-time array. Same as in the case above, start by examining the map file.
  - If the plugin object file is missing from the map file, double-check that the `WHOLE_ARCHIVE` property of the component is set correctly (see the instructions above).
  - If the plugin object file is present, but the plugin description structure is missing, check that the linker fragment rule and `__attribute((section(...)))` use the same section name. Check that the linker fragment rule uses `KEEP()` flag.
  - If the plugin description structure is in the map file but is not located inside the link-time array (is located in some other section), check the generated linker script found in the build directory (`build/esp-idf/esp_system/ld/sections.ld`). Check that the rules for placing the plugin description structure have correct precedence with respect to other rules in the linker script.

