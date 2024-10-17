# This script should be sourced, not executed.

# `idf_tools.py export --deactivate` create statement, with keyword unset, but fish shell support only `set --erase variable`
function unset
    set --erase $argv
end

# Emergency backup option to use previous export.fish (export_legacy.fish) if the new export approach fails.
# To use it, set environmental variable like: export ESP_IDF_LEGACY_EXPORT=1
if test -n "$ESP_IDF_LEGACY_EXPORT"
   source tools/legacy_exports/export_legacy.fish
   exit $status
end

set idf_path (dirname (realpath (status -f)))

if not test -f "$idf_path/tools/idf.py"
   or not test -f "$idf_path/tools/idf_tools.py"
   or not test -f "$idf_path/tools/activate.py"
    echo "Could not detect IDF_PATH. Please set it before sourcing this script:"
    echo "  export IDF_PATH=(add path here)"
    set -e idf_path
    exit 1
end

source "$idf_path"/tools/detect_python.fish

eval ("$idf_path"/tools/activate.py --export)
set -e idf_path
