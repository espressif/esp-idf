/*
 * SPDX-FileCopyrightText: 2026 Spike (looxonline)
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstring>
#include <map>
#include <vector>

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"

#include "ff.h"
#include "diskio_impl.h"

namespace {

/** @brief Sparse synthetic disk with injectable failures; contains no captured user data. */
struct TestDisk {
    /**
     * @brief Get a writable sector, allocating zero-filled bytes when first accessed.
     * @param lba Logical sector address within this synthetic disk.
     * @return Mutable sector bytes, retained until the sector map is cleared.
     */
    std::vector<BYTE> &sector(uint32_t lba)
    {
        auto &bytes = sectors[lba];
        bytes.resize(sector_size);
        return bytes;
    }

    unsigned sector_size = 512;                       /**< Bytes per logical sector. */
    uint32_t sector_count = 131072;                    /**< Number of logical sectors, not the last LBA. */
    uint32_t failed_read = UINT32_MAX;                 /**< LBA to fail, or UINT32_MAX to disable failures. */
    DRESULT capacity_result = RES_OK;                 /**< Result injected into GET_SECTOR_COUNT. */
    unsigned writes = 0;                              /**< Attempted writes, including rejected ones. */
    unsigned capacity_queries = 0;                    /**< Number of GET_SECTOR_COUNT requests. */
    std::map<uint32_t, std::vector<BYTE>> sectors;      /**< Stored sectors; absent sectors read as zeros. */
    std::map<uint32_t, unsigned> reads;                /**< Number of reads attempted at each LBA. */
};

/** Active fixture for the disk callbacks; Catch2 executes these cases sequentially. */
static TestDisk *s_disk;

/**
 * @brief Store a 16-bit little-endian field in fixture metadata.
 * @param sector Destination sector, with at least offset + 2 bytes.
 * @param offset Byte offset of the field.
 * @param value Field value.
 */
static void put16(std::vector<BYTE> &sector, unsigned offset, uint16_t value)
{
    sector[offset] = value;
    sector[offset + 1] = value >> 8;
}

/**
 * @brief Store a 32-bit little-endian field in fixture metadata.
 * @param sector Destination sector, with at least offset + 4 bytes.
 * @param offset Byte offset of the field.
 * @param value Field value.
 */
static void put32(std::vector<BYTE> &sector, unsigned offset, uint32_t value)
{
    put16(sector, offset, value);
    put16(sector, offset + 2, value >> 16);
}

/**
 * @brief Report the synthetic disk as initialized and ready.
 *
 * All disk callbacks ignore the drive argument: both the active drive and any
 * reserved first slot belong to the same fixture. No hardware is accessed.
 * @return Zero (ready).
 */
static DSTATUS disk_ready(BYTE)
{
    return 0;
}

/**
 * @brief Read stored or zero-filled sectors while recording access and injecting failures.
 * @param buffer Destination with room for count sectors.
 * @param start First logical sector address.
 * @param count Number of logical sectors to read.
 * @return RES_OK, RES_PARERR for an out-of-bounds extent, or RES_ERROR for an injected failure.
 */
static DRESULT read_disk(BYTE, BYTE *buffer, uint32_t start, UINT count)
{
    if (start >= s_disk->sector_count || count > s_disk->sector_count - start) {
        return RES_PARERR;
    }
    for (UINT i = 0; i < count; ++i) {
        uint32_t lba = start + i;
        ++s_disk->reads[lba];
        if (lba == s_disk->failed_read) {
            return RES_ERROR;
        }
        auto entry = s_disk->sectors.find(lba);
        if (entry == s_disk->sectors.end()) {
            memset(buffer, 0, s_disk->sector_size);
        } else {
            memcpy(buffer, entry->second.data(), s_disk->sector_size);
        }
        buffer += s_disk->sector_size;
    }
    return RES_OK;
}

/**
 * @brief Count and reject writes, ignoring the payload and extent arguments.
 * @return RES_WRPRT without modifying the synthetic disk.
 */
static DRESULT write_disk(BYTE, const BYTE *, uint32_t, UINT)
{
    ++s_disk->writes;
    return RES_WRPRT;
}

/**
 * @brief Report the fixture geometry or inject a capacity-query failure.
 * @param command GET_SECTOR_SIZE or GET_SECTOR_COUNT.
 * @param buffer Output WORD for sector size, or LBA_t for sector count.
 * @return RES_OK, the injected capacity result, or RES_PARERR for unsupported commands.
 */
static DRESULT control_disk(BYTE, BYTE command, void *buffer)
{
    if (command == GET_SECTOR_SIZE) {
        *static_cast<WORD *>(buffer) = s_disk->sector_size;
        return RES_OK;
    }
    if (command == GET_SECTOR_COUNT) {
        ++s_disk->capacity_queries;
        *static_cast<LBA_t *>(buffer) = s_disk->sector_count;
        return s_disk->capacity_result;
    }
    return RES_PARERR;
}

/** FatFs disk operations, shared by the active and optionally reserved logical drive. */
static const ff_diskio_impl_t s_callbacks = {
    disk_ready, disk_ready, read_disk, write_disk, control_disk
};

/** @brief Own disk registration and mount cleanup for one sequential Catch2 fixture. */
class PartitionTest {
public:
    /**
     * @brief Register a synthetic disk with automatic partition selection.
     * @param reserve Reserve drive zero first to exercise allocation alongside another volume.
     */
    explicit PartitionTest(bool reserve = false) : reserve_first(reserve)
    {
        REQUIRE(s_disk == nullptr);
        s_disk = &disk;
        if (reserve_first) {
            BYTE first;
            REQUIRE(ff_diskio_get_drive(&first) == ESP_OK);
            REQUIRE(first == 0);
            ff_diskio_register(0, &s_callbacks);
        }
        REQUIRE(ff_diskio_get_drive(&drive) == ESP_OK);
        ff_diskio_register(drive, &s_callbacks);
        VolToPart[drive] = {drive, 0};
        path[0] = '0' + drive;
        path[1] = ':';
        path[2] = '/';
    }

    /** @brief Unmount, reset the mapping, release drive slots, and verify no writes occurred. */
    ~PartitionTest()
    {
        f_mount(nullptr, path, 0);
        VolToPart[drive] = {drive, 0};
        ff_diskio_unregister(drive);
        if (reserve_first) {
            ff_diskio_unregister(0);
        }
        CHECK(disk.writes == 0);
        s_disk = nullptr;
    }

    /**
     * @brief Write an MBR entry, allowing deliberately malformed fields for negative tests.
     * @param index Zero-based primary partition index, from 0 through 3.
     * @param type Partition type byte.
     * @param start First logical sector address.
     * @param count Partition length in logical sectors.
     * @param boot Boot indicator byte.
     */
    void partition(unsigned index, BYTE type, uint32_t start, uint32_t count, BYTE boot = 0)
    {
        auto &mbr = disk.sector(0);
        unsigned offset = 446 + index * 16;
        memset(mbr.data() + offset, 0, 16);
        mbr[offset] = boot;
        mbr[offset + 4] = type;
        put32(mbr, offset + 8, start);
        put32(mbr, offset + 12, count);
        put16(mbr, 510, 0xaa55);
    }

    /**
     * @brief Create minimal FAT metadata and one small text file without formatting or disk writes.
     *
     * Uses two FAT copies and one sector per cluster. The supplied volume size must
     * fit the selected FAT type; only sectors needed for mounting and reading are stored.
     * @param start Volume boot sector address.
     * @param count Total sectors in the volume, including metadata.
     * @param fat_bits FAT entry width: 12, 16, or 32.
     * @param name Eleven-byte, space-padded short filename for the root directory entry.
     * @return Logical sector address of the root directory.
     */
    uint32_t volume(uint32_t start, uint32_t count, unsigned fat_bits = 32, const char *name = "FOUND   TXT")
    {
        const unsigned size = disk.sector_size;
        const unsigned reserved = fat_bits == 32 ? 32 : 1;
        const unsigned root_sectors = fat_bits == 32 ? 0 : (512 * 32) / size;
        const unsigned fat_sectors = fat_bits == 32 ? (count - reserved + 2 + size / 4 + 1) / (size / 4 + 2)
                                     : ((count + 2) * fat_bits / 8 + size - 1) / size;
        const uint32_t root = start + reserved + 2 * fat_sectors;
        const uint32_t data = root + root_sectors;
        const uint32_t file_sector = fat_bits == 32 ? data + 1 : data;
        auto &boot = disk.sector(start);
        boot[0] = 0xeb;
        boot[1] = 0x58;
        boot[2] = 0x90;
        put16(boot, 11, size);
        boot[13] = 1;
        put16(boot, 14, reserved);
        boot[16] = 2;
        boot[21] = 0xf8;
        put32(boot, 28, start);
        put16(boot, 510, 0xaa55);
        if (fat_bits == 32) {
            put32(boot, 32, count);
            put32(boot, 36, fat_sectors);
            put32(boot, 44, 2);
            put16(boot, 48, 1);
            memcpy(boot.data() + 82, "FAT32   ", 8);
        } else {
            put16(boot, 17, 512);
            put16(boot, 19, count);
            put16(boot, 22, fat_sectors);
        }
        for (unsigned copy = 0; copy < 2; ++copy) {
            auto &fat = disk.sector(start + reserved + copy * fat_sectors);
            memset(fat.data(), 0xff, 16);
            fat[0] = 0xf8;
        }
        auto &dir = disk.sector(root);
        memcpy(dir.data(), name, 11);
        dir[11] = AM_ARC;
        put16(dir, 26, fat_bits == 32 ? 3 : 2);
        constexpr char text[] = "Lorem ipsum\n";
        put32(dir, 28, sizeof(text) - 1);
        memcpy(disk.sector(file_sector).data(), text, sizeof(text) - 1);
        return root;
    }

    /** @brief Verify the root contains only FOUND.TXT and read its expected complete contents. */
    void read_file()
    {
        FF_DIR dir = {};
        FILINFO info = {};
        REQUIRE(f_opendir(&dir, path) == FR_OK);
        REQUIRE(f_readdir(&dir, &info) == FR_OK);
        CHECK(strcmp(info.fname, "FOUND.TXT") == 0);
        REQUIRE(f_readdir(&dir, &info) == FR_OK);
        CHECK(info.fname[0] == '\0');
        REQUIRE(f_closedir(&dir) == FR_OK);
        char filename[] = "0:/FOUND.TXT";
        filename[0] = path[0];
        FIL file = {};
        REQUIRE(f_open(&file, filename, FA_READ) == FR_OK);
        char buffer[32] = {};
        UINT received;
        REQUIRE(f_read(&file, buffer, sizeof(buffer), &received) == FR_OK);
        CHECK(received == strlen("Lorem ipsum\n"));
        CHECK(strcmp(buffer, "Lorem ipsum\n") == 0);
        REQUIRE(f_close(&file) == FR_OK);
    }

    TestDisk disk;                     /**< Synthetic media owned by this fixture. */
    FATFS fs = {};                     /**< Filesystem object retained until unmount. */
    BYTE drive = FF_DRV_NOT_USED;       /**< Physical and logical drive allocated to this fixture. */
    char path[4] = {};                 /**< Null-terminated root path, such as 0:/. */
    bool reserve_first;                /**< Whether drive zero must also be released. */
};

} // namespace

TEST_CASE("FAT partition table takes precedence over a stale sector-zero VBR", "[fatfs][partition]")
{
    const unsigned sector_size = GENERATE(512, 4096);
    const bool reserve_first = GENERATE(false, true);
    PartitionTest test(reserve_first);
    test.disk.sector_size = sector_size;
    const uint32_t stale_root = test.volume(0, test.disk.sector_count, 32, "STALE   TXT");
    const uint32_t root = test.volume(32, test.disk.sector_count - 32);
    test.partition(0, 0x0c, 32, test.disk.sector_count - 32);
    REQUIRE(f_mount(&test.fs, test.path, 1) == FR_OK);
    CHECK(test.fs.volbase == 32);
    CHECK(test.fs.database == root);
    test.read_file();
    CHECK(test.disk.reads[stale_root] == 0);
    CHECK(test.disk.capacity_queries == 1);

    // Reinsertion of an unpartitioned volume on the same logical drive.
    REQUIRE(f_mount(nullptr, test.path, 0) == FR_OK);
    test.disk.sectors.clear();
    test.volume(0, test.disk.sector_count);
    REQUIRE(f_mount(&test.fs, test.path, 1) == FR_OK);
    CHECK(test.fs.volbase == 0);
    test.read_file();
    CHECK(VolToPart[test.drive].pt == 0);
}

TEST_CASE("Ordinary partitioned and whole-device FAT volumes still mount", "[fatfs][partition]")
{
    const unsigned fat_bits = GENERATE(12, 16, 32);
    const bool partitioned = GENERATE(false, true);
    PartitionTest test;
    const uint32_t count = fat_bits == 12 ? 3072 : fat_bits == 16 ? 32768 : 131040;
    const uint32_t start = partitioned ? 32 : 0;
    test.volume(start, count, fat_bits);
    if (partitioned) {
        test.partition(0, fat_bits == 12 ? 0x01 : fat_bits == 16 ? 0x06 : 0x0c, start, count, 0x80);
    } else {
        // Real FAT boot code is allowed in the bytes occupied by an MBR table.
        memset(test.disk.sector(0).data() + 446, 0xa5, 64);
    }
    REQUIRE(f_mount(&test.fs, test.path, 1) == FR_OK);
    CHECK(test.fs.volbase == start);
    test.read_file();
    CHECK(test.disk.capacity_queries == 0);
}

TEST_CASE("Only a coherent in-bounds MBR overrides a whole-device FAT VBR", "[fatfs][partition]")
{
    PartitionTest test;
    test.volume(0, test.disk.sector_count);
    test.partition(0, 0x0c, 32, 65536);
    SECTION("Invalid boot indicator") {
        test.disk.sector(0)[446] = 0x01;
    }
    SECTION("Zero start") {
        test.partition(0, 0x0c, 0, 65536);
    }
    SECTION("Zero size") {
        test.partition(0, 0x0c, 32, 0);
    }
    SECTION("Empty entry with nonzero size") {
        test.partition(0, 0, 32, 65536);
    }
    SECTION("Overlapping entries") {
        test.partition(1, 0x83, 1024, 2048);
    }
    SECTION("Wrapped last LBA") {
        test.partition(0, 0x0c, 0xfffffff0, 64);
    }
    SECTION("One sector beyond the device") {
        test.partition(0, 0x0c, 32, test.disk.sector_count - 31);
    }
    REQUIRE(f_mount(&test.fs, test.path, 1) == FR_OK);
    CHECK(test.fs.volbase == 0);
    test.read_file();
}

TEST_CASE("Stale VBR is not a fallback after identifying a valid MBR", "[fatfs][partition]")
{
    PartitionTest test;
    test.volume(0, test.disk.sector_count, 32, "STALE   TXT");
    test.volume(32, test.disk.sector_count - 32);
    test.partition(0, 0x0c, 32, test.disk.sector_count - 32);
    FRESULT expected = FR_DISK_ERR;
    SECTION("Sector zero read fails") {
        test.disk.failed_read = 0;
    }
    SECTION("Capacity query fails") {
        test.disk.capacity_result = RES_ERROR;
    }
    SECTION("Partition boot sector read fails") {
        test.disk.failed_read = 32;
    }
    SECTION("Partition has no filesystem") {
        test.disk.sector(32).assign(512, 0);
        expected = FR_NO_FILESYSTEM;
    }
    REQUIRE(f_mount(&test.fs, test.path, 1) == expected);
    CHECK(test.fs.fs_type == 0);

    // A failed mount must not prevent the same media from being mounted again.
    REQUIRE(f_mount(nullptr, test.path, 0) == FR_OK);
    test.disk.failed_read = UINT32_MAX;
    test.disk.capacity_result = RES_OK;
    test.volume(32, test.disk.sector_count - 32);
    REQUIRE(f_mount(&test.fs, test.path, 1) == FR_OK);
    CHECK(test.fs.volbase == 32);
    test.read_file();
}

TEST_CASE("Automatic and explicit selection across multiple primary partitions", "[fatfs][partition]")
{
    const bool explicit_partition = GENERATE(false, true);
    PartitionTest test;
    test.disk.sector_count = 262144;
    test.volume(0, test.disk.sector_count, 32, "STALE   TXT");
    test.partition(0, 0x83, 32, 16384); // No FAT filesystem in the first partition.
    test.partition(1, 0x0c, 20000, 100000, 0x80);
    test.partition(2, 0x0c, 130000, 100000);
    test.volume(20000, 100000);
    test.volume(130000, 100000);
    if (explicit_partition) {
        VolToPart[test.drive].pt = 3;
    }
    REQUIRE(f_mount(&test.fs, test.path, 1) == FR_OK);
    CHECK(test.fs.volbase == (explicit_partition ? 130000 : 20000));
    test.read_file();
}
