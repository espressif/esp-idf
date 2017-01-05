// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

// Common header for SPI flash test data
#pragma once

/* Define a region of flash we can mess up for testing...

   This is pretty ugly, better to do something with a partition but
   this is OK for now.
 */
#define TEST_REGION_START 0x180000
#define TEST_REGION_END   0x1E0000
