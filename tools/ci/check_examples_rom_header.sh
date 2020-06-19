#!/usr/bin/env bash

set -uo pipefail

# Examples shouldn't include rom headers directly

output=$(find ${IDF_PATH}/examples -name "*.[chS]" -o -name "*.cpp" -not -path "**/build/**")
files=$(egrep ".*include.*\Wrom\W.*h" ${output} | cut -d ":" -f 1)
found_rom=0
for file in ${files}
do
    echo "${file} contains rom headers!"
    ((found_rom++))
done

if [ $found_rom -eq 0 ]; then
    echo "No rom headers found in examples"
    exit 0
fi

exit 1
