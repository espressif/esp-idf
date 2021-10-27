# Building
Several configurations are provided as `sdkconfig.ci.XXX` and serve as a template. 

## Example with configuration "panic" for target ESP32
```
idf.py set-target esp32
cat sdkconfig.defaults sdkconfig.ci.panic > sdkconfig
idf.py build
```

# Running
All the setup needs to be done as described in the [test apps README](../../README.md), except that the test cases need to be specified when running the app:

```
python app_test.py test_panic_illegal_instruction
```

Multiple test cases are passed as additional arguments:

```
python app_test.py test_panic_illegal_instruction test_panic_int_wdt test_panic_storeprohibited
``` 

*Note that you need to pick the correct test cases at run time according to the configuration you built before. The above examples are for configuration "panic"*
