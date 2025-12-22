# This file should be sourced, not executed!
#
# This is a port of detect_python.sh. More information are provided there.

function detect_python

set OLDEST_PYTHON_SUPPORTED_MAJOR 3
set OLDEST_PYTHON_SUPPORTED_MINOR 10

if test -z "$ESP_PYTHON_CUSTOM"
    set PYTHON_CANDIDATES python3 python python3.10 python3.11 python3.12 python3.13
else
    echo "Reading ESP_PYTHON_CUSTOM from environment: \"$ESP_PYTHON_CUSTOM\""
    set PYTHON_CANDIDATES "$ESP_PYTHON_CUSTOM"
end

for p_cmd in $PYTHON_CANDIDATES
    $p_cmd --version >/dev/null 2>&1; or continue
    echo "Checking \"$p_cmd\" ..."

    $p_cmd -c "import sys; exit(1) if sys.version_info.major < int(\"$OLDEST_PYTHON_SUPPORTED_MAJOR\") else exit(0);"; or continue
    $p_cmd -c "import sys; exit(1) if sys.version_info.minor < int(\"$OLDEST_PYTHON_SUPPORTED_MINOR\") else exit(0);"; or continue

    set ESP_PYTHON "$p_cmd"
    break
end
set -e PYTHON_CANDIDATES

test -n "$ESP_PYTHON"; or echo "Python $OLDEST_PYTHON_SUPPORTED_MAJOR.$OLDEST_PYTHON_SUPPORTED_MINOR+ is not installed! Please see the documentation for how to install it." >&2
test -n "$ESP_PYTHON"; or return 1

$ESP_PYTHON --version
echo "$ESP_PYTHON has been detected"
set -gx ESP_PYTHON "$ESP_PYTHON"

end
detect_python # Make sure at last line call function, because outside caller need its return value
