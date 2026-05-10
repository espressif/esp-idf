param(
    [string]$DesiredPort = "COM6",
    [switch]$ForceTakePort,
    [string]$ChipPattern = "ESP32-C6"
)

$ErrorActionPreference = "Stop"

function Test-IsAdmin {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($identity)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Ensure-Admin {
    if (Test-IsAdmin) {
        return
    }

    Write-Host "Admin rights are required. Requesting elevation..." -ForegroundColor Yellow

    $argList = @(
        "-NoProfile",
        "-ExecutionPolicy", "Bypass",
        "-File", ('"{0}"' -f $PSCommandPath),
        "-DesiredPort", $DesiredPort,
        "-ChipPattern", ('"{0}"' -f $ChipPattern)
    )

    if ($ForceTakePort) {
        $argList += "-ForceTakePort"
    }

    Start-Process -FilePath "pwsh.exe" -Verb RunAs -ArgumentList $argList
    exit 0
}

function Get-ComFromName([string]$name) {
    if ($name -match "\((COM\d+)\)") {
        return $Matches[1].ToUpperInvariant()
    }

    return $null
}

function Probe-ChipType([string]$port) {
    $output = python -m esptool --port $port chip_id 2>&1 | Out-String

    if ($output -match "Detecting chip type\.\.\.\s*([^\r\n]+)") {
        return @{ Port = $port; Chip = $Matches[1].Trim(); Output = $output }
    }

    if ($output -match "Chip is\s+([^\r\n]+)") {
        return @{ Port = $port; Chip = $Matches[1].Trim(); Output = $output }
    }

    return @{ Port = $port; Chip = "Unknown"; Output = $output }
}

function Get-PortDevices {
    return Get-PnpDevice -Class Ports |
        Where-Object { $_.InstanceId -match "VID_303A" -and $_.Present } |
        ForEach-Object {
            $com = Get-ComFromName $_.FriendlyName
            [PSCustomObject]@{
                FriendlyName = $_.FriendlyName
                InstanceId = $_.InstanceId
                Status = $_.Status
                COM = $com
            }
        }
}

function Test-PortInUseByOtherDevice([string]$port, [string]$instanceId) {
    $other = Get-PnpDevice -Class Ports |
        Where-Object {
            $_.Present -and
            $_.InstanceId -ne $instanceId -and
            $_.FriendlyName -match "\($port\)"
        }

    return $null -ne $other
}

function Set-DevicePort([string]$instanceId, [string]$port) {
    $regPath = "HKLM:\SYSTEM\CurrentControlSet\Enum\$instanceId\Device Parameters"

    if (-not (Test-Path $regPath)) {
        throw "Device Parameters path not found: $regPath"
    }

    Set-ItemProperty -Path $regPath -Name "PortName" -Value $port
}

function Restart-Device([string]$instanceId) {
    $restartOutput = pnputil /restart-device "$instanceId" 2>&1 | Out-String
    if ($LASTEXITCODE -eq 0) {
        return
    }

    Write-Host "pnputil restart failed, trying disable/enable fallback..." -ForegroundColor Yellow
    Disable-PnpDevice -InstanceId $instanceId -Confirm:$false | Out-Null
    Start-Sleep -Seconds 1
    Enable-PnpDevice -InstanceId $instanceId -Confirm:$false | Out-Null
}

Ensure-Admin

Write-Host "Scanning present Espressif USB JTAG/Serial devices..." -ForegroundColor Cyan
$devices = Get-PortDevices

if (-not $devices -or $devices.Count -eq 0) {
    throw "No present VID_303A serial ports found. Connect only the ESP32-C6 native USB first."
}

Write-Host "Candidate devices:" -ForegroundColor Cyan
$devices | Format-Table FriendlyName, COM, Status, InstanceId -AutoSize

$matches = @()
foreach ($dev in $devices) {
    if (-not $dev.COM) {
        continue
    }

    $probe = Probe-ChipType -port $dev.COM
    Write-Host ("Probe {0}: {1}" -f $dev.COM, $probe.Chip) -ForegroundColor Gray

    if ($probe.Chip -match [regex]::Escape($ChipPattern)) {
        $matches += [PSCustomObject]@{
            InstanceId = $dev.InstanceId
            COM = $dev.COM
            Chip = $probe.Chip
            FriendlyName = $dev.FriendlyName
        }
    }
}

if ($matches.Count -eq 0) {
    throw "Could not find a port that probes as '$ChipPattern'."
}

if ($matches.Count -gt 1) {
    throw "Multiple '$ChipPattern' devices detected. Keep only one C6 connected and rerun."
}

$target = $matches[0]
Write-Host ("Selected device: {0} ({1})" -f $target.FriendlyName, $target.InstanceId) -ForegroundColor Green

if ($target.COM -eq $DesiredPort.ToUpperInvariant()) {
    Write-Host ("Already mapped correctly: {0} -> {1}" -f $ChipPattern, $DesiredPort.ToUpperInvariant()) -ForegroundColor Green
    exit 0
}

if ((Test-PortInUseByOtherDevice -port $DesiredPort.ToUpperInvariant() -instanceId $target.InstanceId) -and -not $ForceTakePort) {
    throw "Desired port $DesiredPort is used by another present device. Re-run with -ForceTakePort after disconnecting that device."
}

Write-Host ("Reassigning {0} from {1} to {2}..." -f $ChipPattern, $target.COM, $DesiredPort.ToUpperInvariant()) -ForegroundColor Yellow

$arbiterPath = "HKLM:\SYSTEM\CurrentControlSet\Control\COM Name Arbiter\Devices"
Remove-ItemProperty -Path $arbiterPath -Name $DesiredPort.ToUpperInvariant() -ErrorAction SilentlyContinue

Set-DevicePort -instanceId $target.InstanceId -port $DesiredPort.ToUpperInvariant()
Restart-Device -instanceId $target.InstanceId
Start-Sleep -Seconds 2

$finalPorts = [System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object
Write-Host "Active COM ports:" -ForegroundColor Cyan
$finalPorts | ForEach-Object { Write-Host "  $_" }

$finalProbe = Probe-ChipType -port $DesiredPort.ToUpperInvariant()
Write-Host ("Final probe {0}: {1}" -f $DesiredPort.ToUpperInvariant(), $finalProbe.Chip) -ForegroundColor Cyan

if ($finalProbe.Chip -notmatch [regex]::Escape($ChipPattern)) {
    throw "Mapping change attempted, but final probe on $DesiredPort is not '$ChipPattern'."
}

Write-Host "Mapping complete: ESP32-C6 is now on COM6." -ForegroundColor Green
