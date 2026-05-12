param(
    [string]$Port = "COM6",
    [int]$Baud = 460800
)

$ErrorActionPreference = "Stop"
if ($null -ne (Get-Variable -Name PSNativeCommandUseErrorActionPreference -ErrorAction SilentlyContinue)) {
    $PSNativeCommandUseErrorActionPreference = $false
}

function Fail([string]$msg) {
    Write-Output "ERR $msg"
    exit 1
}

$cliCandidates = @(
    "C:\Users\chuky\esp-idf\tools\pyspinel\spinel-cli.py",
    "C:\Users\chuky\openthread\pyspinel\spinel-cli.py",
    "C:\Users\chuky\openthread-h2-build\openthread\tools\pyspinel\spinel-cli.py"
)

$cliPath = $null
foreach ($candidate in $cliCandidates) {
    if (Test-Path $candidate) {
        $cliPath = $candidate
        break
    }
}

if (-not $cliPath) {
    Fail "MISSING_THREAD_CLI"
}

$chipOut = cmd /c "python -m esptool --port $Port chip-id 2>nul"
if ($LASTEXITCODE -ne 0) {
    Fail "NO_CHIP_ID port=$Port"
}

$chipLine = $chipOut | Select-String -Pattern "Detecting chip type\.\.\." | Select-Object -First 1
$chip = if ($chipLine) { ($chipLine.Line -replace "^.*Detecting chip type\.\.\.\s*", "").Trim() } else { "UNKNOWN" }

if ($chip -notlike "*ESP32-H2*") {
    Fail "THREAD_CLI_EXPECTS_RCP_H2 port=$Port got=$chip"
}

Write-Output "OK THREAD_CLI port=$Port chip=$chip"
python $cliPath -u $Port -b $Baud
