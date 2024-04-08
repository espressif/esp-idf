| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Unit Testing

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use [Unity](https://github.com/ThrowTheSwitch/Unity) library to add unit tests to custom components. Two features of the Unity library are used in this example:

* _Assertions_ (`TEST_ASSERT` and similar) are used when writing the test cases. See [the reference](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md) for more information about assertions.

* `UNITY_BEGIN()` and `UNITY_END()` macros allow Unity to count the number of tests which have passed or failed, and display the summary.

In addition to features of Unity, this example demonstrates _test registration_ feature of ESP-IDF. This feature works when unit test functions are declared using `TEST_CASE` macro. Such functions from all the object files in the program are automatically collected into a list, and run-time facilities are provided to run functions from this list. The example demonstrates usage of `TEST_CASE` macro and of the functions which execute registered tests.

_Note: It is also possible to use built-in Unity facility, unity_fixture.h, to declare and execute the tests. However this is out scope of the current example. Refer to [Unity example_2](https://github.com/ThrowTheSwitch/Unity/tree/master/examples/example_2) for details._

## Project layout

```
unit_test                      — Application project directory
  - components                 — Components of the application project
    + testable
  + main                       - Main source files of the application project
  + test                       — Test project directory
  Makefile / CMakeLists.txt    - Makefiles of the application project
```

The layout of this example resembles a layout of a real project which has custom components in its `components/` directory. In this case this is the component called `testable`. In addition to `components/`, `main/` and `Makefile` / `CMakeLists.txt`, this project includes a *test project* in its *test* subdirectory.

The purpose of two projects is to implement different application behavior when running normally, and when running the unit tests. Top level project is the actual application being developed. Test project included within is a simple application which only runs the unit tests.

## Unit tests for a component

Inside the `testable` component, unit tests are added into `test` directory. `test` directory contains source files of the tests and the component makefile (component.mk / CMakeLists.txt).

```
unit_test
  - components                              - Components of the application project
    - testable
      - include
      - test                                - Test directory of the component
        * component.mk / CMakeLists.txt     - Component makefile of tests
        * test_mean.c                       - Test source file
      * component.mk / CMakeLists.txt       - Component makefile
      * mean.c                              - Component source file
```

When the main application project is compiled, tests are not included. Test project includes the tests by setting `TEST_COMPONENTS` variable in the project makefile.

## How to use example

### Hardware required

This example doesn't require any special hardware, and can run on any ESP32 development board.

### Configure the project

As explained above, this example contains two projects: application project and test project.

For the test project, you can open the project configuration menu (`idf.py menuconfig`) and explore a few options related to Unity under Component Config, Unity unit testing library.

### Build and flash

As explained above, this example contains two projects: application project and test project.

1. Application project calls an API defined in the component, and displays the results. It is not of much value to run. Application project is provided mainly to illustrate the layout of all the files. If you decide to run this project, the procedure is:

    * Run `idf.py -p PORT flash monitor` in the current directory (`unit_test`).
    * Observe the output: a list of random numbers and their mean value.

2. Test project is responsible for running the tests.

	* Enter `test` directory (`unit_test/test`), and run `idf.py -p PORT flash monitor`.
	* Observe the output: results of test case execution.


To exit the serial monitor, type ``Ctrl-]``.

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Example output

### Application project output

```
In main application. Collecting 32 random numbers from 1 to 100:
   5   66   86   98   62   66   40   42    7   62 
  33   77   40   54    8   75   39   82   73    4 
  22    5   72   78   53    1    6   48   23    1 
  84   55 
Mean: 45
```

### Test project output

```
#### Executing one test by its name #####

Running Mean of an empty array is zero...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:16:Mean of an empty array is zero:PASS

-----------------------
1 Tests 0 Failures 0 Ignored 
OK

#### Running tests with [mean] tag #####

Running tests matching '[mean]'...
Running Mean of an empty array is zero...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:16:Mean of an empty array is zero:PASS
Running Mean of a test vector...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:22:Mean of a test vector:PASS
Running Another test case which fails...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:35:Another test case which fails:FAIL: Expected 2147483647 Was -1

-----------------------
3 Tests 1 Failures 0 Ignored 
FAIL

#### Running tests without [fails] tag #####

Running tests NOT matching '[fails]'...
Running Mean of an empty array is zero...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:16:Mean of an empty array is zero:PASS
Running Mean of a test vector...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:22:Mean of a test vector:PASS

-----------------------
2 Tests 0 Failures 0 Ignored 
OK

#### Running all the registered tests #####

Running Mean of an empty array is zero...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:16:Mean of an empty array is zero:PASS
Running Mean of a test vector...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:22:Mean of a test vector:PASS
Running Another test case which fails...
/Users/ivan/e/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:35:Another test case which fails:FAIL: Expected 2147483647 Was -1

-----------------------
3 Tests 1 Failures 0 Ignored 
FAIL
```

This is the initial example output. At this point, press ENTER key to get the list of tests:

```
Here's the test menu, pick your combo:
(1)	"Mean of an empty array is zero" [mean]
(2)	"Mean of some test vectors" [mean]

Enter test for running.
```

This is the test menu, which is launched by `unity_run_menu` function in the test project. It allows running the tests in a few ways:

* Run specific test by its index: type the number and press ENTER.
* Run a group of tests with a certain tag: type the tag, including square brackets, and press ENTER.
* Run all tests except the ones with a certain tag: press `!`, then type the tag, including the square brackets, then press ENTER.
* Run test with a specific name: type the name of the test, including quotes, and press ENTER.
* Run all the tests: press `*` and then ENTER.

Note that the test menu prompt does not echo back the characters typed.

For example, the output when typing `1` and then `ENTER` will be:

```
Running Mean of an empty array is zero...
/home/user/esp/esp-idf/examples/system/unit_test/components/testable/test/test_mean.c:15:Mean of an empty array is zero:PASS
Test ran in 16ms

-----------------------
1 Tests 0 Failures 0 Ignored 
OK
Enter next test, or 'enter' to see menu
```
