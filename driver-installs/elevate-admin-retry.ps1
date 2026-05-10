param(
    [string]$WorkingDir = "C:\Users\chuky\esp-idf\driver-installs",
    [switch]$RunCom6Cleanup
)

$ErrorActionPreference = "Stop"

function Test-IsAdmin {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($identity)
    $isAdmin = $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
    
    return $isAdmin
}

function Test-AdminStrict {
    [CmdletBinding()]
    [OutputType([bool])]
    param()

    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($identity)

    $isAdmin = $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
    $elevatedToken = [bool]([System.Security.Principal.WindowsIdentity]::GetCurrent().Owner)

    if (-not $isAdmin) {
        Write-Host "ERROR: Administrator role not active." -ForegroundColor Red
        Write-Host "  IsAdmin: $isAdmin" -ForegroundColor Red
        Write-Host "  User: $($identity.Name)" -ForegroundColor Red
        return $false
    }

    Write-Host "✓ Administrator token verified:" -ForegroundColor Green
    Write-Host "  User: $($identity.Name)" -ForegroundColor Cyan
    Write-Host "  Admin Role: $isAdmin" -ForegroundColor Cyan
    Write-Host "  Elevated Token: $($elevatedToken -or $isAdmin)" -ForegroundColor Cyan

    return $true
}

if (-not (Test-IsAdmin)) {
    Write-Host "Current shell not elevated. Requesting Administrator elevation..." -ForegroundColor Yellow
    Write-Host "UAC prompt will appear. Please approve." -ForegroundColor Yellow

    $elevationArgs = @(
        "-NoProfile",
        "-ExecutionPolicy", "Bypass",
        "-File", ('"{0}"' -f $PSCommandPath),
        "-WorkingDir", ('"{0}"' -f $WorkingDir)
    )

    if ($RunCom6Cleanup) {
        $elevationArgs += "-RunCom6Cleanup"
    }

    Start-Process -FilePath "pwsh.exe" -Verb RunAs -ArgumentList $elevationArgs
    exit 0
}

if (-not (Test-AdminStrict)) {
    Write-Host "ERROR: Could not verify administrator permissions. Exiting." -ForegroundColor Red
    exit 1
}

Write-Host "Administrator token confirmed and verified." -ForegroundColor Green

if (Test-Path $WorkingDir) {
    Set-Location $WorkingDir
}

Write-Host "Current directory: $((Get-Location).Path)" -ForegroundColor Cyan

if ($RunCom6Cleanup) {
    Write-Host "`nRunning COM6 stale-cache cleanup (ADMIN-ONLY)..." -ForegroundColor Yellow

    if (-not (Test-IsAdmin)) {
        Write-Host "ERROR: Admin elevation required for cleanup. Aborting." -ForegroundColor Red
        exit 1
    }

    $staleIds = @(
        "USB\VID_303A&PID_1001&MI_00\7&3a6d1780&0&0000",
        "USB\VID_303A&PID_1001&MI_00\7&36304bf2&0&0000"
    )

    foreach ($id in $staleIds) {
        Write-Host "Removing: $id" -ForegroundColor Cyan
        try {
            pnputil /remove-device "$id"
            Write-Host "  ✓ Removed successfully" -ForegroundColor Green
        }
        catch {
            Write-Host "  ✗ Failed: $($_.Exception.Message)" -ForegroundColor Red
        }
    }

    Write-Host "Clearing COM6 name-arbiter cache..." -ForegroundColor Cyan
    try {
        $regPath = "HKLM:\SYSTEM\CurrentControlSet\Control\COM Name Arbiter\Devices"
        Remove-ItemProperty -Path $regPath -Name "COM6" -ErrorAction SilentlyContinue
        Write-Host "  ✓ Cache cleared (if entry existed)" -ForegroundColor Green
    }
    catch {
        Write-Host "  ✗ Failed: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }

    Write-Host "Rescanning hardware..." -ForegroundColor Cyan
    pnputil /scan-devices
    Write-Host "  ✓ Hardware rescan complete" -ForegroundColor Green
}

Write-Host "`n╔════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║           ADMIN VERIFICATION SUMMARY                    ║" -ForegroundColor Cyan
Write-Host "╚════════════════════════════════════════════════════════╝" -ForegroundColor Cyan

Write-Host "`nCurrent Status:" -ForegroundColor Yellow
Get-PnpDevice -Class Ports | Select-Object Name, Status, InstanceId | Format-Table -AutoSize

Write-Host "`nAvailable COM ports:" -ForegroundColor Yellow
$ports = [System.IO.Ports.SerialPort]::GetPortNames()
if ($ports.Count -gt 0) {
    $ports | Sort-Object | ForEach-Object { Write-Host "  • $_" -ForegroundColor Green }
}
else {
    Write-Host "  (No ports found)" -ForegroundColor Gray
}

Write-Host "`nAdministrator Status: ACTIVE" -ForegroundColor Green
Write-Host "Ready for COM6 operations." -ForegroundColor Green
Write-Host "`nNext steps:" -ForegroundColor Cyan
Write-Host "  1. Plug in ESP32-C6 USB-C cable" -ForegroundColor Cyan
Write-Host "  2. Run: cd C:\Users\chuky\esp-idf\examples\wifi" -ForegroundColor Cyan
Write-Host "  3. Run: .\build-flash-otbr.ps1 -BuildFlashMonitor -Port COM6" -ForegroundColor Cyan
