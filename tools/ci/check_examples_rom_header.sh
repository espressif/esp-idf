#!/bin/bash

# Examples shouldn't include rom headers directly

output=$(find ${IDF_PATH}/examples -name "*.[chS]" -o -name "*.cpp" -not -path "**/build/**")
files=$(grep ".*include.*rom.*h" ${output} | cut -d ":" -f 1)
found_rom=0
for file in ${files}
do
    echo "${file} contains rom headers!"
    found_rom=`expr $found_rom + 1`;
done

if [ $found_rom -eq 0 ]; then
    echo "No rom headers found in examples"
    exit 0
fi

exit 1
