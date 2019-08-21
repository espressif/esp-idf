# Example: C++ exception handling

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of C++ exceptions in ESP-IDF.

By default, C++ exceptions support is disabled in ESP-IDF. It can be enabled using `CONFIG_COMPILER_CXX_EXCEPTIONS` configuration option.

In this example, `sdkconfig.defaults` file sets `CONFIG_COMPILER_CXX_EXCEPTIONS` option. This enables both compile time support (`-fexceptions` compiler flag) and run-time support for C++ exception handling.

Example source code declares a class which can throw exception from the constructor, depending on the argument. It illustrates that exceptions can be thrown and caught using standard C++ facilities.

## How to use example

### Build and Flash

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
app_main starting
In constructor, arg=42
In constructor, arg=0
In destructor, m_arg=42
Exception caught: Exception in constructor
app_main done
```

