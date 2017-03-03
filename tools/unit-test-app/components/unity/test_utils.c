// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "unity.h"
#include "test_utils.h"

const esp_partition_t *get_test_data_partition()
{
    /* This user type/subtype (0x55) is set in
       partition_table_unit_test_app.csv */
    const esp_partition_t *result = esp_partition_find_first(0x55, 0x55, NULL);
    TEST_ASSERT_NOT_NULL(result); /* means partition table set wrong */
    return result;
}
