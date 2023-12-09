| Supported Targets | ESP32 | ESP32-C3 |
| ----------------- | ----- | -------- |

# esp_hid unit tests

The unit tests are currently run only on the chips listed above just to save CI resources. If you are adding some tests which need to run on a different chip, update [.build-test-rules.yml](../.build-test-rules.yml), adding the chip you need.

When adding new test cases, check if the `depends_components` list in `.build-test-rules.yml` needs to be updated to include additional components. The test app will only be built and tested when these components are modified.

To build and run this test app, using esp32c3 target for example:

```bash
idf.py set-target esp32c3
idf.py build flash monitor
```

To run tests using pytest:

```bash
idf.py set-target esp32c3
idf.py build
pytest --target=esp32c3
```

### Code used to generate tests

```c

void dump_report_map(const uint8_t *data, size_t len){
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(data, len);
    printf("    TEST_ASSERT_NOT_NULL(report_map);\n");
    printf("    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_%s);\n", esp_hid_usage_str(report_map->usage));
    printf("    TEST_ASSERT(report_map->appearance == 0x%04X);\n", report_map->appearance);
    printf("    TEST_ASSERT(report_map->reports_len == %u);\n", report_map->reports_len);
    for(uint8_t i=0; i<report_map->reports_len; i++){
        printf("    TEST_ASSERT(report_map->reports[%u].report_id == %u);\n", i, report_map->reports[i].report_id);
        printf("    TEST_ASSERT(report_map->reports[%u].report_type == ESP_HID_REPORT_TYPE_%s);\n", i, esp_hid_report_type_str(report_map->reports[i].report_type));
        printf("    TEST_ASSERT(report_map->reports[%u].protocol_mode == ESP_HID_PROTOCOL_MODE_%s);\n", i, esp_hid_protocol_mode_str(report_map->reports[i].protocol_mode));
        printf("    TEST_ASSERT(report_map->reports[%u].usage == ESP_HID_USAGE_%s);\n", i, esp_hid_usage_str(report_map->reports[i].usage));
        printf("    TEST_ASSERT(report_map->reports[%u].value_len == %u);\n", i, report_map->reports[i].value_len);
    }
    printf("    esp_hid_free_report_map(report_map);\n");
}

#define _str(a) #a
#define xstr(a) _str(a)
#define TEST_DUMP(map) \
    printf("TEST_CASE(\"can parse " xstr(map) "\", \"[esp_hid]\")\n{\n"); \
    printf("    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(" xstr(map) ", sizeof(" xstr(map) "));\n"); \
    dump_report_map(map, sizeof(map)); \
    printf("}\n\n");

void generate_tests(){
    TEST_DUMP(hidReportMap);
    TEST_DUMP(relMouseReportMap);
    TEST_DUMP(absMouseReportMap);
    TEST_DUMP(keyboardReportMap);
    TEST_DUMP(joystickReportMap);
    TEST_DUMP(mediaReportMap);
    TEST_DUMP(mediaReportMap2);
    TEST_DUMP(hidapiReportMap);
}

```
