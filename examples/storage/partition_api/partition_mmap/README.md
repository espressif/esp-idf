| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Partition Memory Map Example

This example demonstrates how to use `esp_partition_mmap` to configure MMU and map a partition into memory address space for read operations.

# Example Flow

The example uses a [custom partition table](./partitions_example.csv), with a data partition `storage` used for demonstration. Before mapping this partition to memory,
data is written to the partition used for verification.

The partition API function `esp_partition_mmap` is used to get a pointer to the mapped memory region and a handle to the mapping. The pointer is used to transparently read back the
verification data written previously. Once the data written and read are verified to be the same, the function `spi_flash_munmap` is used to release the mapping.

### Output
```
I (309) example: Written sample data to partition: ESP-IDF Partition Memory Map Example
I (309) example: Mapped partition to data memory address 0x3f410000
I (319) example: Read sample data from partition using mapped memory: ESP-IDF Partition Memory Map Example
I (329) example: Data matches
I (329) example: Unmapped partition from data memory
I (339) example: Example end
```

# Others

Detailed functional description of partition API is provided in [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/spi_flash.html).

See the README.md file in the upper level 'examples' directory for more information about examples.
