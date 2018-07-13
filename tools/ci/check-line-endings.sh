#!/bin/sh

if ! [ -z "$1" ]; then
    cd "$1"
fi

echo "Checking for Windows line endings in `pwd`"

if git ls-tree --name-only -r HEAD | xargs file -N | grep CRLF; then
    echo "Some files have CRLF (Windows-style) line endings. Please convert to LF (Unix-style). git can be configured to do this automatically."
    exit 1
fi

exit 0
