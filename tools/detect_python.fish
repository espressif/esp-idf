# This file should be sourced, not executed!
#
# This is a port of detect_python.sh. More information are provided there.

set -x ESP_PYTHON python

for p_cmd in python python3;
    echo "Checking \"$p_cmd\" ..."

    set res ($p_cmd -c "import sys; print(sys.version_info.major)")
    if [ "$res" = "3" ]
        set -x ESP_PYTHON $p_cmd
        break
    end
end

$ESP_PYTHON --version
if [ $status -ne 0 ]
    echo "\"$ESP_PYTHON\" is not installed! Please see the documentation for how to install it."
    # The following exit skips the rest of this file but won't exit fish where the script was sourced. This is not a
    # fatal issue.
    exit 1
end

echo "\"$ESP_PYTHON\" has been detected"
