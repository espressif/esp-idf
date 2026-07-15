| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# Cache Access Counters Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to measure cache hit/miss statistics using the cache access counters present on chips with `SOC_CACHE_CNT_SUPPORTED`, via the `esp_cache_cnt.h` API (`esp_hw_support` component).

The API is chip-agnostic. Each chip defines a list of counter "units" — one unit is the set of counters observing one traffic stream, for example instruction fetches from core 0 into the L1 cache. The typical flow, which this example follows, is:

1. `esp_cache_cnt_start()` — clear and enable all counters.
2. Run the code to be measured.
3. `esp_cache_cnt_stop()` — disable the counters, so that reading out and reporting the results is not counted as well.
4. `esp_cache_cnt_dump(NULL)` — print a table of all counter values; or, for programmatic access, enumerate the units with `esp_cache_cnt_num_units()` / `esp_cache_cnt_get_unit_info()` and read them with `esp_cache_cnt_get()`.

For each unit, the hardware counts completed accesses, miss stall events, requester conflicts, and lines transferred to/from the next level of the memory hierarchy. The number of line fills is the true miss count, so the miss ratio of a unit is `line_fills / accesses` (available as `esp_cache_cnt_miss_ratio()`). See the `esp_cache_cnt.h` documentation for the exact semantics of each counter.

The example runs the same read loop over buffers with different sizes and placements, and prints the counter values after each run. The working set determines which level of the memory hierarchy serves the accesses, which is visible in the counter values. The set of workloads depends on the target.

## How to use example

### Hardware Required

Any development board with a supported chip. To run the PSRAM workloads, the board must have PSRAM.

### Build and Flash

```
idf.py set-target <target>
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

Output for the ESP32-P4 (a chip with a two-level cache hierarchy):

```
Workload: flash rodata, 128 KB x 10 passes
unit                     accesses   line fills   writebacks    conflicts  miss rate
l1-icache-core0            132724           45            -            0      0.03%
l1-icache-core1              2792           13            -            0      0.47%
l1-dcache-core0             20714        20481          307            7     98.88%
l1-dcache-core1               374           25            0        24606      6.68%
l2-cache-inst                   8            1            -            0     12.50%
l2-cache-data               40960         1025            0            0      2.50%

Workload: PSRAM, 192 KB x 20 passes (fits in L2)
unit                     accesses   line fills   writebacks    conflicts  miss rate
l1-icache-core0            379266           24            -            0      0.01%
l1-icache-core1                 0            0            -            0      0.00%
l1-dcache-core0             61674        60747          263            0     98.50%
l1-dcache-core1                 0            0            0            0      0.00%
l2-cache-inst                   3            1            -            0     33.33%
l2-cache-data              121880         1152         1152            0      0.95%

Workload: PSRAM, 2 MB x 5 passes (exceeds L2)
unit                     accesses   line fills   writebacks    conflicts  miss rate
l1-icache-core0           1107734            4            -            0      0.00%
l1-icache-core1              9302           25            -            0      0.27%
l1-dcache-core0            164746       163921          167          154     99.50%
l1-dcache-core1              1870          125            0        10600      6.68%
l2-cache-inst                   2            0            -            0      0.00%
l2-cache-data              327680        80384         2481            0     24.53%

Cache counters example done
```
