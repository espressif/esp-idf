| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |

# LP Core Build System Custom Modification Example:

This example demonstrates how to customize the build-process for a LP-Core project;

The ULP project is added in the [component CMakeLists.txt](main/CMakeLists.txt) with `ulp_add_project()`, which lets you add the custom ULP project defined in [CmakeLists.txt](main/ulp/CMakeLists.txt) to the IDF build-process.

This lets you customize the ULP app build process like you would any other standard cmake project. In this example we show how to add and link a custom library, as well as setting additional compile options.

The ULP project defines and links a library with a test function, `lib_test_func_sum`, which simply adds two numbers. HP-Core will print the result of this operation as an indication that the ULP did indeed successfully link and run a function from this library.

It also show how to add a custom compile option by applying `-msave-restore`, which will force the compiler to use a common library call for function prologues/epilogues, which may reduce the code-size. 

## Example output

```
...
Not a ULP wakeup, initializing it!
Sum calculated by ULP using external library func: 11
...
```