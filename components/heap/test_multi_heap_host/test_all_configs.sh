#!/usr/bin/env bash
#
# Run the test suite with all configurations enabled
#

FAIL=0

for FLAGS in "CONFIG_HEAP_POISONING_NONE" "CONFIG_HEAP_POISONING_LIGHT" "CONFIG_HEAP_POISONING_COMPREHENSIVE" ; do
    echo "==== Testing with config: ${FLAGS} ===="
    CPPFLAGS="-D${FLAGS}" make clean test || FAIL=1
done

make clean

if [ $FAIL == 0 ]; then
    echo "All configurations passed"
else
    echo "Some configurations failed, see log."
    exit 1
fi
