# Experimental C++ Component

*Warning:* This component is subject to change without notice. Don't consider it as a stable API.
It proposes future C++ interfaces of IDF components.

# Usage/Build
To use and build this component, add it as an extra component in your project's cmake file:
```cmake
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/cxx/experimental/experimental_cpp_component)
```

# Tests
To build the tests, first add them to the unit test's CMakeLists.txt:
```cmake
set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/examples/cxx/experimental/experimental_cpp_component/")
```
Then go to the unit test app's directory and run:
```bash
idf.py -T experimental_cpp_component build
```
