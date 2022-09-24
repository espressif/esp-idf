#!/usr/bin/env bash

function help() {
  echo "Usage: bash build_espcoredump.sh [OUTPUT_DIR]"
}

if [ -z "$1" ]; then
  help
  exit 1
else
  output_dir=$1
fi

SUPPORTED_TARGETS=("esp32" "esp32s2" "esp32c3" "esp32s3" "esp32c2")
for chip in "${SUPPORTED_TARGETS[@]}"; do
  {
    echo "--------------------------"
    echo "building $chip binaries..."
    echo "--------------------------"
    idf.py fullclean && rm -f sdkconfig
    idf.py set-target $chip
    idf.py build
    cp ./build/test_core_dump.elf "${output_dir}/${chip}.elf"
  }
done
