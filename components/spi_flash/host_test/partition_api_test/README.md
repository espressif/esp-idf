| Supported Targets | Linux |
| ----------------- | ----- |

This is a test project for partition-related APIs on Linux target (CONFIG_IDF_TARGET_LINUX).

# Build
Source the IDF environment as usual.

Once this is done, build the application:
```bash
idf.py build partition-table
```
Note that for the time being, `partition-table` target needs to be built manually.

# Run
```bash
`build/partition_api_test.elf`
```
