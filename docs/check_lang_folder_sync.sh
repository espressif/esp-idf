#!/bin/bash
#
# Check if folders with localized documentation are in sync
#
# 1. Traverse each folder with language version and generate a sorted list
#    of all the files inside
# 2. Compare the sorted lists of files and flag differences
#
# Note:
# All differences between folders with language versions should be resolved
# before releasing documentation
#

RESULT=0
STARS='***************************************************'

find en -type f | cut -d/ -f2- | sort > file_list_en
find zh_CN -type f | cut -d/ -f2- | sort > file_list_zh_CN

# format is to display new or different filenames
DIFF_FORMAT="--unchanged-line-format= --old-line-format=[en]:%L --new-line-format=[zh_CN]:%L"

FOLDER_DIFFERENCES=$(diff $DIFF_FORMAT file_list_en file_list_zh_CN)
if ! [ -z "$FOLDER_DIFFERENCES" ]; then
    echo "$STARS"
    echo "Build failed due to the following differences in 'en' and 'zh_CN' folders:"
    echo "$FOLDER_DIFFERENCES"
    echo "$STARS"
    echo "Please synchronize contents of 'en' and 'zh_CN' folders to contain files with identical names"
    RESULT=1
fi

# remove temporary files
rm file_list_en file_list_zh_CN

exit $RESULT
