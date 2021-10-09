#!/usr/bin/env bash

set -euo

for path in \
  "examples/get-started/hello_world" \
  "examples/bluetooth/nimble/blecent"; do
  cd "${IDF_PATH}/${path}"

  echo "CONFIG_APP_REPRODUCIBLE_BUILD=y" >>sdkconfig.defaults
  rm -f sdkconfig

  idf.py -B build_first fullclean build
  idf.py -B build_second fullclean build

  for item in \
    "partition_table/partition-table.bin" \
    "bootloader/bootloader.bin" \
    "bootloader/bootloader.elf" \
    "bootloader/bootloader.map" \
    "*.bin" \
    "*.elf" \
    "*.map"; do
    diff -s build_first/${item} build_second/${item} # use glob, don't use double quotes
  done
done
