# This file should be sourced, not executed!
#
# This is a port of detect_python.sh. More information are provided there.

set OLDEST_PYTHON_SUPPORTED_MAJOR 3
set OLDEST_PYTHON_SUPPORTED_MINOR 8

set -x ESP_PYTHON python

for p_cmd in python3 python python3.8 python3.9 python3.10 python3.11 python3.12;
    $p_cmd --version >/dev/null 2>&1; or continue
    echo "Checking \"$p_cmd\" ..."

    $p_cmd -c "import sys; exit(1) if sys.version_info.major < int(\"$OLDEST_PYTHON_SUPPORTED_MAJOR\") else exit(0);"; or continue
    $p_cmd -c "import sys; exit(1) if sys.version_info.minor < int(\"$OLDEST_PYTHON_SUPPORTED_MINOR\") else exit(0);"; or continue

    set ESP_PYTHON $p_cmd
    break
end

test -n "$ESP_PYTHON"; or echo "Python $OLDEST_PYTHON_SUPPORTED_MAJOR.$OLDEST_PYTHON_SUPPORTED_MINOR+ is not installed! Please see the documentation for how to install it."
test -n "$ESP_PYTHON"; or exit 1

$ESP_PYTHON --version
echo "$ESP_PYTHON has been detected"
