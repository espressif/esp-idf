| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Non-Volatile Storage (NVS) Statistics Example

This example demonstrates the usage of obtaining and interpreting statistics about the a given NVS partition, namely free/used/available/total entries and namespace count.

The default "nvs" partition is first erased, then a mock string data configuration is written to 2 different namespaces, followed by checking the changed statistics and mainly the number of newly used NVS entries.

Statistics obtained via [nvs_get_stats()](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html#_CPPv413nvs_get_statsPKcP11nvs_stats_t) are the following:
|                |                     |
| -------------- | ------------------- |
| `available entries` | Essential statistic, specifies free non-reserved entries available for data storage. |
| `used entries` | Includes 1 overhead entry per namespace + actual data entries used by data storage. |
| `free entries` | Free entries are both the free available to the user entries, and free but internally reserved (not available to the user). |
| `total entries` | Number of all entries (free + used). |


Detailed functional description of NVS and API is provided in [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_flash.html).

## Blob Storage-Overhead Measurement

In addition to the basic statistics demonstration, the example can measure how much usable storage a blob actually consumes, taking the NVS metadata and free-space fragmentation into account. This part is enabled by default and can be turned off via `idf.py menuconfig` → *Example Configuration* → *Run NVS blob storage-overhead measurement*.

The measurement sweeps a matrix of **partition sizes** × **blob sizes** and, for each cell, fills the partition to capacity and reports the heap demand, NVS entry usage and the resulting storage overhead.

### Variable partition size

The example uses a custom partition table (`partitions.csv`) that defines several NVS partitions of different sizes (`nvs_16k`, `nvs_32k`, `nvs_64k`). The measurement iterates over them using `nvs_flash_init_partition()` / `nvs_get_stats()`, so the influence of the partition size on the relative overhead becomes directly visible.

### Variable blob size

For each partition, blobs of increasing size (128, 256, 512, 1024, 2048 and 4096 bytes) are stored with unique keys until `nvs_set_blob()` returns `ESP_ERR_NVS_NOT_ENOUGH_SPACE`. The number of stored blobs is then compared against the theoretical (ideal) count derived from the documented per-blob entry cost:

```
entries_per_blob = 1 (BLOB_INDEX) + k (per-page BLOB_DATA chunk headers) + ceil(blob_size / 32)
```

where `k` is the number of pages the blob data is split across.

### Worst-case fragmentation

By design, each NVS page is a 4096-byte flash sector holding 126 usable 32-byte entries. A string occupies `1 + ceil((len + 1) / 32)` entries and must fit contiguously within a single page, while a blob may split its data into per-page chunks.

To demonstrate the worst case, the example optionally pre-populates a partition so that **every page is filled up to its last 2 entries** (enabled via *Add a worst-case (pre-fragmented) measurement pass*). This is achieved by writing large strings:

* the first string is sized to leave 2 free entries on page 0 while accounting for the namespace entry,
* every following string fills a fresh page to 124 entries, leaving exactly 2 free.

After this step the partition reports a large amount of `free_entries`, but the largest contiguous run of free entries on any page is only 2. The consequences are then measured by filling the partition with blobs:

* each blob chunk can only use 1 chunk-header + 1 data entry per page, so roughly half of the consumed space becomes metadata overhead,
* because the number of chunks per blob is bounded, large blobs may become unstoreable even though many `free_entries` remain.

This makes the relationship between fragmentation, remaining free space and resulting overhead measurable, instead of presenting a single (best-case) number that could create false expectations.

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The first part of the output shows the basic statistics demonstration:

```
...
I (265) nvs_statistics_example: Erasing the contents of the default NVS partition...
I (475) nvs_statistics_example: Opening Non-Volatile Storage (NVS) handle for namespace '_mock_data'...
I (485) nvs_statistics_example: Getting NVS statistics...
I (485) nvs_statistics_example: NVS statistics:
I (485) nvs_statistics_example: Used NVS entries: 1
I (485) nvs_statistics_example: Free NVS entries: 755
I (495) nvs_statistics_example: Available NVS entries: 629
I (495) nvs_statistics_example: Total NVS entries: 756
I (505) nvs_statistics_example: Namespace count: 1
...
I (565) nvs_statistics_example: Newly used entries match expectation.
...
```

The second part reports the blob storage overhead for each partition / blob size, in both a pristine and a pre-fragmented partition:

```
...
I (1265) nvs_statistics_example: Starting NVS blob storage-overhead measurement...

NVS BLOB TEST - 128 B (partition 'nvs_16k', pristine):
======================
heap before NVS init: 299220 B
heap after NVS init:  255472 B (diff 43748 B)

available heap after fill: 254100 B (diff 1372 B)
expected blobs count: 21
stored blobs count:   20
used_entries:  120 (3840 B)
free_entries:  6 (192 B)
total_entries: 126 (4032 B)
STORAGE OVERHEAD: 36%

NVS BLOB TEST - 128 B (partition 'nvs_16k', fragmented):
======================
heap before NVS init: 299220 B
heap after NVS init:  255472 B (diff 43748 B)
fragmentation strings written: 3

available heap after fill: 254100 B (diff 1372 B)
expected blobs count: 21
stored blobs count:   2
used_entries:  124 (3968 B)
free_entries:  2 (64 B)
total_entries: 126 (4032 B)
STORAGE OVERHEAD: 98%
...
I (9000) nvs_statistics_example: NVS blob storage-overhead measurement done.
I (9010) nvs_statistics_example: Returning from app_main().
...
```

> The exact numbers depend on the target, partition size and blob size; the values above are illustrative. The key takeaway is the difference in `STORAGE OVERHEAD` and `stored blobs count` between the pristine and fragmented passes.

To reset NVS data, erase the contents of flash memory using `idf.py erase-flash`, then upload the program again as described above.
