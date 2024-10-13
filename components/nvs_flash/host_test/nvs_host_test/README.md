| Supported Targets | Linux |
| ----------------- | ----- |

# To build 
Navigate to the host test folder and run the IDF build command for linux target

```
cd $IDF_PATH
. ./export.sh
cd components/nvs_flash/host_test/nvs_host_test
idf.py --preview set-target linux
idf.py build
```

# To run tests locally
Navigate to the IDF root directory and run the binary from there. This ensures all calls to relative paths from test cases are resolved correctly.
This is limitation of ci pipeline as it was not possible to specify the working directory of the host tests.

```
cd $IDF_PATH
./components/nvs_flash/host_test/nvs_host_test/build/nvs_host_test.elf
```

# To create coverage report
After running the tests, the coverage report can be generated following way.

```
cd components/nvs_flash/host_test/nvs_host_test
idf.py build coverage
open ./build/coverage_report/index.html
```