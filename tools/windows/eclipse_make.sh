#!/bin/bash
# A wrapper for make on Windows with Eclipse
#
# Eclipse's output parser expects to see output of the form C:/dir/dir/file but our Make
# process uses MinGW paths of the form /c/dir/dir/file. So parse these out...
#
# A little hacky as it looks for any single character of form /X/something.
#
echo "Running make in $(pwd)"
make $@ V=1 | sed -E "s@/([a-z])/([^/+])@\1:/\2@g"
