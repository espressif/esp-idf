/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
static const char* TAG = "nvs_page_host_test";

#include <stdio.h>
#include "unity.h"
#include "test_fixtures.hpp"
#include "esp_log.h"

using namespace std;
using namespace nvs;

void test_Page_load_reading_header_fails()
{
    PartitionEmulationFixture fix;
    /*
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_ARG);
    */

    /*
    At the moment we cannot simulate esp_partition_read_raw call failure
    */

    Page page;

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, page.load(&fix.part, 0));
}

void test_Page_load_reading_data_fails()
{
    uint8_t header[64];
    std::fill_n(header, sizeof(header)/sizeof(header[0]), UINT8_MAX);

    PartitionEmulationFixture fix;
    fix.erase_all();
    fix.write_raw(0, header, sizeof(header));

    /*
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(header, 32);
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_FAIL);
    */

    /*
    At the moment we cannot simulate esp_partition_read_raw call failure
    */

    Page page;

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
    TEST_ASSERT_EQUAL(ESP_FAIL, page.load(&fix.part, 0));
}

void test_Page_load__uninitialized_page_has_0xfe()
{
    PartitionEmulationFixture fix;
    Page page;

    uint8_t uninitialized_symbol = 0xfe;
    fix.write_raw(511, &uninitialized_symbol, sizeof(uninitialized_symbol));

    // Page::load() should return ESP_OK, but state has to be corrupt
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part, 0));

    TEST_ASSERT_EQUAL(Page::PageState::CORRUPT, page.state());
}

void test_Page_load__initialized_corrupt_header()
{
    PartitionEmulationFixture fix;
    Page page;

    uint8_t raw_header_corrupt [] = {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x16, 0xdd, 0xdc};

    fix.write_raw(0, raw_header_corrupt, sizeof(raw_header_corrupt));

    // Page::load() should return ESP_OK, but state has to be corrupt
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part, 0));

    TEST_ASSERT_EQUAL(Page::PageState::CORRUPT, page.state());
}

void test_Page_load__corrupt_entry_table()
{
    PartitionEmulationFixture fix;

    // valid header
    uint8_t raw_header_valid [32] = {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc2, 0x16, 0xdd, 0xdc};

    // entry table with one entry
    uint8_t raw_entry_table [32];

    uint8_t ns_entry [32] = {0x00, 0x01, 0x01, 0xff, 0x68, 0xc5, 0x3f, 0x0b, 't', 'e', 's', 't', '_', 'n', 's', '\0',
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    uint8_t raw_header[4] = {0xff, 0xff, 0xff, 0xff};

    std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0);
    // corrupt entry table as well as crc of corresponding item
    raw_entry_table[0] = 0xf6;

    // Expected sequence of data blocks read by page.load
    fix.write_raw(  0, raw_header_valid, sizeof(raw_header_valid));
    fix.write_raw( 32, raw_entry_table,  sizeof(raw_entry_table));
    fix.write_raw(128, raw_header,       sizeof(raw_header));
    fix.write_raw( 64, ns_entry,         sizeof(ns_entry));

    Page page;

    // Page::load() should return ESP_OK, but state has to be corrupt
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part, 0));

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, page.state());
    TEST_ASSERT_EQUAL(1, page.getUsedEntryCount());
}

void test_Page_load_success()
{
    PartitionEmulationFixture fix;

    const size_t nvs_page_size = 512;
    const size_t nvs_header_size = 32;

    uint8_t raw_header[nvs_page_size];

    std::fill_n(raw_header, sizeof(raw_header)/sizeof(raw_header[0]), UINT8_MAX);

    // Expected sequence of data blocks read by page.load
    fix.write_raw(0, raw_header, nvs_header_size);
    for (int i = 0; i < 8; i++) {
        fix.write_raw(i * nvs_page_size, raw_header, sizeof(raw_header));
    }

    Page page;

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part, 0));
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, page.state());
}

void test_Page_load_full_page()
{
    PartitionEmulationFixture fix;

    // valid header
    uint8_t raw_header_valid [32] = {0xfc, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa3, 0x48, 0x9f, 0x38};

    // entry table with just one entry free
    uint8_t raw_entry_table [32];

    // namespace entry
    uint8_t ns_entry [32] = {0x00, 0x01, 0x01, 0xff, 0x68, 0xc5, 0x3f, 0x0b, 't', 'e', 's', 't', '_', 'n', 's', '\0',
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    // one value entry
    uint8_t value_entry [32] = {0x01, 0x01, 0x01, 0xff, 0x3d, 0xf3, 0x99, 0xe5, 't', 'e', 's', 't', '_', 'v', 'a', 'l',
                'u', 'e', '\0', '\0', '\0', '\0', '\0', '\0', 47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    // entry_table with all elements deleted except the namespace entry written and the last entry free
    std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0);
    raw_entry_table[0] = 0x0a;
    raw_entry_table[31] = 0xFC;


    // Expected sequence of data blocks read by page.load
    fix.write_raw(  0, raw_header_valid, sizeof(raw_header_valid));
    fix.write_raw( 32, raw_entry_table,  sizeof(raw_entry_table));
    fix.write_raw( 64, ns_entry,         sizeof(ns_entry));
    fix.write_raw( 96, value_entry,      sizeof(value_entry));

    Page page;

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part, 0));
    TEST_ASSERT_EQUAL(Page::PageState::FULL, page.state());
}

void test_Page_load__seq_number_0()
{
    NVSValidPageFixture fix;

    uint32_t seq_num;
    fix.page.getSeqNumber(seq_num);
    TEST_ASSERT_EQUAL(0, seq_num);
}

void test_Page_erase__write_fail()
{
    NVSValidPageFixture fix;

    // fail at 1st attempt to call esp_partition_erase_range
    fix.fail_erase_at(1);

    TEST_ASSERT_EQUAL(ESP_ERR_FLASH_OP_FAIL, fix.page.erase());
    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
}

void test_Page_erase__success()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.erase());
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());
}

void test_Page_write__initialize_write_failure()
{
    PartitionEmulationFixture fix;

    // Make partition empty - uninitialized
    fix.erase_all();

    uint8_t write_data = 47;

    // Emulate failed attempt to write data to the flash after writeItem is called
    fix.fail_write_at(1);

    Page page;

    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part, 0));
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, page.state());

    TEST_ASSERT_EQUAL(ESP_ERR_FLASH_OP_FAIL, page.writeItem(1, nvs::ItemType::U8, "test", &write_data, sizeof(write_data)));
    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
}

void test_Page_write__write_data_fails()
{
    NVSPageFixture fix;

    uint8_t write_data = 47;

    // It is expected, that 2 write calls will be made
    // Emulate failed second attempt to write data to the flash after writeItem is called
    fix.fail_write_at(2);

    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());

    TEST_ASSERT_EQUAL(ESP_ERR_FLASH_OP_FAIL, fix.page.writeItem(1, nvs::ItemType::U8, "test", &write_data, sizeof(write_data)));

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
}

void test_page_write__write_correct_entry_state()
{
    NVSPageFixture fix;
    uint8_t write_data = 47;
    uint8_t raw_result [4];
    std::fill_n(raw_result, sizeof(raw_result)/sizeof(raw_result[0]), UINT8_MAX);
    // mark first entry as written
    raw_result[0] = 0xfe;

    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());
    TEST_ASSERT_EQUAL(ESP_OK, fix.page.writeItem(1, nvs::ItemType::U8, "test_key", &write_data, sizeof(write_data)));
    // Test whether partition memory space contains indication of first entry written
    TEST_ASSERT_EQUAL(ESP_OK, fix.compare_raw(32, raw_result, sizeof(raw_result)));
    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_write__write_correct_data()
{
    NVSPageFixture fix;
    uint8_t write_data = 47;
    uint8_t raw_result [32] = {0x01, 0x01, 0x01, 0xff, 0x98, 0x6f, 0x21, 0xfd, 't', 'e', 's', 't', '_', 'k', 'e', 'y',
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());
    TEST_ASSERT_EQUAL(ESP_OK, fix.page.writeItem(1, nvs::ItemType::U8, "test_key", &write_data, sizeof(write_data)));
    // Test whether partition memory space contains value entry
    TEST_ASSERT_EQUAL(ESP_OK, fix.compare_raw(64, raw_result, sizeof(raw_result)));
    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_readItem__read_entry_fails()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    uint8_t read_value = 0;
    /*
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_FAIL);
    */

    /*
    At the moment we do not have simulation of failed flash read
    */
    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.readItem(NVSValidPageFixture::NS_INDEX, "test_value", read_value));

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());

    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());
}

void test_Page_readItem__read_corrupted_entry()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    uint8_t read_value = 0;

    // prepare corrupted entry in flash memory
    uint8_t corrupted_value_entry = 0x0;
    fix.write_raw( 96, &corrupted_value_entry, sizeof(corrupted_value_entry));

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, fix.page.readItem(NVSValidPageFixture::NS_INDEX, "test_value", read_value));
    // check that repair mechanism of corrupted entry caused erasing entry by setting bit in entry table (0xfa -> 0xf2)
    uint8_t raw_result [4] = {0xf2, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL(ESP_OK, fix.compare_raw(32, raw_result, sizeof(raw_result)));
    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
    TEST_ASSERT_EQUAL(1, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(123, fix.page.getErasedEntryCount());
}

void test_Page_readItem__read_corrupted_second_read_fail()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    uint8_t read_value = 0;

    // corrupting entry
    fix.value_entry[0] = 0x0;
    fix.write_raw(96, fix.value_entry, sizeof(fix.value_entry));

    /*
    // first read the entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_FAIL);
    */

    /*
    It is expected that readItem will call esp_partition_read function twice
    We cannot simulate the second read attempt failure at the moment
    */

    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.readItem(NVSValidPageFixture::NS_INDEX, "test_value", read_value));

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
}

void test_Page_readItem__read_corrupted_erase_fail()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    uint8_t read_value = 0;

    // corrupting entry
    fix.value_entry[0] = 0x0;
    // prepare corrupt entry for reading
    fix.write_raw( 96, fix.value_entry, sizeof(fix.value_entry));

    // emulate write failure as nvs will try to invalidate the corupt entry
    // by setting bit in entry table (0xfa -> 0xf2)
    fix.fail_write_at(1);

    TEST_ASSERT_EQUAL(ESP_ERR_FLASH_OP_FAIL, fix.page.readItem(NVSValidPageFixture::NS_INDEX, "test_value", read_value));

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
}

void test_Page_readItem__read_entry_suceeds()
{
    NVSValidPageFixture fix;
    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    uint8_t read_value = 0;

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.readItem(NVSValidPageFixture::NS_INDEX, "test_value", read_value));

    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(47, read_value);

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_readItem__blob_read_data_fails()
{
    NVSValidBlobPageFixture fix;
    TEST_ASSERT_EQUAL(4, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(0, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    uint8_t chunk_start = 0;
    uint8_t read_data [32];

    /*
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_FAIL);
    */

    /*
    Disabled. For now, we are unable to simulate read attempt failures
    readItem will read from index 96 in first call and from offset 128 in the second call.
    The second call should fail
    */

    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.readItem(NVSValidPageFixture::NS_INDEX,
            ItemType::BLOB_DATA,
            "test_blob",
            read_data,
            32,
            chunk_start));

    TEST_ASSERT_EQUAL(4, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(0, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_readItem__corrupt_data_erase_failure()
{
    NVSValidBlobPageFixture fix;
    TEST_ASSERT_EQUAL(4, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(0, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    uint8_t chunk_start = 0;
    uint8_t read_data [32];

    // corupt the data
    fix.blob_data[16] = 0xdf;
    fix.write_raw(128, fix.blob_data, sizeof(fix.blob_data));

    /*
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_FAIL);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);
    */

    /*
    We cannot migrate the test case at the moment as we are not able to simulate read failure
    fix.page.readItem will:
    esp_partition_read at offset 96
    esp_partition_read at offset 128
    esp_partition_read at offset 96, which should fail
    */

    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.readItem(NVSValidPageFixture::NS_INDEX,
            ItemType::BLOB_DATA,
            "test_blob",
            read_data,
            32,
            chunk_start));

    TEST_ASSERT_EQUAL(4, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(0, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_readItem__blob_corrupt_data()
{
    NVSValidBlobPageFixture fix;
    TEST_ASSERT_EQUAL(4, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(0, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    uint8_t chunk_start = 0;
    uint8_t read_data [32];

    // damage blob data
    fix.blob_data[16] = 0xdf;
    fix.write_raw(128, fix.blob_data, sizeof(fix.blob_data));

    // Try to read blob with damaged blob_data
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, fix.page.readItem(NVSValidPageFixture::NS_INDEX,
            ItemType::BLOB_DATA,
            "test_blob",
            read_data,
            32,
            chunk_start));

    /*
    This test cannot be implemented same way as it was with CMock
    The fix.page.readItem will read the data using esp_partition_read from offsets in sequence
    96, 128, 96. Original implementation has provided different data to the read from offset 96, see fragment of original code below
    ...
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);
    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);
    ...

    Then the result was in deleting only one entry and subsequent getUsedEntryCount has
    returned 3 and getErasedEntryCount has returned 1

    Current implementation provides damaged data on index 128 and always provides same data on index 96
    It leads to the outlined expected erasing of blob entry as well as data of blob entry and thus
    getUsedEntryCount returns 2 and getErasedEntryCount returns 2
    */

    uint8_t raw_result [4] = {0x82, 0xff, 0xff, 0xff};
    TEST_ASSERT_EQUAL(ESP_OK, fix.compare_raw(32, raw_result, sizeof(raw_result)));

    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(2, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_readItem__blob_read_entry_suceeds()
{
    NVSValidBlobPageFixture fix;
    TEST_ASSERT_EQUAL(4, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(0, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    uint8_t chunk_start = 0;
    uint8_t read_data [32];

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.readItem(NVSValidPageFixture::NS_INDEX,
            ItemType::BLOB_DATA,
            "test_blob",
            read_data,
            32,
            chunk_start));

    TEST_ASSERT_EQUAL_MEMORY(fix.blob_data, read_data, fix.BLOB_DATA_SIZE);

    // make sure nothing was erased, i.e. checksums matched
    TEST_ASSERT_EQUAL(4, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(0, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_cmp__uninitialized()
{
    Page page;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_INVALID_STATE, page.cmpItem(uint8_t(1) , "test", 47));
}

void test_Page_cmp__item_not_found()
{
    NVSValidPageFixture fix;

    // comparison uses the item hash list
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, fix.page.cmpItem(uint8_t(1), "different", 47));
}

void test_Page_cmp__item_type_mismatch()
{
    NVSValidPageFixture fix;

    // read normal entry
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_TYPE_MISMATCH, fix.page.cmpItem(uint8_t(1), "test_value", int(47)));
}

void test_Page_cmp__item_content_mismatch()
{
    NVSValidPageFixture fix;

    // read normal entry
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_CONTENT_DIFFERS, fix.page.cmpItem(uint8_t(1), "test_value", uint8_t(46)));
}

void test_Page_cmp__item_content_match()
{
    NVSValidPageFixture fix;

    // read normal entry
    TEST_ASSERT_EQUAL(ESP_OK, fix.page.cmpItem(NVSValidPageFixture::NS_INDEX, "test_value", uint8_t(47)));
}

void test_Page_cmpItem__blob_data_mismatch()
{
    uint8_t chunk_start = 0;

    uint8_t blob_data_different [] =
           {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef,
            0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee};

    NVSValidBlobPageFixture fix;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_CONTENT_DIFFERS,
            fix.page.cmpItem(uint8_t(1),
            ItemType::BLOB_DATA,
            "test_blob",
            blob_data_different,
            32,
            chunk_start));
}

void test_Page_cmpItem__blob_data_match()
{
    uint8_t chunk_start = 0;

    uint8_t blob_data_same [] =
           {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef,
            0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef};

    NVSValidBlobPageFixture fix;

    TEST_ASSERT_EQUAL(ESP_OK,
            fix.page.cmpItem(NVSValidPageFixture::NS_INDEX,
            ItemType::BLOB_DATA,
            "test_blob",
            blob_data_same,
            32,
            chunk_start));
}

void test_Page_eraseItem__uninitialized()
{
    Page page;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, page.eraseItem<uint8_t>(NVSValidPageFixture::NS_INDEX, "test_value"));
}

void test_Page_eraseItem__key_not_found()
{
    NVSValidPageFixture fix;
    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, fix.page.eraseItem<uint8_t>(NVSValidPageFixture::NS_INDEX, "different"));

    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_eraseItem__write_fail()
{
    NVSValidPageFixture fix;
    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    // it is expected, that nvs will try to erasing entry by setting bit in entry table (0xfa -> 0xf2)
    // simulated write failure should fail the eraseItem call
    fix.fail_write_at(1);

    TEST_ASSERT_EQUAL(ESP_ERR_FLASH_OP_FAIL, fix.page.eraseItem<uint8_t>(NVSValidPageFixture::NS_INDEX, "test_value"));

    TEST_ASSERT_EQUAL(1, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(123, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
}

void test_Page_eraseItem__write_succeed()
{
    NVSValidPageFixture fix;
    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());
    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
    TEST_ASSERT_EQUAL(ESP_OK, fix.page.eraseItem<uint8_t>(NVSValidPageFixture::NS_INDEX, "test_value"));
    TEST_ASSERT_EQUAL(1, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(123, fix.page.getErasedEntryCount());
    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_findItem__uninitialized()
{
    Page page;

    size_t index = 0;
    Item item;
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND,
        page.findItem(NVSValidPageFixture::NS_INDEX, nvs::ItemType::U8, "test_value", index, item));
}

void test_Page_find__wrong_ns()
{
    NVSValidPageFixture fix;
    size_t index = 0;
    Item item;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND,
            fix.page.findItem(NVSValidPageFixture::NS_INDEX + 1, nvs::ItemType::U8, "test_value", index, item));
}

void test_Page_find__wrong_type()
{
    NVSValidPageFixture fix;
    size_t index = 0;
    Item item;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_TYPE_MISMATCH,
            fix.page.findItem(NVSValidPageFixture::NS_INDEX, nvs::ItemType::I8, "test_value", index, item));
}

void test_Page_find__key_empty()
{
    NVSValidPageFixture fix;
    size_t index = 0;
    Item item;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND,
            fix.page.findItem(NVSValidPageFixture::NS_INDEX, nvs::ItemType::U8, "", index, item));
}

void test_Page_find__wrong_key()
{
    NVSValidPageFixture fix;
    size_t index = 0;
    Item item;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND,
            fix.page.findItem(NVSValidPageFixture::NS_INDEX, nvs::ItemType::U8, "different", index, item));
}

void test_Page_find__too_large_index()
{
    NVSValidPageFixture fix;
    size_t index = 2;
    Item item;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND,
            fix.page.findItem(NVSValidPageFixture::NS_INDEX, nvs::ItemType::U8, "test_value", index, item));
}

void test_Page_findItem__without_read()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND,
            fix.page.findItem(NVSValidPageFixture::NS_INDEX, nvs::ItemType::U8, "different"));
}

void test_Page_markFull__wrong_state()
{
    NVSPageFixture fix;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_INVALID_STATE, fix.page.markFull());
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());
}

void test_Page_markFull__success()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.markFull());
    TEST_ASSERT_EQUAL(Page::PageState::FULL, fix.page.state());
}

void test_Page_markFreeing__wrong_state()
{
    NVSPageFixture fix;

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_INVALID_STATE, fix.page.markFreeing());
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());
}

void test_Page_markFreeing__success()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.markFreeing());
    TEST_ASSERT_EQUAL(Page::PageState::FREEING, fix.page.state());
}

void test_Page_markFull__write_fail()
{
    NVSValidPageFixture fix;

    // simulate failure during the page state update propagation to the flash
    fix.fail_write_at(1);

    TEST_ASSERT_EQUAL(ESP_ERR_FLASH_OP_FAIL, fix.page.markFreeing());
    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
}

void test_Page_getVarDataTailroom__uninitialized_page()
{
    NVSPageFixture fix;

    TEST_ASSERT_EQUAL(Page::CHUNK_MAX_SIZE, fix.page.getVarDataTailroom());
}

void test_Page_getVarDataTailroom__success()
{
    NVSValidPageFixture fix;

    // blob data item, written namespace item, written normal item: 3 items
    TEST_ASSERT_EQUAL((Page::ENTRY_COUNT - 3) * Page::ENTRY_SIZE, fix.page.getVarDataTailroom());
}

void test_Page_calcEntries__uninit()
{
    NVSPageFixture fix;
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());

    nvs_stats_t nvsStats = {0, 0, 0, 0, 0};

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(0, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

void test_Page_calcEntries__corrupt()
{
    uint8_t raw_header_corrupt [] = {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x16, 0xdd, 0xdc};

    PartitionEmulationFixture fix;
    Page page;

    // Prepare corrupted header at index 0
    fix.write_raw(0, raw_header_corrupt, sizeof(raw_header_corrupt));

    // Page::load() should return ESP_OK, but state has to be corrupt
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part, 0));

    TEST_ASSERT_EQUAL(Page::PageState::CORRUPT, page.state());

    nvs_stats_t nvsStats = {0, 0, 0, 0, 0};

    TEST_ASSERT_EQUAL(ESP_OK, page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(0, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

void test_Page_calcEntries__active_wo_blob()
{
    NVSValidPageFixture fix;

    nvs_stats_t nvsStats = {0, 0, 0, 0, 0};

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(2, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(124, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

void test_Page_calcEntries__active_with_blob()
{
    NVSValidBlobPageFixture fix;

    nvs_stats_t nvsStats = {0, 0, 0, 0, 0};

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(4, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(122, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

void test_Page_calcEntries__invalid()
{
    Page page;

    nvs_stats_t nvsStats = {0, 0, 0, 0, 0};

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());

    // total entries always get updated
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(0, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

int main(int argc, char **argv)
{
#define TEMPORARILY_DISABLED(x)

    UNITY_BEGIN();
    // TEMPORARILY_DISABLED function error codes - to be decided, how to emulate failing esp_partition layer
    TEMPORARILY_DISABLED(RUN_TEST(test_Page_load_reading_header_fails);)
    TEMPORARILY_DISABLED(RUN_TEST(test_Page_load_reading_data_fails);)
    RUN_TEST(test_Page_load__uninitialized_page_has_0xfe);
    RUN_TEST(test_Page_load__initialized_corrupt_header);
    RUN_TEST(test_Page_load__corrupt_entry_table);
    RUN_TEST(test_Page_load_success);
    RUN_TEST(test_Page_load_full_page);
    RUN_TEST(test_Page_load__seq_number_0);
    RUN_TEST(test_Page_erase__write_fail);
    RUN_TEST(test_Page_erase__success);
    RUN_TEST(test_Page_write__initialize_write_failure);
    RUN_TEST(test_Page_write__write_data_fails);
    RUN_TEST(test_page_write__write_correct_entry_state);
    RUN_TEST(test_Page_write__write_correct_data);
    TEMPORARILY_DISABLED(RUN_TEST(test_Page_readItem__read_entry_fails);)
    RUN_TEST(test_Page_readItem__read_corrupted_entry);
    TEMPORARILY_DISABLED(RUN_TEST(test_Page_readItem__read_corrupted_second_read_fail);)
    RUN_TEST(test_Page_readItem__read_corrupted_erase_fail);
    RUN_TEST(test_Page_readItem__read_entry_suceeds);
    TEMPORARILY_DISABLED(RUN_TEST(test_Page_readItem__blob_read_data_fails);)
    TEMPORARILY_DISABLED(RUN_TEST(test_Page_readItem__blob_corrupt_data);)
    RUN_TEST(test_Page_readItem__blob_read_entry_suceeds);
    RUN_TEST(test_Page_cmp__uninitialized);
    RUN_TEST(test_Page_cmp__item_not_found);
    RUN_TEST(test_Page_cmp__item_type_mismatch);
    RUN_TEST(test_Page_cmp__item_content_mismatch);
    RUN_TEST(test_Page_cmp__item_content_match);
    RUN_TEST(test_Page_cmpItem__blob_data_mismatch);
    RUN_TEST(test_Page_cmpItem__blob_data_match);
    RUN_TEST(test_Page_eraseItem__uninitialized);
    RUN_TEST(test_Page_eraseItem__key_not_found);
    RUN_TEST(test_Page_eraseItem__write_fail);
    TEMPORARILY_DISABLED(RUN_TEST(test_Page_readItem__corrupt_data_erase_failure);)
    RUN_TEST(test_Page_eraseItem__write_succeed);
    RUN_TEST(test_Page_findItem__uninitialized);
    RUN_TEST(test_Page_find__wrong_ns);
    RUN_TEST(test_Page_find__wrong_type);
    RUN_TEST(test_Page_find__key_empty);
    RUN_TEST(test_Page_find__wrong_key);
    RUN_TEST(test_Page_find__too_large_index);
    RUN_TEST(test_Page_findItem__without_read);
    RUN_TEST(test_Page_markFull__wrong_state);
    RUN_TEST(test_Page_markFull__success);
    RUN_TEST(test_Page_markFreeing__wrong_state);
    RUN_TEST(test_Page_markFreeing__success);
    RUN_TEST(test_Page_markFull__write_fail);
    RUN_TEST(test_Page_getVarDataTailroom__uninitialized_page);
    RUN_TEST(test_Page_getVarDataTailroom__success);
    RUN_TEST(test_Page_calcEntries__uninit);
    RUN_TEST(test_Page_calcEntries__corrupt);
    RUN_TEST(test_Page_calcEntries__active_wo_blob);
    RUN_TEST(test_Page_calcEntries__active_with_blob);
    RUN_TEST(test_Page_calcEntries__invalid);
    int failures = UNITY_END();
    return failures;
}
