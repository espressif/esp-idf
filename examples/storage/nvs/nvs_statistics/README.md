| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Non-Volatile Storage (NVS) Statistics Example

This example demonstrates the usage of obtaining and interpreting statistics about the a given NVS partition, namely free/used/available/total entries and namespace count.

## When to use this example

- You want to measure NVS partition usage (free/used/available/total entries, namespace count) via `nvs_get_stats()`.
- You need to understand blob storage overhead and fragmentation before sizing an NVS partition.

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

In addition to the basic statistics demonstration, the example can measure how many entries a single blob actually consumes compared to its ideal (non-fragmented) cost, taking the NVS metadata and free-space fragmentation into account. This part is disabled by default and can be turned on via `idf.py menuconfig` → *Example Configuration* → *Run NVS blob storage-overhead measurement*.

For every measured cell the example erases and initializes a dedicated partition, pre-populates its pages with strings so that a target number of free entries remains on each page, writes a single blob and compares its real entry footprint against the ideal one. The results are printed as a single table.

### Configurable sweep

The measurement iterates over two configurable arrays defined at the top of `nvs_statistics_example.c`:

* `measurement_combos[]` — partition size / blob size combinations. The default set sweeps three partition sizes and, for the 16 kB partition, a few blob sizes that straddle the point where the blob no longer fits: `16k / 128 B`, `16k / 350 B`, `16k / 416 B`, `16k / 417 B`, `32k / 768 B`, `32k / 920 B` and `64k / 1612 B`. The partition names refer to the NVS partitions declared in the custom partition table (`partitions.csv`).
* `page_free_ranges[]` — per-page free-entry targets, each a `{ first_page_free, rest_free }` pair given as an absolute number of free (available) entries that must remain on a page (out of 126 per page). The default set is `{14, 11}`, `{14, 7}`, `{14, 4}` and `{13, 4}`.

Every combination is measured at every free-entry target.

### Per-page pre-population

By design, each NVS page is a 4096-byte flash sector holding 126 usable 32-byte entries. A string occupies `1 + ceil((len + 1) / 32)` entries and must fit contiguously within a single page, while a blob may split its data into per-page chunks.

Each free-entry target leaves `first_page_free` free entries on the first page and `rest_free` on every remaining page (the namespace entries on page 0 count towards page 0's occupancy). To leave the requested number of free entries on a page, the example writes a "keep" string that occupies all but the requested free entries, followed by a "filler" string that occupies those remaining entries. Once the whole partition has been filled this way, all filler strings are erased. Every page therefore exposes exactly the requested number of reclaimable free entries, scattering the free space into per-page gaps — the fragmentation pattern the subsequent blob write has to cope with.

### Reported columns

A single blob is then written into the pre-populated partition and its consumption is tabulated:

| Column | Meaning |
| ------ | ------- |
| `Blob Size [B]` | Blob payload size. |
| `Partition Size [k]` | Partition size in kibibytes. |
| `Free Entries per Page` → `First [-]` | Number of free (available) entries left on the first NVS page. |
| `Free Entries per Page` → `Remaining [-]` | Number of free (available) entries left on every remaining NVS page. |
| `Available Entries [-]` | `available_entries` from `nvs_get_stats()` right before the blob is written. |
| `Expected Entries [-]` | Ideal (non-fragmented) blob cost: `1 (BLOB_INDEX) + chunks + ceil(blob_size / 32)`. |
| `Actual Entries [-]` | Real entry consumption of the written blob (`used_entries` delta). |
| `Overhead Entries [-]` | `Actual Entries − Expected Entries`. |
| `Overhead [%]` | `100 × Overhead Entries / Expected Entries`. |

If the blob does not fit into the pre-populated partition, `nvs_set_blob()` returns `ESP_ERR_NVS_NOT_ENOUGH_SPACE` and the corresponding row reports `FAIL` for the actual consumption and the derived overhead columns.

### Why a blob may not fit even when `Available Entries` looks sufficient

`Available Entries` is a *partition-wide* total of free, non-reserved entries. It is tempting to compare it against `Expected Entries` and conclude that any blob whose ideal cost is smaller must fit — but a fragmented partition does not work that way, which is exactly what this measurement demonstrates. Several NVS design properties make the real requirement larger, and the usable free space smaller, than that single number suggests:

* **A blob is stored as one data chunk per page, not as one contiguous run.** When the free space is scattered (as the per-page pre-population deliberately arranges it), NVS splits the blob so that each page receives at most one `BLOB_DATA` chunk. The chunks are sized to whatever run of free entries a page can offer, and the split continues page by page until the whole payload is stored.
* **Every chunk carries its own metadata entry.** Each `BLOB_DATA` chunk costs one header entry on top of its payload entries, and the blob as a whole costs one `BLOB_INDEX` entry. The more the free space is fragmented, the more chunks are needed and the more of these header entries are spent — this is precisely the difference between `Expected Entries` (ideal, minimum chunking) and `Actual Entries` (real, fragmented chunking), and it can push `Overhead [%]` well above 100 %.
* **A page needs at least two free entries to hold any chunk** (one header entry plus at least one payload entry). Free space that survives only as isolated single-entry gaps still counts towards `Available Entries`, yet no blob chunk can ever be placed there, so that space is effectively unusable for the blob.
* **The *first* chunk needs a minimum starting free run.** Unless the whole blob fits into the current page, NVS refuses to place the very first `BLOB_DATA` chunk on a page whose free var-data run is smaller than `CHUNK_MAX_SIZE / 10`. With the default `CHUNK_MAX_SIZE = ENTRY_SIZE × (ENTRY_COUNT − 1) = 32 × 125 = 4000` bytes, this threshold is `400` bytes — roughly 13 entries. Such a page is marked full and NVS looks for a page offering a larger free run; if none exists, the write fails with `ESP_ERR_NVS_NOT_ENOUGH_SPACE` *before any chunk is written*. A partition whose free space survives only as many small per-page gaps (for example the `rest_free = 4` target, i.e. ≈ 96 bytes of free var-data per page) therefore offers no page able to *start* a multi-page blob — even though those same gaps could still host the blob's *subsequent* chunks, and even though every one of their entries is counted in `Available Entries`.
* **One page is always held in reserve** for the power-loss-safe space-reclaim algorithm, and space reclaim only consolidates free entries within a single candidate page per call. There is no operation that gathers scattered free entries from many pages into one large contiguous run for a single write.

Put together, these effects mean the blob's real footprint (`Actual Entries`) can be far larger than its ideal footprint (`Expected Entries`), while the portion of `Available Entries` that a single blob can actually reach is smaller than the raw figure. When the fragmented free space can no longer absorb the chunked-and-metadata-inflated blob, the write fails with `ESP_ERR_NVS_NOT_ENOUGH_SPACE` — even though `Available Entries` on its own looked large enough. In the table below this shows up as rows where `Available Entries` ≥ `Expected Entries` yet `Actual Entries` still reads `FAIL`.

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

The second part reports, for every partition size / blob size combination and every per-page free-entry target, the ideal vs. real blob entry consumption in a single table:

```
...
I (1265) nvs_statistics_example: Starting NVS blob storage-overhead measurement...

+---------------+--------------------+---------------------------+-----------------------+----------------------+--------------------+----------------------+--------------+
|     Blob Size |     Partition Size |   Free Entries per Page   |     Available Entries |     Expected Entries |     Actual Entries |     Overhead Entries |     Overhead |
|               |                    +-----------+---------------+                       |                      |                    |                      |              |
|           [B] |                [k] | First [-] | Remaining [-] |                   [-] |                  [-] |                [-] |                  [-] |          [%] |
+---------------+--------------------+-----------+---------------+-----------------------+----------------------+--------------------+----------------------+--------------+
|           128 |                 16 |        14 |            11 |                    39 |                    6 |                  6 |                    0 |          0.0 |
|           128 |                 16 |        14 |             4 |                    18 |                    6 |                  6 |                    0 |          0.0 |
|           350 |                 16 |        14 |             7 |                    28 |                   13 |                 15 |                    2 |         15.4 |
|           416 |                 16 |        14 |             4 |                    18 |                   15 |                 18 |                    3 |         20.0 |
|           417 |                 16 |        14 |             4 |                    18 |                   16 |               FAIL |                    - |            - |
|           768 |                 32 |        14 |             7 |                    56 |                   26 |                 30 |                    4 |         15.4 |
|          1612 |                 64 |        14 |             4 |                    60 |                   53 |                 71 |                   18 |         34.0 |
...
+---------------+--------------------+-----------+---------------+-----------------------+----------------------+--------------------+----------------------+--------------+

I (9000) nvs_statistics_example: NVS blob storage-overhead measurement done.
I (9010) nvs_statistics_example: Returning from app_main().
...
```

> The exact numbers depend on the target, partition size, blob size and per-page free-entry target; the values above are illustrative. Note the following patterns:
>
> * `Overhead [%]` grows as fewer free entries are left per page (each additional chunk adds one metadata entry), so the same blob costs more on a heavily fragmented partition than on a lightly fragmented one.
> * The neighbouring `416 B` and `417 B` rows straddle the fit boundary. With only 4 free entries (≈ 96 bytes) left on every page except the first, the first page — with 14 free entries (≈ 416 bytes of free var-data) — is the only one whose free run clears the ~400-byte minimum-first-chunk threshold, so it is the only page on which a blob can start. The `416 B` blob fits entirely into that first chunk (consuming all 18 available entries), whereas a single extra payload byte in the `417 B` blob no longer fits and there is no further usable space, so the write reports `FAIL` — even though its ideal `Expected Entries` (16) is below the reported `Available Entries` (18). This is the "seems like enough space, but the write still fails" case explained above.

To reset NVS data, erase the contents of flash memory using `idf.py erase-flash`, then upload the program again as described above.
