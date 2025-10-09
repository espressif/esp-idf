| Supported Targets | ESP32 | ESP32-C3 |
| ----------------- | ----- | -------- |

This is a test app for 'esp_partition' component. It verifies all the important APIs and properties and prints the results.
The Block Device Layer related tests have names with a prefix 'test_bdl', and they check all the BDL operations related to 'esp_partition' including parallel access to 2 partitions.

In CI, it is sufficient to run this test for one chip of each architecture.

# Building and Running

To run locally:

```bash
idf.py build flash monitor
```

The tests will be executed and the summary will be printed.
Note, when the Python test script is executed in internal CI, it will test each configuration one by one. When executing this script locally, it will use whichever binary is already built and available in `build` directory.
