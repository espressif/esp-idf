| Supported Targets | Linux |
| ----------------- | ----- |

This is a test project for verification of 'esp_partition' component APIs on Linux target (CONFIG_IDF_TARGET_LINUX).
It verifies all important APIs and properties, and prints the results.
The Block-Device Layer tests have names with a prefix 'test_partition_bdl', available in 'components/esp_partition/host_test/partition_bdl_test', and the tests check all the BDL operations and commands related to 'esp_partition' (on host side)

# Build
Source the IDF environment as usual.

Once this is done, build the application:
```bash
idf.py build
```

# Run
```bash
idf.py monitor
```
