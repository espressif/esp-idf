#!/bin/bash
# A wrapper for make on Windows with Eclipse
#
# Eclipse's output parser expects to see output of the form C:/dir/dir/file but our Make
# process uses MinGW paths of the form /c/dir/dir/file. So parse these out...
#
# (regexp deliberate only matches after a space character to try and avoid false-positives.)
echo "Running make in $(pwd)"
make $@ V=1 | sed -E "s@ /([a-z])/(.+)/@ \1:/\2/@g" | sed -E "s@-I/([a-z])/(.+)/@-I\1:/\2/@g" | sed -E "s@-L/([a-z])/(.+)/@-L\1:/\2/@g"
