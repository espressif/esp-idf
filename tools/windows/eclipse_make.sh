#!/bin/bash
# A wrapper for make on Windows with Eclipse
#
# Eclipse's output parser expects to see output of the form C:/dir/dir/file but our Make
# process uses Cygwin/MinGW paths of the form /c/dir/dir/file. So parse these out...
#
# There is a heuristic where Eclipse CDT itself can detect it's
# running on "Cygwin" and run cygpath on paths itself, but I've been
# unable to find a way to trigger it.
#
# (regexp only matches after certain characters/patterns, to try and avoid false-positives.)
echo "Running make in $(pwd)"
make $@ V=1 | sed -E -e "s@( |-I|-L|'|=)/([a-z])/([^ ]+)/@\1\2:/\3/@g"
