# SPDX-License-Identifier: Apache-2.0
# OTBR Diagnostics & Testing Helper

param(
    [string]$Port = "COM6",
    [switch]$QuickTest,
    [switch]$Diagnostics,
    [switch]$WiFiTest,
    [switch]$ThreadTest,
    [switch]$RCPTest,
    [switch]$All,
    [switch]$Help
)

function Write-Header {
    Write-Host "`n╔════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "║        OTBR Diagnostics & Testing Helper               ║" -ForegroundColor Cyan
    Write-Host "╚════════════════════════════════════════════════════════╝`n" -ForegroundColor Cyan
}

function Show-Help {
    Write-Host @"
OTBR Diagnostics and Testing

USAGE:
  .\otbr-diagnostics.ps1 [options]

OPTIONS:
  -Port COM#              Serial port (default: COM6)
  -QuickTest             Run quick sanity checks
  -Diagnostics           Full system diagnostics
  -WiFiTest              Test WiFi connectivity
  -ThreadTest            Test Thread network
  -RCPTest               Test RCP (Radio Co-Processor)
  -All                   Run all tests
  -Help                  Show this help

EXAMPLES:
  # Quick sanity check
  .\otbr-diagnostics.ps1 -QuickTest

  # Full diagnostics
  .\otbr-diagnostics.ps1 -Diagnostics

  # Test everything
  .\otbr-diagnostics.ps1 -All

"@
}

function Show-Menu {
    Write-Host "╔════════════════════════════════════════════════════════╗" -ForegroundColor Yellow
    Write-Host "║              OTBR Test Menu                            ║" -ForegroundColor Yellow
    Write-Host "╠════════════════════════════════════════════════════════╣" -ForegroundColor Yellow
    Write-Host "║                                                        ║" -ForegroundColor Yellow
    Write-Host "║  1. Quick Test (sanity check)                          ║" -ForegroundColor Yellow
    Write-Host "║  2. WiFi Test                                          ║" -ForegroundColor Yellow
    Write-Host "║  3. Thread Test                                        ║" -ForegroundColor Yellow
    Write-Host "║  4. RCP Test (Radio Co-Processor)                      ║" -ForegroundColor Yellow
    Write-Host "║  5. Full Diagnostics                                   ║" -ForegroundColor Yellow
    Write-Host "║  6. Network Information Report                         ║" -ForegroundColor Yellow
    Write-Host "║                                                        ║" -ForegroundColor Yellow
    Write-Host "║  Enter choice (1-6) or press Ctrl+C to exit:           ║" -ForegroundColor Yellow
    Write-Host "║                                                        ║" -ForegroundColor Yellow
    Write-Host "╚════════════════════════════════════════════════════════╝" -ForegroundColor Yellow
    
    $Choice = Read-Host ""
    return $Choice
}

function Create-TestScript {
    param(
        [string[]]$Commands,
        [string]$TestName
    )
    
    Write-Host "`n═══════════════════════════════════════════════════════" -ForegroundColor Cyan
    Write-Host "Test: $TestName" -ForegroundColor Cyan
    Write-Host "═══════════════════════════════════════════════════════`n" -ForegroundColor Cyan
    
    Write-Host "📋 Commands to run in OTBR console:" -ForegroundColor Yellow
    Write-Host "┌───────────────────────────────────────────────────────┐" -ForegroundColor DarkGray
    
    $i = 1
    foreach ($cmd in $Commands) {
        Write-Host "$i. $cmd" -ForegroundColor Green
        $i++
    }
    
    Write-Host "└───────────────────────────────────────────────────────┘`n" -ForegroundColor DarkGray
    
    Write-Host "Press Enter to open serial monitor (Ctrl+] to exit)..." -ForegroundColor Yellow
    Read-Host "" | Out-Null
    
    Push-Location "c:\Users\chuky\esp-idf\examples\openthread\ot_br"
    idf.py -p $Port monitor
    Pop-Location
}

# Test: Quick Sanity Check
function Invoke-QuickTest {
    $Commands = @(
        "version",
        "state",
        "channel",
        "panid",
        "wifi_status"
    )
    
    Create-TestScript -Commands $Commands -TestName "Quick Sanity Check"
}

# Test: WiFi Connectivity
function Invoke-WiFiTest {
    $Commands = @(
        "wifi_status",
        "wifi_scan",
        "ipaddr",
        "ifconfig"
    )
    
    Create-TestScript -Commands $Commands -TestName "WiFi Connectivity"
}

# Test: Thread Network
function Invoke-ThreadTest {
    $Commands = @(
        "state",
        "networkname",
        "channel",
        "panid",
        "extpanid",
        "networkkey",
        "childlist",
        "neighbors",
        "ipaddr"
    )
    
    Create-TestScript -Commands $Commands -TestName "Thread Network"
}

# Test: RCP (Radio Co-Processor)
function Invoke-RCPTest {
    $Commands = @(
        "rcp_version",
        "rcp_state"
    )
    
    Create-TestScript -Commands $Commands -TestName "RCP (Radio Co-Processor)"
}

# Full Diagnostics
function Invoke-Diagnostics {
    $Commands = @(
        "# === FIRMWARE ===",
        "version",
        "rcp_version",
        "",
        "# === THREAD NETWORK ===",
        "state",
        "networkname",
        "channel",
        "panid",
        "extpanid",
        "networkkey",
        "",
        "# === DEVICES ===",
        "childlist",
        "neighbors",
        "",
        "# === NETWORK CONFIG ===",
        "ipaddr",
        "ifconfig",
        "",
        "# === WiFi ===",
        "wifi_status",
        "",
        "# === RCP STATUS ===",
        "rcp_state"
    )
    
    Write-Host "`n═══════════════════════════════════════════════════════" -ForegroundColor Cyan
    Write-Host "Full System Diagnostics" -ForegroundColor Cyan
    Write-Host "═══════════════════════════════════════════════════════`n" -ForegroundColor Cyan
    
    Write-Host "📋 Diagnostic commands:" -ForegroundColor Yellow
    Write-Host "┌───────────────────────────────────────────────────────┐" -ForegroundColor DarkGray
    
    $i = 1
    foreach ($cmd in $Commands) {
        if ($cmd -match "^#") {
            Write-Host $cmd -ForegroundColor DarkGray
        }
        elseif ($cmd -eq "") {
            # Empty line
        }
        else {
            Write-Host "$i. $cmd" -ForegroundColor Green
            $i++
        }
    }
    
    Write-Host "└───────────────────────────────────────────────────────┘`n" -ForegroundColor DarkGray
    
    Write-Host "Press Enter to open serial monitor (Ctrl+] to exit)..." -ForegroundColor Yellow
    Read-Host "" | Out-Null
    
    Push-Location "c:\Users\chuky\esp-idf\examples\openthread\ot_br"
    idf.py -p $Port monitor
    Pop-Location
}

# Report Generation
function Show-TestReport {
    Write-Host "`n╔════════════════════════════════════════════════════════╗" -ForegroundColor Green
    Write-Host "║         OTBR Diagnostic Report Template               ║" -ForegroundColor Green
    Write-Host "╠════════════════════════════════════════════════════════╣" -ForegroundColor Green
    Write-Host "║                                                        ║" -ForegroundColor Green
    Write-Host "║ Firmware & Version                                     ║" -ForegroundColor Green
    Write-Host "║  ☐ Firmware version: __________________________        ║" -ForegroundColor Green
    Write-Host "║  ☐ RCP version: __________________________            ║" -ForegroundColor Green
    Write-Host "║                                                        ║" -ForegroundColor Green
    Write-Host "║ Thread Network                                         ║" -ForegroundColor Green
    Write-Host "║  ☐ State: ________________________ (leader/router)    ║" -ForegroundColor Green
    Write-Host "║  ☐ Network Name: __________________________            ║" -ForegroundColor Green
    Write-Host "║  ☐ Channel: ________________________                  ║" -ForegroundColor Green
    Write-Host "║  ☐ PAN ID: __________________________                ║" -ForegroundColor Green
    Write-Host "║  ☐ Extended PAN ID: __________________________        ║" -ForegroundColor Green
    Write-Host "║  ☐ Network Key: __________________________            ║" -ForegroundColor Green
    Write-Host "║                                                        ║" -ForegroundColor Green
    Write-Host "║ Connected Devices                                      ║" -ForegroundColor Green
    Write-Host "║  ☐ Children: ________________________                 ║" -ForegroundColor Green
    Write-Host "║  ☐ Neighbors: ________________________                ║" -ForegroundColor Green
    Write-Host "║                                                        ║" -ForegroundColor Green
    Write-Host "║ Network Connectivity                                   ║" -ForegroundColor Green
    Write-Host "║  ☐ IPv6 Address: __________________________            ║" -ForegroundColor Green
    Write-Host "║  ☐ IPv4 Address: __________________________            ║" -ForegroundColor Green
    Write-Host "║  ☐ WiFi Status: ________________________             ║" -ForegroundColor Green
    Write-Host "║                                                        ║" -ForegroundColor Green
    Write-Host "║ RCP Status                                             ║" -ForegroundColor Green
    Write-Host "║  ☐ RCP State: ________________________                ║" -ForegroundColor Green
    Write-Host "║                                                        ║" -ForegroundColor Green
    Write-Host "╚════════════════════════════════════════════════════════╝`n" -ForegroundColor Green
}

# Main Execution
Write-Header

if ($Help) {
    Show-Help
    exit 0
}

# If no specific test is requested, show menu
if (-not ($QuickTest -or $Diagnostics -or $WiFiTest -or $ThreadTest -or $RCPTest -or $All)) {
    while ($true) {
        $Choice = Show-Menu
        
        switch ($Choice) {
            "1" { Invoke-QuickTest; break }
            "2" { Invoke-WiFiTest; break }
            "3" { Invoke-ThreadTest; break }
            "4" { Invoke-RCPTest; break }
            "5" { Invoke-Diagnostics; break }
            "6" { Show-TestReport; break }
            default { 
                Write-Host "Invalid choice. Please try again." -ForegroundColor Red
                continue
            }
        }
        
        $Again = Read-Host "`nRun another test? (y/n)"
        if ($Again -ne "y" -and $Again -ne "Y") {
            break
        }
    }
}
else {
    # Run specific tests based on parameters
    if ($QuickTest) { Invoke-QuickTest }
    if ($WiFiTest) { Invoke-WiFiTest }
    if ($ThreadTest) { Invoke-ThreadTest }
    if ($RCPTest) { Invoke-RCPTest }
    if ($Diagnostics) { Invoke-Diagnostics }
    if ($All) {
        Invoke-QuickTest
        Invoke-WiFiTest
        Invoke-ThreadTest
        Invoke-RCPTest
        Invoke-Diagnostics
    }
}

Write-Host "`n✓ Diagnostics complete`n" -ForegroundColor Green
