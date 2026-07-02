| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Generic Partition over the Block Device Layer (BDL)

This example shows how to partition a raw block device **at run time** and mount
two different filesystems on the resulting slices, using only the generic
Block Device Layer (BDL) interface (`esp_blockdev`).

It combines three building blocks:

* a **whole-disk BDL** — a flash data partition (`esp_partition_get_blockdev()`)
  or an SD/eMMC card over SDMMC or SDSPI (`sdmmc_get_blockdev()`);
* the [`esp_ext_part_tables`](https://components.espressif.com/components/espressif/esp_ext_part_tables)
  managed component — to generate/write and read/parse an [**MBR**](https://en.wikipedia.org/wiki/Master_boot_record) partition table;
* the **generic-partition BDL** (`esp_blockdev_generic_partition_get()`) — to expose
  each MBR partition entry as its own BDL that maps a slice of the whole disk.

> Note: Putting an MBR partition table on a **SPI flash** data partition (the
> default medium) is mainly for demonstration — it needs no extra hardware and
> runs in QEMU/CI. In real projects flash is normally partitioned with the ESP-IDF
> partition table instead. MBR partitioning at run time makes practical sense for
> **removable/large media such as SD cards and eMMC**, where a standard, host-PC
> readable partition table is expected.

## When to use this example

- You need to partition a raw block device **at run time**, with a standard MBR
  table that a host PC can also read.
- You are working with removable or large media (SD card, eMMC) that is expected
  to carry its own partition table.
- You want several filesystems (FATFS and LittleFS here) on slices of one device,
  sharing the same `esp_blockdev` interface.

Use a different example when you need:

- FATFS over the Block Device Layer on a single wear-levelled flash partition →
  [fatfs/bdl_wl](../fatfs/bdl_wl/)
- the classic wear levelling + FATFS flow on SPI flash →
  [wear_levelling](../wear_levelling/)
- a plain FATFS SD card mount without run-time partitioning →
  [sd_card/sdmmc](../sd_card/sdmmc/) or [sd_card/sdspi](../sd_card/sdspi/)
- LittleFS on a single ESP-IDF partition → [littlefs](../littlefs/)

## BDL stack

```
         +--------------+  +--------------+
         |   LittleFS   |  |    FATFS     |   file systems (VFS)
         +--------------+  +--------------+  +-----------------------+
         |              |  |             <-- | WL BDL (if SPI flash) | 
         | generic-part |  +- - - - - - - +  +-----------------------+
         |     BDL      |  | generic-part |   esp_blockdev_generic_partition_get()
         |              |  |     BDL      |   (one slice per MBR partition entry)
  +------+--------------+--+--------------+
  | MBR  |        whole-disk BDL          |   esp_partition_get_blockdev() /
  +------+                                |   sdmmc_get_blockdev()
  +---------------------------------------+
  |     SPI flash partition / SD card     |   physical storage
  +---------------------------------------+
```

The whole disk is laid out as an MBR followed by the two data slices. Partition
starts use 4 KiB alignment on SPI flash and 1 MiB alignment on SD/eMMC, matching
the usual alignment used by PC partitioning tools.

For the 2 MiB SPI flash `storage` partition, the layout is:

```
  0x0     0x200             0x1000                        0x101000              disk end
  +-------+-----------------+-----------------------------+---------------------+-----+
  |  MBR  | pad (align gap) |  LittleFS slice (1 MiB,     |  FAT slice          |[tail]
  | 512 B |  ~3.5 KiB       |  MBR entry 0, type 0xC3)    |  (rest of disk,     | gap
  |       |  (unused)       |                             |  MBR entry 1,       |
  |       |                 |                             |  type 0x0C)         |
  +-------+-----------------+-----------------------------+---------------------+-----+
  ^-- reserved MBR block ---^                                                    ^
      (align_up(512, 4 KiB))              align_down() may drop a few KiB here --+
```

On SD/eMMC, FAT has a fixed size of 16 MiB to keep formatting time bounded.
The card must have at least 18 MiB for the alignment gap and both partitions:

```
  0x0     0x200             0x100000                     0x200000              0x1200000       disk end
  +-------+-----------------+----------------------------+---------------------+----------------+
  |  MBR  | pad (align gap) |  LittleFS slice (1 MiB,    |  FAT slice          | Unallocated    |
  | 512 B |  ~1 MiB - 512 B |  MBR entry 0, type 0xC3)   |  (16 MiB,           | remainder      |
  |       |  (unused)       |                            |  MBR entry 1,       |                |
  |       |                 |                            |  type 0x0C)         |                |
  +-------+-----------------+----------------------------+---------------------+----------------+
  ^-- reserved MBR block ---^
    (align_up(512, 1 MiB))
```

Notes on the gaps:

* **MBR padding**: the MBR is only 512 B, but the first block is reserved up to
  the selected alignment (`align_up(MBR_SIZE, align)`). This leaves ~3.5 KiB
  unused on SPI flash (`0x200`..`0x1000`) or ~1 MiB - 512 B on SD/eMMC
  (`0x200`..`0x100000`).
* **Between slices**: none — LittleFS is a whole number of aligned blocks, so the
  FAT slice starts immediately after it (`fat_start = lfs_start + lfs_size`).
* **SPI flash tail** (`[tail] gap`): `fat_size` is rounded *down* to the alignment, so if the
  disk size is not a multiple of the alignment, less than one alignment unit at
  the very end stays unused. For the 2 MiB flash partition the numbers divide
  evenly, so this gap is zero.
* **SD/eMMC remainder**: space after the fixed FAT slice (offset 18 MiB) is left
  unallocated. The example still overwrites the card's partition table and data;
  the smaller FAT slice does not make it safe to use a card containing valuable data.

Because every layer speaks the same BDL interface, the same
`esp_blockdev_generic_partition_get()` slice works with any bottom device, and the
FATFS / LittleFS integration code does not depend on the storage driver.

## What the example does

1. Creates a whole-disk BDL from the selected storage medium.
2. Computes a layout that reserves space for the MBR up to the selected partition
   alignment (4 KiB on SPI flash, 1 MiB on SD/eMMC), places a fixed 1 MiB LittleFS
   slice next, then a FAT slice of 16 MiB on SD/eMMC or the remaining space on SPI flash.
3. Builds an in-memory partition list, generates an MBR, and writes it to the
   whole disk with `esp_ext_part_list_bdl_write()`.
4. Reads the MBR back with `esp_ext_part_list_bdl_read()` and, for each entry,
   creates a generic-partition BDL and mounts the matching filesystem
   (formatting it on first run).
5. Writes and reads back a small file on each filesystem, then unmounts and
   releases every BDL handle.

## How to use example

### Choose the storage medium

Run `idf.py menuconfig` and open **Example Configuration → Whole-disk block device**:

* **SPI flash 'storage' data partition** (default): no extra hardware needed, also
  works in QEMU/CI.
* **SD/eMMC card**: uses a real card and overwrites its existing partition table and
  data. Pick the peripheral under **SD card host peripheral**:
  * **SDMMC host**: uses the default SDMMC slot. You can select the bus width (1 or
    4 lines) in the same menu.
  * **SD SPI**: accesses the card over the generic SPI bus. Set the MOSI/MISO/CLK/CS
    GPIOs in the same menu; the data bus is always 1-line.

  **SD power supply comes from internal LDO IO** defaults to enabled on supported
  targets, with LDO ID 4 on ESP32-P4 and 1 on ESP32-S31. Check your board schematic
  and adjust the ID, or disable this option if the card uses an external supply.
  > Warning: this overwrites the card's existing partition table and data.

### Build and flash

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

The managed components (`esp_ext_part_tables`, `joltwallet/littlefs`) are fetched
automatically by the IDF Component Manager from
[`main/idf_component.yml`](main/idf_component.yml).

## Example output

```
I (321) example: Whole disk: SPI flash data partition 'storage'
I (331) example: Whole disk BDL: disk_size=2097152, read_size=1, write_size=1, erase_size=4096
I (341) example: Layout: LittleFS = 1024 KiB (fixed), FAT = remainder of the disk
I (361) example: Writing MBR partition table to the whole disk
I (401) example: Reading MBR partition table back
I (411) example: Mountable partition 0: type=4, address=0x00001000, size=0x00100000
I (421) example: Mounting LittleFS on the LittleFS partition BDL
I (521) example: Writing '/littlefs/hello.txt'
I (611) example: Read back from /littlefs/hello.txt: 'Hello from LittleFS over a generic-partition BDL!'
I (621) example: Mountable partition 1: type=3, address=0x00101000, size=0x000ff000
I (631) example: Mounting FATFS on the FAT partition BDL
I (811) example: Writing '/fat/hello.txt'
I (951) example: Read back from /fat/hello.txt: 'Hello from FATFS over a generic-partition BDL!'
I (961) example: Unmounting FATFS
I (971) example: Unmounting LittleFS
I (981) example: Releasing whole-disk BDL
I (991) example: Done
```

## Notes

* An MBR holds at most **4 primary partition entries**, so this scheme supports up
  to 4 slices; this example uses 2 (FAT + LittleFS). `esp_mbr_generate()` keeps only
  the first 4 entries and logs a warning if the partition list is longer.
* When SPI flash is selected, the FAT partition BDL is wrapped in a wear-levelling
  BDL before FATFS is mounted. This layer handles flash erases and distributes
  writes across the partition. LittleFS is mounted directly on its partition BDL
  because it handles erase-before-write and wear levelling internally.
* `esp_ext_part_list_bdl_write()` writes the raw MBR sector without erasing first,
  so on flash-like devices (`erase_before_write` flag set) the example erases the
  MBR block beforehand.
* The MBR partition **type** byte drives the filesystem choice: FAT entries are
  mounted with FATFS and the LittleFS entry with LittleFS. The generator writes
  the raw MBR type bytes `0x0C` (FAT32 with LBA) and `0xC3` (LittleFS); the
  `type=` values printed on read-back (`3` for FAT32, `4` for LittleFS) are the
  `esp_ext_part_tables` enum (`esp_ext_part_type_known_t`), not the raw bytes.
* LittleFS has no standard MBR type, so `esp_ext_part_tables` uses a **custom
  `0xC3` "hack"**: `0xC3` = `0x83` (Linux-style filesystem) `| 0x40` (a flag meaning
  "the CHS field carries the LittleFS block size") `| 0x10` (hidden). Because the
  block size is smuggled into the entry's otherwise-unused CHS field, the example
  must supply it via the `extra` field together with the `ESP_EXT_PART_FLAG_EXTRA`
  flag (it passes the BDL `erase_size`, which LittleFS uses as its block size in
  classic mode). This is a non-standard convention, not something a PC OS will
  interpret as LittleFS.
* The example lets `esp_ext_part_tables` place the partitions automatically:
  each entry sets `ESP_EXT_PART_FLAG_AUTO_ADDRESS` (so the generator assigns an
  aligned start address after the previous entry and past the MBR sector).
  On SPI flash, the FAT entry additionally sets `ESP_EXT_PART_FLAG_FILL` with
  `size == 0` to fill the rest of the disk. On SD/eMMC, it sets a fixed 16 MiB size
  without `ESP_EXT_PART_FLAG_FILL`. The example explicitly sets `total_size`
  from the device geometry, enabling the generator's built-in overlap and
  "fits within the disk" checks. If `total_size` is left at zero,
  `esp_ext_part_list_bdl_write()` supplies it from the device geometry instead.
* The example requests `ESP_EXT_PART_ALIGN_4KiB` for SPI flash and
  `ESP_EXT_PART_ALIGN_1MiB` for SD/eMMC when generating the MBR. Both alignments
  satisfy the underlying media's BDL erase-alignment requirement.
* On the **first** run the data area is empty, so both filesystems fail to mount
  and are formatted automatically (`format_if_mount_failed`). You will therefore
  see a few `W`/`E` log lines the first time, for example
  `esp_littlefs: ... Corrupted dir pair` followed by `mount failed ... formatting`.
  Subsequent runs mount the existing filesystems directly.
* FATFS probes the block device with an `ioctl` that the flash whole-disk BDL does
  not implement, so a harmless `esp_blockdev/generic_partition: ... Parent device
  does not implement ioctl` error may be logged; the mount still succeeds.
