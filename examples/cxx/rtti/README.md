| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Example: C++ run-time type info (RTTI)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of the RTTI feature in ESP-IDF.

By default, RTTI support is disabled in ESP-IDF. It can be enabled using `CONFIG_COMPILER_CXX_RTTI` configuration option.

In this example, `sdkconfig.defaults` file sets `CONFIG_COMPILER_CXX_RTTI` option. This enables compile time support for RTTI (`-frtti` compiler flag).

The example prints demangled type names of a few objects and functions, obtained from `typeinfo().name`. The example also generates several objects of two classes, derived from a common base class. For each object, it is demonstrated that `dynamic_cast` behaves as expected, returning non-NULL when casting to the real object type, and NULL when casting to a different type.

## How to use example

### Configure the project

To run this example, no additional configuration is necessary.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
Type names of a few objects:
	Type name of std::cout is: std::ostream
	Type name of std::cin is: std::istream
	Type of app_main is: void ()
	Type name of a lambda function is: app_main::{lambda(int, int)#1}

Generating 5 random objects and printing their types:
obj->name() is: DerivedB
	typeid(*obj).name() is: DerivedB
	dynamic_cast<DerivedA*>(obj)=0
	dynamic_cast<DerivedB*>(obj)=0x3ffb7e88

obj->name() is: DerivedB
	typeid(*obj).name() is: DerivedB
	dynamic_cast<DerivedA*>(obj)=0
	dynamic_cast<DerivedB*>(obj)=0x3ffb7e9c

obj->name() is: DerivedA
	typeid(*obj).name() is: DerivedA
	dynamic_cast<DerivedA*>(obj)=0x3ffb7eb0
	dynamic_cast<DerivedB*>(obj)=0

obj->name() is: DerivedB
	typeid(*obj).name() is: DerivedB
	dynamic_cast<DerivedA*>(obj)=0
	dynamic_cast<DerivedB*>(obj)=0x3ffb7ec4

obj->name() is: DerivedA
	typeid(*obj).name() is: DerivedA
	dynamic_cast<DerivedA*>(obj)=0x3ffb7ed8
	dynamic_cast<DerivedB*>(obj)=0

Example finished.
```
