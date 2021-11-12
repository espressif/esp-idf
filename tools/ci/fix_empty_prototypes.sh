#!/usr/bin/env bash

# This script finds and fixes up empty prototypes, to satisfy `-Wstrict-prototypes` and to сomply the C Standard

set -o errexit
set -o pipefail
set -o nounset

ctags -R -f - --c-kinds=pf --languages="c" --langmap=c:.c.h | grep "([[:space:]]*)" > tmp_ctags.txt || :

ctags -R -f - --c-kinds=pf --languages="c++" --langmap=c++:.cpp | grep "([[:space:]]*)" | grep -F 'extern "C"' >> tmp_ctags.txt || :

while read TAGLINE; do

    # a format of ctags:
    # https://en.wikipedia.org/wiki/Ctags

    # a 2nd column,
    FILENAME=$(printf "$TAGLINE" | sed -E "s/([^[:space:]]+)[[:space:]]([^[:space:]]+)[[:space:]](.+)\;\".*/\2/g")

    # a 3rd column
    # a pattern
    # /^ void   sdmmc_host_dma_stop  (      );$/
    OLDLINE=$(printf "$TAGLINE" | sed -E "s/([^[:space:]]+)[[:space:]]([^[:space:]]+)[[:space:]](.+)\;\".*/\3/g")

    # remove leading and trailng '/'-s
    OLDLINE="${OLDLINE#/}"
    OLDLINE="${OLDLINE%/}"

    # remove leading '^' and trailing '$'
    OLDLINE="${OLDLINE#^}"
    OLDLINE="${OLDLINE%$}"


    OLDBRACERS=$(printf "$OLDLINE"  | sed "s/.*\(([[:space:]]*)\).*/\1/")
    NEWBRACERS="(void)"

    NEWLINE=${OLDLINE/$OLDBRACERS/$NEWBRACERS}

    # escaping
    OLDLINE=$(printf "%q" "$OLDLINE")
    NEWLINE=$(printf "%q" "$NEWLINE")

    sed -i -E 's,'"^$OLDLINE\$"','"$NEWLINE"',' $FILENAME

done < tmp_ctags.txt

echo "+++"
echo "Also 'git grep -E  \"^.*\([^\)]+\) *\(\).*$\"' will help to find some callback declarations"
echo "+++"
