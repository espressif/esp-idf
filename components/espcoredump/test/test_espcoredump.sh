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

if ! command -v coverage &> /dev/null; then
  echo "coverage could not be found, please install it ('pip install coverage')"
  exit 1
fi

SUPPORTED_TARGETS=("esp32" "esp32s2" "esp32c3" "esp32s3" )
res=0
coverage erase
for chip in "${SUPPORTED_TARGETS[@]}"; do
  {
    echo "run b64 decoding tests on $chip"
    coverage run -a --source=corefile ../espcoredump.py --chip="$chip" --gdb-timeout-sec 5 info_corefile -m -t b64 -c "${chip}/coredump.b64" -s "${chip}/core.elf" "${elf_dir}/${chip}.elf" &>"${chip}/output" &&
      diff "${chip}/expected_output" "${chip}/output" &&
      coverage run -a --source=corefile ../espcoredump.py --chip="$chip" --gdb-timeout-sec 5 info_corefile -m -t elf -c "${chip}/core.elf" "${elf_dir}/${chip}.elf" &>"${chip}/output2" &&
      diff "${chip}/expected_output" "${chip}/output2"
  } || {
    echo 'The test for espcoredump has failed!'
    res=1
  }
done
coverage run -a --source=corefile ./test_espcoredump.py
coverage report ../corefile/*.py ../espcoredump.py
exit $res
