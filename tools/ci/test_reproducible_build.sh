#!/usr/bin/env bash

set -euo

for path in \
  "examples/get-started/hello_world" \
  "examples/bluetooth/nimble/blecent"; do
  cd "${IDF_PATH}/${path}"

  echo "CONFIG_APP_REPRODUCIBLE_BUILD=y" >sdkconfig

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

  # test gdb
  rm -f gdb.txt
  elf_file=$(find build_first -maxdepth 1 -iname '*.elf')
  xtensa-esp32-elf-gdb -x build_first/prefix_map_gdbinit -ex 'set logging enabled' -ex 'set pagination off' -ex 'list app_main' -ex 'quit' "$elf_file"
  if grep "No such file or directory" gdb.txt; then
    exit 1
  fi
done
