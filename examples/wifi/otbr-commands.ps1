# SPDX-License-Identifier: Apache-2.0
# OTBR Serial Console Command Generator
# Creates ready-to-paste commands for OTBR configuration

param(
    [string]$Port = "COM6",
    [switch]$WiFiOnly,
    [switch]$NetworkInfo,
    [switch]$Help
)

function Write-Header {
    Write-Host "`n╔═══════════════════════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "║   OTBR Serial Console Command Generator               ║" -ForegroundColor Cyan
    Write-Host "╚═══════════════════════════════════════════════════════╝`n" -ForegroundColor Cyan
}

function Show-Help {
    Write-Host @"
OTBR Serial Console Commands

USAGE:
  .\otbr-commands.ps1 [options]

OPTIONS:
  -Port COM#              Serial port to monitor (default: COM6)
  -WiFiOnly              Show WiFi configuration commands only
  -NetworkInfo           Show network info retrieval commands only
  -Help                  Show this help message

EXAMPLES:
  # Generate all commands
  .\otbr-commands.ps1

  # WiFi configuration commands only
  .\otbr-commands.ps1 -WiFiOnly

  # Network info commands only
  .\otbr-commands.ps1 -NetworkInfo

"@
}

$WiFiCommands = @(
    @{
        Title = "Configure Priority 1: TP-Link_merge_MLO"
        Command = 'wifi_set_sta_config --ssid "TP-Link_merge_MLO" --password "S2s5s0S1" --priority 1'
    },
    @{
        Title = "Configure Priority 2: TP-Link_merge"
        Command = 'wifi_set_sta_config --ssid "TP-Link_merge" --password "S1s0s5S2" --priority 2'
    },
    @{
        Title = "Configure Priority 3: TP-Link_IoT_2.4"
        Command = 'wifi_set_sta_config --ssid "TP-Link_IoT_2.4" --password "T1t0t5T2" --priority 3'
    },
    @{
        Title = "Configure Priority 4: TP-Link_IoT_5G"
        Command = 'wifi_set_sta_config --ssid "TP-Link_IoT_5G" --password "T2t5t0t1" --priority 4'
    },
    @{
        Title = "Connect to WiFi"
        Command = "wifi_connect"
    },
    @{
        Title = "Check WiFi Status"
        Command = "wifi_status"
    },
    @{
        Title = "Disconnect WiFi"
        Command = "wifi_disconnect"
    }
)

$NetworkCommands = @(
    @{
        Title = "Show Thread Network State"
        Command = "state"
        Description = "Shows if device is 'leader', 'router', 'child', or 'detached'"
    },
    @{
        Title = "Show Thread Channel"
        Command = "channel"
        Description = "Current Thread channel (11-26, default 11)"
    },
    @{
        Title = "Show Network Key"
        Command = "networkkey"
        Description = "32-character hex Thread network key"
    },
    @{
        Title = "Show PAN ID"
        Command = "panid"
        Description = "Personal Area Network ID (hex)"
    },
    @{
        Title = "Show Extended PAN ID"
        Command = "extpanid"
        Description = "Extended PAN ID (16 hex chars) - SAVE THIS!"
    },
    @{
        Title = "Show Network Name"
        Command = "networkname"
        Description = "Human-readable network name"
    },
    @{
        Title = "Show IPv6 Addresses"
        Command = "ipaddr"
        Description = "All IPv6 addresses (look for fd...)"
    },
    @{
        Title = "Show IPv4 Address"
        Command = "ifconfig"
        Description = "Network interface configuration"
    },
    @{
        Title = "List Child Nodes"
        Command = "childlist"
        Description = "Shows Thread child devices"
    },
    @{
        Title = "List Neighbors"
        Command = "neighbors"
        Description = "Shows neighbor nodes"
    },
    @{
        Title = "Show RCP Version"
        Command = "rcp_version"
        Description = "Radio Co-Processor firmware version"
    },
    @{
        Title = "Show RCP State"
        Command = "rcp_state"
        Description = "Radio state (should be RADIO_STATE_RECEIVE)"
    }
)

function Write-CommandSection {
    param(
        [string]$SectionTitle,
        [array]$Commands
    )
    
    Write-Host "╔═══════════════════════════════════════════════════════╗" -ForegroundColor Yellow
    Write-Host "║ $SectionTitle".PadRight(57) + "║" -ForegroundColor Yellow
    Write-Host "╚═══════════════════════════════════════════════════════╝`n" -ForegroundColor Yellow
}

function Write-CommandList {
    param(
        [array]$Commands,
        [bool]$ShowDescription = $false
    )
    
    $Counter = 1
    foreach ($cmd in $Commands) {
        Write-Host "$Counter. $($cmd.Title)" -ForegroundColor Green
        Write-Host "   Command: " -ForegroundColor Gray -NoNewline
        Write-Host "`"$($cmd.Command)`"" -ForegroundColor Cyan
        
        if ($ShowDescription -and $cmd.Description) {
            Write-Host "   → $($cmd.Description)" -ForegroundColor DarkGray
        }
        Write-Host ""
        $Counter++
    }
}

function Export-Commands {
    param(
        [array]$Commands,
        [string]$Filename
    )
    
    $Content = @()
    $Content += "# OTBR Serial Console Commands"
    $Content += "# Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
    $Content += "# Paste these commands into OTBR serial console"
    $Content += ""
    
    foreach ($cmd in $Commands) {
        $Content += "# $($cmd.Title)"
        $Content += $cmd.Command
        $Content += ""
    }
    
    $Content | Set-Content -Path $Filename -Encoding UTF8
    Write-Host "✓ Commands saved to: $Filename`n" -ForegroundColor Green
}

# Main execution
Write-Header

if ($Help) {
    Show-Help
    exit 0
}

$ShowWiFi = -not $NetworkInfo -or $WiFiOnly
$ShowNetwork = -not $WiFiOnly -or $NetworkInfo

# WiFi Commands
if ($ShowWiFi) {
    Write-CommandSection "WiFi Configuration Commands" | Out-Null
    Write-CommandList -Commands $WiFiCommands
    
    Write-Host "📝 INSTRUCTIONS:" -ForegroundColor Cyan
    Write-Host "1. Open serial monitor: idf.py -p COM6 monitor" -ForegroundColor Gray
    Write-Host "2. Copy and paste each command line into the OTBR console" -ForegroundColor Gray
    Write-Host "3. Press Enter after each command" -ForegroundColor Gray
    Write-Host "4. Wait for response before next command`n" -ForegroundColor Gray
}

# Network Info Commands
if ($ShowNetwork) {
    Write-CommandSection "Thread Network Information Commands" | Out-Null
    Write-CommandList -Commands $NetworkCommands -ShowDescription $true
    
    Write-Host "📋 SAVE THIS INFORMATION FOR HOME ASSISTANT:" -ForegroundColor Cyan
    Write-Host "┌─────────────────────────────────────────────┐" -ForegroundColor Cyan
    Write-Host "│ Extended PAN ID (extpanid): ________________ │" -ForegroundColor Cyan
    Write-Host "│ Channel (channel):          ________________ │" -ForegroundColor Cyan
    Write-Host "│ Network Key (networkkey):   ________________ │" -ForegroundColor Cyan
    Write-Host "│ OTBR IPv6 Address (ipaddr): ________________ │" -ForegroundColor Cyan
    Write-Host "│ OTBR IPv4 Address (ifconfig): ______________ │" -ForegroundColor Cyan
    Write-Host "└─────────────────────────────────────────────┘`n" -ForegroundColor Cyan
}

# Export options
Write-Host "💾 EXPORT OPTIONS:" -ForegroundColor Yellow
Write-Host "These commands have been saved to files in your OTBR directory:" -ForegroundColor Gray
Write-Host ""

if ($ShowWiFi) {
    $WifiFile = "c:\Users\chuky\esp-idf\examples\openthread\ot_br\wifi-setup-commands.txt"
    Export-Commands -Commands $WiFiCommands -Filename $WifiFile
}

if ($ShowNetwork) {
    $NetworkFile = "c:\Users\chuky\esp-idf\examples\openthread\ot_br\network-info-commands.txt"
    Export-Commands -Commands $NetworkCommands -Filename $NetworkFile
}

Write-Host "🚀 NEXT STEPS:" -ForegroundColor Green
Write-Host "1. Flash OTBR to ESP32-H2 N4 COM6: .\build-flash-otbr.ps1 -BuildFlashMonitor -Port COM6" -ForegroundColor Gray
Write-Host "2. Configure WiFi using commands above" -ForegroundColor Gray
Write-Host "3. Verify Thread network with network info commands" -ForegroundColor Gray
Write-Host "4. Add to Home Assistant Settings → Devices & Services → Thread`n" -ForegroundColor Gray
