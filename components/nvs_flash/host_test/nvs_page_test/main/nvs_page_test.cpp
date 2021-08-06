// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include "unity.h"
#include "test_fixtures.hpp"

extern "C" {
#include "Mockesp_partition.h"
}

using namespace std;
using namespace nvs;

void test_Page_load_reading_header_fails()
{
    PartitionMock mock(0, 4096);
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_ARG);
    Page page;

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, page.load(&mock, 0));

    Mockesp_partition_Verify();
}

void test_Page_load_reading_data_fails()
{
    uint8_t header[64];
    std::fill_n(header, sizeof(header)/sizeof(header[0]), UINT8_MAX);
    PartitionMock mock(0, 4096);
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(header, 32);
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_FAIL);
    Page page;

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
    TEST_ASSERT_EQUAL(ESP_FAIL, page.load(&mock, 0));

    Mockesp_partition_Verify();
}

void test_Page_load__uninitialized_page_has_0xfe()
{
    PartitionMockFixture fix;
    Page page;

    fix.raw_header[511] = 0xfe;
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(fix.raw_header, 32);

    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(fix.raw_header, 512);

    // Page::load() should return ESP_OK, but state has to be corrupt
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part_mock, 0));

    TEST_ASSERT_EQUAL(Page::PageState::CORRUPT, page.state());

    Mockesp_partition_Verify();
}

void test_Page_load__initialized_corrupt_header()
{
    PartitionMockFixture fix;
    Page page;

    uint8_t raw_header_corrupt [] = {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x16, 0xdd, 0xdc};

    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header_corrupt, 32);

    // Page::load() should return ESP_OK, but state has to be corrupt
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part_mock, 0));

    TEST_ASSERT_EQUAL(Page::PageState::CORRUPT, page.state());

    Mockesp_partition_Verify();
}

void test_Page_load__corrupt_entry_table()
{
    PartitionMockFixture fix;

    // valid header
    uint8_t raw_header_valid [32] = {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc2, 0x16, 0xdd, 0xdc};

    // entry table with one entry
    uint8_t raw_entry_table [32];

    uint8_t ns_entry [32] = {0x00, 0x01, 0x01, 0xff, 0x68, 0xc5, 0x3f, 0x0b, 't', 'e', 's', 't', '_', 'n', 's', '\0',
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    uint8_t raw_header[4] = {0xff, 0xff, 0xff, 0xff};
    std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0);
    raw_entry_table[0] = 0xfa;

    // read page header
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header_valid, 32);

    // read entry table
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_entry_table, 32);

    // read next free entry's header
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header, 4);

    // read namespace entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

    // we expect a raw word write from the partition in order to change the entry bits to erased (0)
    esp_partition_write_raw_ExpectAndReturn(&fix.part_mock.partition, 32, nullptr, 4, ESP_OK);
    esp_partition_write_raw_IgnoreArg_src();

    // corrupt entry table as well as crc of corresponding item
    raw_entry_table[0] = 0xf6;

    Page page;

    // Page::load() should return ESP_OK, but state has to be corrupt
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part_mock, 0));

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, page.state());
    TEST_ASSERT_EQUAL(1, page.getUsedEntryCount());

    Mockesp_partition_Verify();
}

void test_Page_load_success()
{
    PartitionMockFixture fix;
    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(fix.raw_header, 32);
    for (int i = 0; i < 8; i++) {
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(fix.raw_header, 512);
    }
    Page page;

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part_mock, 0));
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, page.state());
}

void test_Page_load_full_page()
{
    NVSFullPageFixture fix(0, 1, NVS_DEFAULT_PART_NAME, false);

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
    TEST_ASSERT_EQUAL(ESP_OK, fix.page.load(&fix.part_mock, 0));
    TEST_ASSERT_EQUAL(Page::PageState::FULL, fix.page.state());
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

    esp_partition_erase_range_ExpectAndReturn(&fix.part_mock.partition, 0, 4096, ESP_FAIL);

    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.erase());
    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
}

void test_Page_erase__success()
{
    NVSValidPageFixture fix;

    esp_partition_erase_range_ExpectAndReturn(&fix.part_mock.partition, 0, 4096, ESP_OK);

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.erase());
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());
}

void test_Page_write__initialize_write_failure()
{
    PartitionMockFixture fix;
    uint8_t write_data = 47;

    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(fix.raw_header, 32);
    for (int i = 0; i < 8; i++) {
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(fix.raw_header, 512);
    }
    esp_partition_write_raw_ExpectAnyArgsAndReturn(ESP_FAIL);

    Page page;

    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part_mock, 0));
    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, page.state());

    TEST_ASSERT_EQUAL(ESP_FAIL, page.writeItem(1, nvs::ItemType::U8, "test", &write_data, sizeof(write_data)));
    TEST_ASSERT_EQUAL(Page::PageState::INVALID, page.state());
}

void test_Page_write__write_data_fails()
{
    NVSPageFixture fix;
    uint8_t write_data = 47;
    esp_partition_write_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_write_ExpectAnyArgsAndReturn(ESP_FAIL);

    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());

    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.writeItem(1, nvs::ItemType::U8, "test", &write_data, sizeof(write_data)));

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

    // initialize page
    esp_partition_write_raw_ExpectAnyArgsAndReturn(ESP_OK);

    // write entry
    esp_partition_write_ExpectAnyArgsAndReturn(ESP_OK);

    // write entry state
    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, 1, 32, raw_result, 4, 4, ESP_OK);

    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.writeItem(1, nvs::ItemType::U8, "test_key", &write_data, sizeof(write_data)));

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_write__write_correct_data()
{
    NVSPageFixture fix;
    uint8_t write_data = 47;
    uint8_t raw_result [32] = {0x01, 0x01, 0x01, 0xff, 0x98, 0x6f, 0x21, 0xfd, 't', 'e', 's', 't', '_', 'k', 'e', 'y',
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    // initialize page
    esp_partition_write_raw_ExpectAnyArgsAndReturn(ESP_OK);

    // write entry
    esp_partition_write_ExpectWithArrayAndReturn(&fix.part_mock.partition, 1, 64, raw_result, 32, 32, ESP_OK);

    // write entry state
    esp_partition_write_raw_ExpectAnyArgsAndReturn(ESP_OK);

    TEST_ASSERT_EQUAL(Page::PageState::UNINITIALIZED, fix.page.state());

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.writeItem(1, nvs::ItemType::U8, "test_key", &write_data, sizeof(write_data)));

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());
}

void test_Page_readItem__read_entry_fails()
{
    NVSValidPageFixture fix;

    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    uint8_t read_value = 0;

    esp_partition_read_ExpectAnyArgsAndReturn(ESP_FAIL);

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

    // corrupting entry
    fix.value_entry[0] = 0x0;

    // first read the entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // erasing entry by setting bit in entry table (0xfa -> 0xf2)
    uint8_t raw_result [4] = {0xf2, 0x00, 0x00, 0x00};
    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, 1, 32, raw_result, 4, 4, ESP_OK);

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, fix.page.readItem(NVSValidPageFixture::NS_INDEX, "test_value", read_value));

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

    // first read the entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_FAIL);

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

    // first read the entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // erasing entry by setting bit in entry table (0xfa -> 0xf2)
    uint8_t raw_result [4] = {0xf2, 0x00, 0x00, 0x00};
    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, 1, 32, raw_result, 4, 4, ESP_FAIL);

    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.readItem(NVSValidPageFixture::NS_INDEX, "test_value", read_value));

    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
}

void test_Page_readItem__read_entry_suceeds()
{
    NVSValidPageFixture fix;
    TEST_ASSERT_EQUAL(2, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(122, fix.page.getErasedEntryCount());

    TEST_ASSERT_EQUAL(Page::PageState::ACTIVE, fix.page.state());

    uint8_t read_value = 0;

    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

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

    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_FAIL);

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

    fix.blob_data[16] = 0xdf;
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_FAIL);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);

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

    fix.blob_data[16] = 0xdf;
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);

    // erasing entry by setting bit in entry table (0xfa -> 0xf2)
    uint8_t raw_result [4] = {0xa2, 0xff, 0xff, 0xff};
    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, 1, 32, raw_result, 4, 4, ESP_OK);

    esp_partition_erase_range_ExpectAndReturn(&fix.part_mock.partition, 96, 64, ESP_OK);

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, fix.page.readItem(NVSValidPageFixture::NS_INDEX,
            ItemType::BLOB_DATA,
            "test_blob",
            read_data,
            32,
            chunk_start));

    TEST_ASSERT_EQUAL(3, fix.page.getUsedEntryCount());
    TEST_ASSERT_EQUAL(1, fix.page.getErasedEntryCount());

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

    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);

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

    // no expectations here since comparison uses the item hash list

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, fix.page.cmpItem(uint8_t(1), "different", 47));
}

void test_Page_cmp__item_type_mismatch()
{
    NVSValidPageFixture fix;

    // read normal entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_TYPE_MISMATCH, fix.page.cmpItem(uint8_t(1), "test_value", int(47)));
}

void test_Page_cmp__item_content_mismatch()
{
    NVSValidPageFixture fix;

    // read normal entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_CONTENT_DIFFERS, fix.page.cmpItem(uint8_t(1), "test_value", uint8_t(46)));
}

void test_Page_cmp__item_content_match()
{
    NVSValidPageFixture fix;

    // read normal entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.cmpItem(NVSValidPageFixture::NS_INDEX, "test_value", uint8_t(47)));
}

void test_Page_cmpItem__blob_data_mismatch()
{
    NVSValidBlobPageFixture fix;

    // read blob entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);

    // read blob data
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);


    uint8_t blob_data_different [] =
           {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef,
            0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee};

    TEST_ASSERT_EQUAL(ESP_ERR_NVS_CONTENT_DIFFERS,
            fix.page.cmpItem(uint8_t(1),
            ItemType::BLOB_DATA,
            "test_blob",
            blob_data_different,
            32));
}

void test_Page_cmpItem__blob_data_match()
{
    NVSValidBlobPageFixture fix;

    // read blob entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_entry, 32);

    // read blob data
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.blob_data, fix.BLOB_DATA_SIZE);


    uint8_t blob_data_same [] =
           {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef,
            0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef};

    TEST_ASSERT_EQUAL(ESP_OK,
            fix.page.cmpItem(NVSValidPageFixture::NS_INDEX,
            ItemType::BLOB_DATA,
            "test_blob",
            blob_data_same,
            32));
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

    // first read the entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // erasing entry by setting bit in entry table (0xfa -> 0xf2)
    uint8_t raw_result [4] = {0xf2, 0x00, 0x00, 0x00};
    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, 1, 32, raw_result, 4, 4, ESP_FAIL);

    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.eraseItem<uint8_t>(NVSValidPageFixture::NS_INDEX, "test_value"));

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

    // first read the entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // Page::eraseEntryAndSpan() reads entry again
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

    // erasing entry by setting bit in entry table (0xfa -> 0xf2)
    uint8_t raw_result [4] = {0xf2, 0x00, 0x00, 0x00};
    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, 1, 32, raw_result, 4, 4, ESP_OK);

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

    // read normal entry
    esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_ReturnArrayThruPtr_dst(fix.value_entry, 32);

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
    Page::PageState expected_state = Page::PageState::FULL;

    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, sizeof(fix.part_mock.partition), 0, &expected_state, sizeof(expected_state), 4, ESP_OK);

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.markFull());
    TEST_ASSERT_EQUAL(Page::PageState::FULL, fix.page.state());
}

void test_Page_markFull__write_fail()
{
    NVSValidPageFixture fix;
    Page::PageState expected_state = Page::PageState::FREEING;

    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, sizeof(fix.part_mock.partition), 0, &expected_state, sizeof(expected_state), 4, ESP_FAIL);

    TEST_ASSERT_EQUAL(ESP_FAIL, fix.page.markFreeing());
    TEST_ASSERT_EQUAL(Page::PageState::INVALID, fix.page.state());
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
    Page::PageState expected_state = Page::PageState::FREEING;

    esp_partition_write_raw_ExpectWithArrayAndReturn(&fix.part_mock.partition, sizeof(fix.part_mock.partition), 0, &expected_state, sizeof(expected_state), 4, ESP_OK);

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.markFreeing());
    TEST_ASSERT_EQUAL(Page::PageState::FREEING, fix.page.state());
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

    nvs_stats_t nvsStats = {0, 0, 0, 0};

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(0, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

void test_Page_calcEntries__corrupt()
{
    PartitionMockFixture fix;
    Page page;

    uint8_t raw_header_corrupt [] = {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x16, 0xdd, 0xdc};

    esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
    esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header_corrupt, 32);

    // Page::load() should return ESP_OK, but state has to be corrupt
    TEST_ASSERT_EQUAL(ESP_OK, page.load(&fix.part_mock, 0));

    TEST_ASSERT_EQUAL(Page::PageState::CORRUPT, page.state());

    nvs_stats_t nvsStats = {0, 0, 0, 0};

    TEST_ASSERT_EQUAL(ESP_OK, page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(0, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

void test_Page_calcEntries__active_wo_blob()
{
    NVSValidPageFixture fix;

    nvs_stats_t nvsStats = {0, 0, 0, 0};

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(2, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(124, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

void test_Page_calcEntries__active_with_blob()
{
    NVSValidBlobPageFixture fix;

    nvs_stats_t nvsStats = {0, 0, 0, 0};

    TEST_ASSERT_EQUAL(ESP_OK, fix.page.calcEntries(nvsStats));
    TEST_ASSERT_EQUAL(4, nvsStats.used_entries);
    TEST_ASSERT_EQUAL(122, nvsStats.free_entries);
    TEST_ASSERT_EQUAL(Page::ENTRY_COUNT, nvsStats.total_entries);
    TEST_ASSERT_EQUAL(0, nvsStats.namespace_count);
}

void test_Page_calcEntries__invalid()
{
    Page page;

    nvs_stats_t nvsStats = {0, 0, 0, 0};

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
    UNITY_BEGIN();
    RUN_TEST(test_Page_load_reading_header_fails);
    RUN_TEST(test_Page_load_reading_data_fails);
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
    RUN_TEST(test_Page_readItem__read_entry_fails);
    RUN_TEST(test_Page_readItem__read_corrupted_entry);
    RUN_TEST(test_Page_readItem__read_corrupted_second_read_fail);
    RUN_TEST(test_Page_readItem__read_corrupted_erase_fail);
    RUN_TEST(test_Page_readItem__read_entry_suceeds);
    RUN_TEST(test_Page_readItem__blob_read_data_fails);
    RUN_TEST(test_Page_readItem__blob_corrupt_data);
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
    RUN_TEST(test_Page_readItem__corrupt_data_erase_failure);
    RUN_TEST(test_Page_eraseItem__write_succeed);
    RUN_TEST(test_Page_findItem__uninitialized);
    RUN_TEST(test_Page_find__wrong_ns);
    RUN_TEST(test_Page_find__wrong_type);
    RUN_TEST(test_Page_find__key_empty);
    RUN_TEST(test_Page_find__wrong_key);
    RUN_TEST(test_Page_find__too_large_index);
    RUN_TEST(test_Page_findItem__without_read);
    RUN_TEST(test_Page_markFull__wrong_state);
    RUN_TEST(test_Page_markFreeing__wrong_state);
    RUN_TEST(test_Page_markFull__success);
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
