#!/bin/bash
#
# Check for Documentation warnings:
# doxygen-warning-log.txt should be an empty file
# sphinx-warning-log.txt should only contain (fuzzy) matches to ../sphinx-known-warnings.txt
RESULT=0
STARS='***************************************************'

if [ -s doxygen-warning-log.txt ]; then
    echo "$STARS"
    echo "Build failed due to doxygen warnings:"
    cat doxygen-warning-log.txt
    echo "$STARS"
    RESULT=1
fi

# Remove escape characters, file paths, line numbers from
# the Sphinx warning log
# (escape char removal from https://www.commandlinefu.com/commands/view/6141/remove-color-codes-special-characters-with-sed
sed -r 's:\x1B\[[0-9;]*[mK]::g' sphinx-warning-log.txt | \
    sed -E "s/.*\/(.*):[0-9]+:/\1:line:/" > sphinx-warning-log-sanitized.txt

# diff sanitized warnings, ignoring lines which only appear in ../sphinx-known-warnings.txt

# format is to display only lines new or changed in second argument
DIFF_FORMAT="--unchanged-line-format= --old-line-format= --new-line-format=%L"

SPHINX_WARNINGS=$(diff $DIFF_FORMAT ../sphinx-known-warnings.txt sphinx-warning-log-sanitized.txt)
if ! [ -z "$SPHINX_WARNINGS" ]; then
    echo "$STARS"
    echo "Build failed due to new/different Sphinx warnings:"
    echo "$SPHINX_WARNINGS"
    echo "$STARS"
    RESULT=1
    echo "(Check files ../sphinx-known-warnings.txt and sphinx-warning-log.txt for full details.)"
fi

exit $RESULT

