#!/usr/bin/env bash
set -e

# IDF_GIT_SAFE_DIR has the same format as system PATH environment variable.
# All path specified in IDF_GIT_SAFE_DIR will be added to user's
# global git config as safe.directory paths. For more information
# see git-config manual page.
if [ -n "${IDF_GIT_SAFE_DIR+x}" ]
then
	echo "Adding following directories into git's safe.directory"
	echo "$IDF_GIT_SAFE_DIR" | tr ':' '\n' | while read -r dir
	do
		git config --global --add safe.directory "$dir"
		echo "  $dir"
	done
fi

. $IDF_PATH/export.sh

exec "$@"
