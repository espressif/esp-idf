// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

// This file contains various convenience macros to be used in ULP programs.

// Helper macros to calculate bit field width from mask, using the preprocessor.
// Used later in READ_RTC_FIELD and WRITE_RTC_FIELD.
#define IS_BIT_SET(m, i) (((m) >> (i)) & 1)
#define MASK_TO_WIDTH_HELPER1(m, i)  IS_BIT_SET(m, i)
#define MASK_TO_WIDTH_HELPER2(m, i)  (MASK_TO_WIDTH_HELPER1(m, i)  + MASK_TO_WIDTH_HELPER1(m, i + 1))
#define MASK_TO_WIDTH_HELPER4(m, i)  (MASK_TO_WIDTH_HELPER2(m, i)  + MASK_TO_WIDTH_HELPER2(m, i + 2))
#define MASK_TO_WIDTH_HELPER8(m, i)  (MASK_TO_WIDTH_HELPER4(m, i)  + MASK_TO_WIDTH_HELPER4(m, i + 4))
#define MASK_TO_WIDTH_HELPER16(m, i) (MASK_TO_WIDTH_HELPER8(m, i)  + MASK_TO_WIDTH_HELPER8(m, i + 8))
#define MASK_TO_WIDTH_HELPER32(m, i) (MASK_TO_WIDTH_HELPER16(m, i) + MASK_TO_WIDTH_HELPER16(m, i + 16))

// Peripheral register access macros, build around REG_RD and REG_WR instructions.
// Registers defined in rtc_cntl_reg.h, rtc_io_reg.h, sens_reg.h, and rtc_i2c_reg.h are usable with these macros.

// Read from rtc_reg[low_bit + bit_width - 1 : low_bit] into R0, bit_width <= 16
#define READ_RTC_REG(rtc_reg, low_bit, bit_width) \
    REG_RD (((rtc_reg) - DR_REG_RTCCNTL_BASE) / 4), ((low_bit) + (bit_width) - 1), (low_bit)

// Write immediate value into rtc_reg[low_bit + bit_width - 1 : low_bit], bit_width <= 8
#define WRITE_RTC_REG(rtc_reg, low_bit, bit_width, value) \
    REG_WR (((rtc_reg) - DR_REG_RTCCNTL_BASE) / 4), ((low_bit) + (bit_width) - 1), (low_bit), ((value) & 0xff)

// Read from a field in rtc_reg into R0, up to 16 bits
#define READ_RTC_FIELD(rtc_reg, field) \
    READ_RTC_REG(rtc_reg, field ## _S, MASK_TO_WIDTH_HELPER16(field ## _V, 0))

// Write immediate value into a field in rtc_reg, up to 8 bits
#define WRITE_RTC_FIELD(rtc_reg, field, value) \
    WRITE_RTC_REG(rtc_reg, field ## _S, MASK_TO_WIDTH_HELPER8(field ## _V, 0), ((value) & field ## _V))
