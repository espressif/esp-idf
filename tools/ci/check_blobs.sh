#!/usr/bin/env bash
set -euo pipefail

if [ -z "${IDF_PATH:-}" ]; then
    echo "IDF_PATH must be set before running this script"
    exit 1
fi

failed=""

# check_lib_symbols <libraray path> <symbols to look for...>
#
# If the given library contains references to the listed symbols, prints
# a message and adds the library to "failed" list.
function check_lib_symbols {
    lib="$1"
    symbols="${@:2}"
    syms_file="$(mktemp)"
    # for symbols="foo bar" create grep search argument "foo\|bar"
    symbols_args="${symbols// /\\|}"
    errors=0
    nm -A $lib 2>/dev/null | { grep -w "${symbols_args}" > ${syms_file} || true; }
    if [ $(wc -l <${syms_file}) != 0 ]; then
        echo "${lib}: found illegal symbol references:"
        cat ${syms_file} | sed 's/^/\t/'
        failed="${failed} ${lib}"
        errors=1
    fi
    if [ $errors == 0 ]; then
        echo "${lib}: OK"
    fi
    rm -f ${syms_file}
}

# Check Wi-Fi, PHY libraries for references to "printf"-like functions:
illegal_symbols="printf ets_printf"

pushd ${IDF_PATH}/components/esp_wifi/lib > /dev/null
wifi_targets=$(find . -type d -name 'esp*' -exec basename {} \; | sort)
for target in ${wifi_targets}; do
    for library in ${target}/*.a; do
        check_lib_symbols ${library} ${illegal_symbols}
    done
done
popd > /dev/null

pushd ${IDF_PATH}/components/esp_phy/lib > /dev/null
phy_targets=$(find . -type d -name 'esp*' -exec basename {} \; | sort)
for target in ${phy_targets}; do
    libraries=$(find ${target} -name '*.a')
    for library in ${libraries}; do
        check_lib_symbols ${library} ${illegal_symbols}
    done
done
popd > /dev/null

# Print summary
if [ -n "${failed}" ]; then
    echo "Issues found in the following libraries:"
    for lib in $failed; do
        echo "- $lib"
    done
    exit 1
fi
