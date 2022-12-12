#!/usr/bin/env bash

function help() {
  echo "Usage: bash test_espcoredump.sh [ELF_DIR]"
}

if [ -z "$1" ]; then
  help
  exit 1
else
  elf_dir=$1
fi

COREDUMP_VERSION="espcoredump.py v$(python -c "import pkg_resources; print(pkg_resources.get_distribution('esp-coredump').version)")"
COREDUMP_VERSION_REGEX="espcoredump.py v([0-9])+.([0-9a-z-])+(.[0-9a-z-])?"
SUPPORTED_TARGETS=("esp32" "esp32s2" "esp32c3" "esp32s3" "esp32c2")
res=0
for chip in "${SUPPORTED_TARGETS[@]}"; do
  {
    echo "run b64 decoding tests on $chip"
    esp-coredump --chip="$chip" --gdb-timeout-sec 5 info_corefile -m -t b64 -c "${chip}/coredump.b64" -s "${chip}/core.elf" "${elf_dir}/${chip}.elf" &>"${chip}/output" &&
    sed -i -E "s/$COREDUMP_VERSION_REGEX/$COREDUMP_VERSION/" "${chip}/expected_output"
      diff "${chip}/expected_output" "${chip}/output" &&
      esp-coredump --chip="$chip" --gdb-timeout-sec 5 info_corefile -m -t elf -c "${chip}/core.elf" "${elf_dir}/${chip}.elf" &>"${chip}/output2" &&
      sed -E "s/$COREDUMP_VERSION_REGEX/$COREDUMP_VERSION/" "${chip}/expected_output"
      diff "${chip}/expected_output" "${chip}/output2"
  } || {
    echo 'The test for espcoredump has failed!'
    res=1
  }
done
exit $res
