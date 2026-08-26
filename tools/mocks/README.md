# Mocked Components

All components in this directory mock their respective originals in the [component directory](../../components). The components in this directory are for **testing only**. Currently, the main goal is to implement Linux-based host tests with these mocking components. Target-based tests using the mocking components are not possible now but may be possible in the future.

Some components only consist of header files without any CMakeLists.txt file. The headers in these are currently needed by other mocking components to satisfy a minimal set of definitions from dependencies. *They are not a full mock implementation.* These components with header files only may be replaced by an actual mock implementation of the corresponding component in the future.
