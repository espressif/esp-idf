# This script should be sourced, not executed.

# `idf_tools.py export --deactivate` create statement, with keyword unset, but fish shell support only `set --erase variable`
function unset
    set --erase $argv
end

set script_dir (dirname (realpath (status -f)))
eval ("$script_dir"/activate.py --export)
