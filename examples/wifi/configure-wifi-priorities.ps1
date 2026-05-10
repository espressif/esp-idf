# SPDX-License-Identifier: Apache-2.0
# WiFi Network Priority Configuration Script for ESP32
# Configures multiple WiFi networks with priority levels

param(
    [string]$Port = "COM6",
    [int]$BaudRate = 115200,
    [int]$TimeoutSeconds = 5
)

# WiFi Network Configuration with Priorities
$WifiNetworks = @(
    @{
        Priority = 1
        SSID     = "TP-Link_merge_MLO"
        Password = "S2s5s0S1"
    },
    @{
        Priority = 2
        SSID     = "TP-Link_merge"
        Password = "S1s0s5S2"
    },
    @{
        Priority = 3
        SSID     = "TP-Link_IoT_2.4"
        Password = "T1t0t5T2"
    },
    @{
        Priority = 4
        SSID     = "TP-Link_IoT_5G"
        Password = "T2t5t0t1"
    }
)

function Write-Header {
    Write-Host "`n========================================" -ForegroundColor Cyan
    Write-Host "  WiFi Priority Configuration Script" -ForegroundColor Cyan
    Write-Host "========================================`n" -ForegroundColor Cyan
}

function Display-Networks {
    Write-Host "Configured WiFi Networks:" -ForegroundColor Yellow
    Write-Host "─────────────────────────────────────────" -ForegroundColor Gray
    
    foreach ($network in $WifiNetworks) {
        Write-Host "Priority $($network.Priority): $($network.SSID)" -ForegroundColor Green
        Write-Host "  └─ Password: $($network.Password)" -ForegroundColor DarkGray
    }
    Write-Host ""
}

function Save-Configuration {
    param(
        [string]$OutputPath
    )
    
    # Create output directory if it doesn't exist
    $OutputDir = Split-Path -Parent $OutputPath
    if (-not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    }
    
    # Create JSON configuration file
    $ConfigObject = @{
        timestamp     = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
        networks      = $WifiNetworks
        total_networks = $WifiNetworks.Count
    }
    
    $ConfigObject | ConvertTo-Json | Set-Content -Path $OutputPath -Encoding UTF8
    Write-Host "✓ Configuration saved to: $OutputPath" -ForegroundColor Green
}

function Create-CommandScript {
    param(
        [string]$OutputPath
    )
    
    # Create a script that can be used to configure the device via console
    $ScriptContent = @"
# ESP32 WiFi Configuration Commands
# Execute these commands in the ESP32 console (via monitor)

# Priority 1: TP-Link_merge_MLO
wifi_set_sta_config --ssid "TP-Link_merge_MLO" --password "S2s5s0S1" --priority 1

# Priority 2: TP-Link_merge
wifi_set_sta_config --ssid "TP-Link_merge" --password "S1s0s5S2" --priority 2

# Priority 3: TP-Link_IoT_2.4
wifi_set_sta_config --ssid "TP-Link_IoT_2.4" --password "T1t0t5T2" --priority 3

# Priority 4: TP-Link_IoT_5G
wifi_set_sta_config --ssid "TP-Link_IoT_5G" --password "T2t5t0t1" --priority 4

# Trigger WiFi connection
wifi_connect
"@
    
    $OutputDir = Split-Path -Parent $OutputPath
    if (-not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    }
    
    Set-Content -Path $OutputPath -Value $ScriptContent -Encoding UTF8
    Write-Host "✓ Command script saved to: $OutputPath" -ForegroundColor Green
}

# Main execution
Write-Header
Display-Networks

# Define output paths
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ConfigFile = Join-Path $ScriptDir "wifi-config.json"
$CommandFile = Join-Path $ScriptDir "wifi-commands.txt"

# Save configuration files
Write-Host "Saving configurations..." -ForegroundColor Yellow
Save-Configuration -OutputPath $ConfigFile
Create-CommandScript -OutputPath $CommandFile

Write-Host "`nConfiguration complete!" -ForegroundColor Green
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "1. Flash the ESP32 device with WiFi example firmware"
Write-Host "2. Connect to the serial monitor: idf.py -p $Port monitor"
Write-Host "3. Use the configuration to provision the device`n" -ForegroundColor Gray

# Display summary
Write-Host "Summary:" -ForegroundColor Yellow
Write-Host "  Total networks: $($WifiNetworks.Count)"
Write-Host "  Config file: $ConfigFile"
Write-Host "  Command file: $CommandFile"
Write-Host "  Target port: $Port @ $BaudRate baud`n"
