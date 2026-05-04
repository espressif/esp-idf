# Flash and monitor OpenThread Border Router (ot_br) on ESP32-C6
# Usage: .\flash-esp32c6-otbr.ps1 [-Port COM6] [-BuildOnly] [-MonitorOnly]

param(
    [string]$Port = "COM6",
    [switch]$BuildOnly,
    [switch]$MonitorOnly
)

$PYTHON  = "C:\Users\chuky\.espressif\python_env\idf5.4_py3.12_env\Scripts\python.exe"
$IDF_PY  = "C:\Users\chuky\esp-idf\tools\idf.py"
$BUILD   = "C:\Users\chuky\esp-idf\examples\openthread\ot_br\build"

if ($BuildOnly) {
    & $PYTHON $IDF_PY -B $BUILD build
} elseif ($MonitorOnly) {
    & $PYTHON $IDF_PY -B $BUILD -p $Port monitor
} else {
    & $PYTHON $IDF_PY -B $BUILD -p $Port flash monitor
}
