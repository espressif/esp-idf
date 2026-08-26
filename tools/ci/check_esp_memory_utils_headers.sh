#!/usr/bin/env bash

set -euo pipefail

# Some memory utility functions need to be defined both in bootloader and app contexts.
# To avoid adding bootloader_support as a public dependency of every component,
# and to avoid adding esp_hw_support as a bootloader dependency, some code is duplicated
# between two memory utils files. This script checks that the duplicated code is in sync.

esp_hw_support_header="${IDF_PATH}/components/esp_hw_support/include/esp_memory_utils.h"
bootloader_support_header="${IDF_PATH}/components/bootloader_support/include/bootloader_memory_utils.h"

bootloader_support_start="The content of this file is to be kept in sync with the common section of esp_memory_utils.h"
bootloader_support_end="End of the common section that has to be in sync with esp_memory_utils.h"

esp_hw_support_start="Common functions, to be kept in sync with bootloader_memory_utils.h"
esp_hw_support_end="End of common functions to be kept in sync with bootloader_memory_utils.h"

# get_file_part <input> <output> <from> <to>
#   Extract lines of <input> starting with a line which matches <from>
#   and ending with a line which matches <to>, and write the result to <output>.
function get_file_part
{
    input_file=$1
    output_file=$2
    from_line=$3
    to_line=$4

    awk "/${from_line}/{print START; select=1; next} /${to_line}/{print \"END\"; select=0} select{print \$0}" "${input_file}" > "${output_file}"
}

esp_hw_support_part_file=esp_hw_support_part.h
bootloader_support_part_file=bootloader_support_part.h

get_file_part "${esp_hw_support_header}" "${esp_hw_support_part_file}" "${esp_hw_support_start}" "${esp_hw_support_end}"
get_file_part "${bootloader_support_header}" "${bootloader_support_part_file}" "${bootloader_support_start}" "${bootloader_support_end}"

if ! diff --unified "${esp_hw_support_part_file}" "${bootloader_support_part_file}"; then
    echo ""
    echo "    The content of common sections in esp_memory_utils.h and bootloader_memory_utils.h is different."
    echo "    If you made changes to one of these files, please update the other one with the same changes."
    echo "    See ${esp_hw_support_part_file} and ${bootloader_support_part_file} for details."
    exit 1
fi

echo "esp_memory_utils.h and bootloader_memory_utils.h are in sync."
rm -f "${esp_hw_support_part_file}" "${bootloader_support_part_file}"
