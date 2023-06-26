| Supported Targets | Linux |
| ----------------- | ----- |

# NVS Page Unit Test

This unit test for the `nvs::Page` C++ class uses CMock to abstract its dependency to the `spi_flash` component. Its `CMakeLists.txt` builds mocks instead of the actual `spi_flash` component by setting the component property `USE_MOCK` for `spi_flash`. It also needs some stubs for CRC32 since the ROM component can not be mocked yet. These are retrieved from the `mock` directory inside `nvs_flash`.

The mocked version of `spi_flash` receives all function calls to the `esp_partition` API. E.g., have a look at the first unit test in the [NVS page test](main/nvs_page_test.cpp), the test function `test_Page_load_reading_header_fails()`. It simply checks whether an error is propagated correctly from the `esp_partition` API to upper layers during a call to `nvs::Page::load`. The relevant mocking code here is:

```c
esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_ARG);
```

It tells the generated mocks to expect a call to `esp_partition` API's `esp_partition_read_raw()`, accept any arguments and return `ESP_ERR_INVALID_ARG`. When `nvs::Page::load` is called later, the mock will "replay" this action and the test will check that the return value is correctly propagated with:

```c
TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, page.load(&mock, 0));
```

For more information on how to control the mock's behavior, take a look at the [CMock README](../../../cmock/CMock/docs/CMock_Summary.md). The rest of `main/nvs_page_test.cpp` also provides many more examples on how to control the mocks.

## Requirements

* The usual IDF requirements
* The host's gcc/g++
* `ruby`

This application has been tested on Ubuntu 20.04 with `gcc` version *9.3.0*.

## Build

First, make sure that the target is set to Linux. Run `idf.py --preview set-target linux` if you are not sure. Then do a normal IDF build: `idf.py build`.

## Run

```bash
idf.py monitor
```

## Coverage

To generate the coverage, run: `idf.py coverage`. Afterwards, you can view the coverage by opening `build/coverage_report/index.html` with your browser. Note that you need to run the application at least once before generating the coverage information. If you run it multiple times, the coverage information adds up.

## Example Output

Ideally, all tests pass, which is indicated by the last two log lines after the dashed line:

```bash
$ idf.py monitor
../main/nvs_page_test.cpp:880:test_Page_load_reading_header_fails:PASS
../main/nvs_page_test.cpp:881:test_Page_load_reading_data_fails:PASS
../main/nvs_page_test.cpp:882:test_Page_load__uninitialized_page_has_0xfe:PASS
../main/nvs_page_test.cpp:883:test_Page_load__initialized_corrupt_header:PASS
../main/nvs_page_test.cpp:884:test_Page_load_success:PASS
../main/nvs_page_test.cpp:885:test_Page_load_full_page:PASS
../main/nvs_page_test.cpp:886:test_Page_load__seq_number_0:PASS
../main/nvs_page_test.cpp:887:test_Page_erase__write_fail:PASS
../main/nvs_page_test.cpp:888:test_Page_erase__success:PASS
../main/nvs_page_test.cpp:889:test_Page_write__initialize_write_failure:PASS
../main/nvs_page_test.cpp:890:test_Page_write__write_data_fails:PASS
../main/nvs_page_test.cpp:891:test_page_write__write_correct_entry_state:PASS
../main/nvs_page_test.cpp:892:test_Page_write__write_correct_data:PASS
../main/nvs_page_test.cpp:893:test_Page_readItem__read_entry_fails:PASS
../main/nvs_page_test.cpp:894:test_Page_readItem__read_corrupted_entry:PASS
../main/nvs_page_test.cpp:895:test_Page_readItem__read_corrupted_second_read_fail:PASS
../main/nvs_page_test.cpp:896:test_Page_readItem__read_corrupted_erase_fail:PASS
../main/nvs_page_test.cpp:897:test_Page_readItem__read_entry_suceeds:PASS
../main/nvs_page_test.cpp:898:test_Page_readItem__blob_read_data_fails:PASS
../main/nvs_page_test.cpp:899:test_Page_readItem__blob_corrupt_data:PASS
../main/nvs_page_test.cpp:900:test_Page_readItem__blob_read_entry_suceeds:PASS
../main/nvs_page_test.cpp:901:test_Page_cmp__uninitialized:PASS
../main/nvs_page_test.cpp:902:test_Page_cmp__item_not_found:PASS
../main/nvs_page_test.cpp:903:test_Page_cmp__item_type_mismatch:PASS
../main/nvs_page_test.cpp:904:test_Page_cmp__item_content_mismatch:PASS
../main/nvs_page_test.cpp:905:test_Page_cmp__item_content_match:PASS
../main/nvs_page_test.cpp:906:test_Page_cmpItem__blob_data_mismatch:PASS
../main/nvs_page_test.cpp:907:test_Page_cmpItem__blob_data_match:PASS
../main/nvs_page_test.cpp:908:test_Page_eraseItem__uninitialized:PASS
../main/nvs_page_test.cpp:909:test_Page_eraseItem__key_not_found:PASS
../main/nvs_page_test.cpp:910:test_Page_eraseItem__write_fail:PASS
../main/nvs_page_test.cpp:911:test_Page_readItem__corrupt_data_erase_failure:PASS
../main/nvs_page_test.cpp:912:test_Page_eraseItem__write_succeed:PASS
../main/nvs_page_test.cpp:913:test_Page_findItem__uninitialized:PASS
../main/nvs_page_test.cpp:914:test_Page_find__wrong_ns:PASS
../main/nvs_page_test.cpp:915:test_Page_find__wrong_type:PASS
../main/nvs_page_test.cpp:916:test_Page_find__key_empty:PASS
../main/nvs_page_test.cpp:917:test_Page_find__wrong_key:PASS
../main/nvs_page_test.cpp:918:test_Page_find__too_large_index:PASS
../main/nvs_page_test.cpp:919:test_Page_findItem__without_read:PASS
../main/nvs_page_test.cpp:920:test_Page_markFull__wrong_state:PASS
../main/nvs_page_test.cpp:921:test_Page_markFreeing__wrong_state:PASS
../main/nvs_page_test.cpp:922:test_Page_markFull__success:PASS
../main/nvs_page_test.cpp:923:test_Page_markFreeing__success:PASS
../main/nvs_page_test.cpp:924:test_Page_markFull__write_fail:PASS
../main/nvs_page_test.cpp:925:test_Page_getVarDataTailroom__uninitialized_page:PASS
../main/nvs_page_test.cpp:926:test_Page_getVarDataTailroom__success:PASS
../main/nvs_page_test.cpp:927:test_Page_calcEntries__uninit:PASS
../main/nvs_page_test.cpp:928:test_Page_calcEntries__corrupt:PASS
../main/nvs_page_test.cpp:929:test_Page_calcEntries__active_wo_blob:PASS
../main/nvs_page_test.cpp:930:test_Page_calcEntries__active_with_blob:PASS
../main/nvs_page_test.cpp:931:test_Page_calcEntries__invalid:PASS

-----------------------
52 Tests 0 Failures 0 Ignored 
OK
```
