#!/usr/bin/env bash

set -uo pipefail

# Examples shouldn't use EXTRA_COMPONENT_DIRS, instead the dependencies should be specified in idf_component.yml files

output=$(find ${IDF_PATH}/examples -name "CMakeLists.txt" -not -path "**/managed_components/**" -not -path "**/build/**")
files=$(egrep "set\(EXTRA_COMPONENT_DIRS" ${output} | cut -d ":" -f 1)
found_issues=0
for file in ${files}
do
    if [ "$file" == "${IDF_PATH}/examples/system/unit_test/test/CMakeLists.txt" ]; then
        # this specific one is okay
        continue
    fi
    echo "${file} uses EXTRA_COMPONENT_DIRS. Change it to reference the component using idf_component.yml file."
    ((found_issues++))
done

if [ $found_issues -eq 0 ]; then
    echo "No examples use EXTRA_COMPONENT_DIRS"
    exit 0
fi

exit 1
