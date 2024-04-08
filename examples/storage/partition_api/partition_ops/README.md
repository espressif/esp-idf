| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Partition Read, Write, Erase Example

This example demonstrates how to perform partition read, write and erase operations using API functions `esp_partition_read`, `esp_partition_write` and `esp_partition_erase`.

# Example Flow

The example uses a [custom partition table](./partitions_example.csv), with a data partition `storage` used as the demo partition. For the most part the example code is well-commented so users should be able to follow along the code easily. Nevertheless, this section provides an overview of the code flow.

The partition table is searched for the `storage` partition. Once found, the entire partition is erased using `esp_partition_erase_range`. Sample data is written using `esp_partition_write`
and read back using `esp_partition_read`, verifying the read and written data match. The partition is erased once again using `esp_partition_erase_range`, limited to the sector the sample data was written to.

### Output

```
I (588) example: Written data: ESP-IDF Partition Operations Example (Read, Erase, Write)
I (588) example: Read data: ESP-IDF Partition Operations Example (Read, Erase, Write)
I (638) example: Erased data
I (638) example: Example end
```

# Others

Detailed functional description of partition API is provided in [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/spi_flash.html).

See the README.md file in the upper level 'examples' directory for more information about examples.
