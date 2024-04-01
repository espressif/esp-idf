| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Finding Partitions Example

This example demonstrates how to use the partition API functions `esp_partition_find`  and `esp_partition_find_first` to search the device partition table.
These functions return the matching partitions given a set of constraints. By constraints we simply mean
properties that returned results should match - notably partition type, subtype and label/name. In the case of `esp_partition_find_first`, the first matching instance is returned;
for `esp_partition_find`, an iterator is returned iterating over matching instances.

# Example Flow

The example uses a [custom partition table](./partitions_example.csv). The first part uses `esp_partition_find_first` to search for partition instances
using two constraints - type and subtype. However, the partition table contains partitions with same type/subtype combination. In order to differentiate these partitions,
a third constraint - the label, is specified.

The second part shows how to iterate over partitions that match certain constraints, manually checking the properties of each iterated partition.
The iterator is obtained using `esp_partition_find` and is released after its use to avoid memory leaks.

### Output

```
I (310) example: Printing partition table csv file contents for reference...

# Name,   Type, SubType, Offset,  Size, Flags
# Note: if you have increased the bootloader size, make sure to update the offsets to avoid overlap
nvs,        data, nvs,      0x9000,  0x6000,
phy_init,   data, phy,      0xf000,  0x1000,
factory,    app,  factory,  0x10000, 1M,
storage1,    data, fat,             , 0x40000,
storage2,    data, fat,             , 0x40000,

I (350) example: ----------------Find partitions---------------
I (350) example: Find partition with type ESP_PARTITION_TYPE_DATA, subtype ESP_PARTITION_SUBTYPE_DATA_NVS, label NULL (unspecified)...
I (370) example:        found partition 'nvs' at offset 0x9000 with size 0x6000
I (370) example: Find partition with type ESP_PARTITION_TYPE_DATA, subtype ESP_PARTITION_SUBTYPE_DATA_PHY, label NULL (unspecified)...
I (390) example:        found partition 'phy_init' at offset 0xf000 with size 0x1000
I (390) example: Find partition with type ESP_PARTITION_TYPE_APP, subtype ESP_PARTITION_SUBTYPE_APP_FACTORY, label NULL (unspecified)...
I (410) example:        found partition 'factory' at offset 0x10000 with size 0x100000
I (410) example: Find partition with type ESP_PARTITION_TYPE_DATA, subtype ESP_PARTITION_SUBTYPE_DATA_FAT, label NULL (unspecified)...
I (430) example:        found partition 'storage1' at offset 0x110000 with size 0x40000
I (440) example: Find second FAT partition by specifying the label
I (440) example: Find partition with type ESP_PARTITION_TYPE_DATA, subtype ESP_PARTITION_SUBTYPE_DATA_FAT, label storage2...
I (450) example:        found partition 'storage2' at offset 0x150000 with size 0x40000
I (460) example: ----------------Iterate through partitions---------------
I (470) example: Iterating through app partitions...
I (480) example:        found partition 'factory' at offset 0x10000 with size 0x100000
I (480) example: Iterating through data partitions...
I (490) example:        found partition 'nvs' at offset 0x9000 with size 0x6000
I (500) example:        found partition 'phy_init' at offset 0xf000 with size 0x1000
I (500) example:        found partition 'storage1' at offset 0x110000 with size 0x40000
I (510) example:        found partition 'storage2' at offset 0x150000 with size 0x40000
I (520) example: Example end
```

# Others

Detailed functional description of partition API is provided in [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/spi_flash.html).

See the README.md file in the upper level 'examples' directory for more information about examples.
