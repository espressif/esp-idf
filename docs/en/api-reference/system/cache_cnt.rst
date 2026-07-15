Cache Access Counters
=====================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

{IDF_TARGET_NAME} has hardware counters attached to the cache request buses. They record the number of completed cache accesses, miss stall events, requester conflicts, and cache lines transferred to and from the next level of the memory hierarchy. These counters can be used to measure the cache hit/miss behavior of a piece of code, for example to choose the placement of data in memory, or to find out why some code runs slower than expected.

Counter Units
-------------

The set of counters differs between chips: the number of cache levels, the number of request buses per cache, and which counters exist per bus all vary. Instead of a fixed list of caches, the API exposes a chip-defined list of counter *units*. Each unit is one set of counters observing one traffic stream, for example instruction fetches from core 0 into the L1 cache. Applications enumerate the units at runtime using :cpp:func:`esp_cache_cnt_num_units` and :cpp:func:`esp_cache_cnt_get_unit_info`, so they keep working when a new chip adds or removes units.

Usage
-----

1. Call :cpp:func:`esp_cache_cnt_start` to clear and enable all counters.
2. Run the code to be measured.
3. Call :cpp:func:`esp_cache_cnt_stop` to disable the counters, so that reading out and reporting the results is not counted as well.
4. Call :cpp:func:`esp_cache_cnt_dump` to print a table of all counter values, or read the values of individual units with :cpp:func:`esp_cache_cnt_get`.

:cpp:func:`esp_cache_cnt_clear` resets the counters without changing whether they are running, which is useful when the counters have to stay enabled across measurement phases.

Counter Semantics
-----------------

For each unit, :cpp:func:`esp_cache_cnt_get` returns:

- ``accesses``: the number of completed accesses. The hardware "hit" counter increments once for every access that completes, whether or not the access had to wait for a line fill first, so it is reported as the total access count.
- ``stall_events``: incremented repeatedly while an access is stalled on a miss. This value grows with the total miss latency, not with the number of missed accesses, so it is only useful as a relative measure.
- ``conflicts``: the number of conflicts between requesters on the cache.
- ``line_fills``: the number of lines fetched from the next level of the memory hierarchy. This is the true miss count.
- ``writebacks``: the number of lines written back to the next level. Only present for data traffic on chips with a write-back cache.

The miss ratio of a unit is therefore ``line_fills / accesses``, available as :cpp:func:`esp_cache_cnt_miss_ratio`. Not every counter exists for every unit; a field of :cpp:struct:`esp_cache_cnt_data_t` is only meaningful if the corresponding bit of the ``valid_mask`` member is set.

Application Examples
--------------------

- :example:`system/cache_counters` runs a read workload over working sets of different sizes and placements, and prints the counter values after each run, showing how the working set size determines which level of the memory hierarchy serves the accesses.

API Reference
-------------

.. include-build-file:: inc/esp_cache_cnt.inc
