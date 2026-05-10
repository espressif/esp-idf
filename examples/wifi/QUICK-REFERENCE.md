# OpenThread + Home Assistant Setup - Quick Reference

**Location:** `c:\Users\chuky\ESP-IDF\examples\wifi\`

---

## Files Created

| File | Purpose |
|------|---------|
| `configure-wifi-priorities.ps1` | WiFi configuration for OTBR |
| `run-wifi-config.ps1` | Launcher for WiFi config |
| `build-flash-otbr.ps1` | Build & flash OTBR to ESP32-C6 |
| `otbr-commands.ps1` | Generate OTBR console commands |
| `otbr-diagnostics.ps1` | Diagnostics & testing helper |
| `OTBR-SETUP-GUIDE.md` | Complete OTBR setup guide |
| `HOME-ASSISTANT-THREAD-SETUP.md` | HA Thread configuration guide |

---

## Quick Start Steps

### 1. Build & Flash OTBR to ESP32-C6

```powershell
cd c:\Users\chuky\ESP-IDF\examples\wifi
.\build-flash-otbr.ps1 -BuildFlashMonitor -Port COM6
```

**Expected output:** OTBR console appears in terminal

---

### 2. Configure WiFi in OTBR Console

In the OTBR monitor that opened:

```
wifi_set_sta_config --ssid "TP-Link_merge_MLO" --password "S2s5s0S1" --priority 1
wifi_set_sta_config --ssid "TP-Link_merge" --password "S1s0s5S2" --priority 2
wifi_set_sta_config --ssid "TP-Link_IoT_2.4" --password "T1t0t5T2" --priority 3
wifi_set_sta_config --ssid "TP-Link_IoT_5G" --password "T2t5t0t1" --priority 4
wifi_connect
wifi_status
```

**Wait for:** IP address to appear in `wifi_status` output

---

### 3. Verify Thread Network

```
state          # Should show: "leader" or "router"
channel        # Note the channel number
panid          # Note the PAN ID
extpanid       # SAVE THIS - needed for Home Assistant
networkkey     # SAVE THIS - network credentials
```

---

### 4. Set up Home Assistant Add-ons

1. **Settings → Add-ons & Integrations → Add-on Store**
2. Search `"multiprotocol"`
3. Install **Silicon Labs Multiprotocol**
4. **Start** the add-on
5. Enable **Start on boot**

---

### 5. Add Thread Integration to Home Assistant

1. **Settings → Devices & Services**
2. Click **Create Entry** (blue button)
3. Search for **"Thread"**
4. If auto-discovered, click **Submit**
5. If manual, enter thread network credentials

---

### 6. Add OTBR Integration to Home Assistant

1. **Settings → Devices & Services**
2. Click **Create Entry** (blue button)
3. Search for **"OpenThread Border Router"**
4. Enter: `openthread-border-router.local` or OTBR IP address
5. Click **Submit**

---

### 7. Commission Thread Device

1. Put Thread device in **commissioning mode**
2. **Settings → Devices & Services → Thread**
3. Click **Commission Device**
4. Scan QR code or enter 11-digit code
5. Wait 30-60 seconds for commissioning

---

## Troubleshooting Commands

### Monitor OTBR Status
```powershell
cd c:\Users\chuky\esp-idf\examples\openthread\ot_br
idf.py -p COM6 monitor
```

### Quick Diagnostics
```powershell
.\otbr-diagnostics.ps1 -QuickTest
```

### Full Diagnostics
```powershell
.\otbr-diagnostics.ps1 -Diagnostics
```

### WiFi Test
```powershell
.\otbr-diagnostics.ps1 -WiFiTest
```

### Thread Network Test
```powershell
.\otbr-diagnostics.ps1 -ThreadTest
```

### RCP Test
```powershell
.\otbr-diagnostics.ps1 -RCPTest
```

---

## Critical Information to Save

```
Extended PAN ID (extpanid):  ________________________________
Thread Channel (channel):    ________________________________
Network Key (networkkey):    ________________________________
OTBR IPv6 Address:          ________________________________
OTBR IPv4 Address:          ________________________________
OTBR Hostname:              openthread-border-router.local
```

---

## Serial Monitor Control

- **Open monitor:** `idf.py -p COM6 monitor`
- **Exit monitor:** `Ctrl + ]` (hold Ctrl, press right bracket)
- **Reset device:** `Ctrl + R` (in monitor)

---

## Ports Used

| Device | Port | Purpose |
|--------|------|---------|
| OTBR (ESP32-C6) | COM6 | Border Router |
| RCP (ESP32-H2) | COM7 | Radio Co-Processor |

If different, adjust `-Port` parameter in scripts.

---

## Network Configuration

**WiFi Priority Order:**
1. TP-Link_merge_MLO (S2s5s0S1)
2. TP-Link_merge (S1s0s5S2)
3. TP-Link_IoT_2.4 (T1t0t5T2)
4. TP-Link_IoT_5G (T2t5t0t1)

OTBR will connect to whichever is available first.

---

## OTBR Console Common Commands

```
state               Show current state (leader/router/child/detached)
wifi_status         Show WiFi connection status
ipaddr              Show IPv6 addresses
childlist           List commissioned Thread devices
neighbors           List neighbor nodes
channel             Show Thread channel
panid               Show PAN ID
extpanid            Show Extended PAN ID
networkkey          Show Network key
rcp_version         Show RCP version
rcp_state           Show RCP state
help                Show all available commands
```

---

## Home Assistant Important URLs

- Thread Integration: `Settings → Devices & Services → Integrations → Thread`
- OTBR Integration: `Settings → Devices & Services → Integrations → OpenThread Border Router`
- Add-ons: `Settings → Add-ons & Integrations → Add-on Store`

---

## Support & Resources

- **ESP-IDF Docs:** https://docs.espressif.com/
- **OpenThread Guide:** https://openthread.io/guides/border-router
- **Home Assistant Thread:** https://www.home-assistant.io/integrations/thread/
- **HA Community:** https://community.home-assistant.io

---

## Status Checklist

- [ ] ESP32-C6 flashed with OTBR
- [ ] ESP32-H2 flashed with RCP (already done)
- [ ] OTBR connected to WiFi
- [ ] Thread network formed
- [ ] Silicon Labs Multiprotocol add-on installed
- [ ] Thread integration added to HA
- [ ] OTBR integration added to HA
- [ ] First Thread device commissioned
- [ ] Thread device appears in HA

---

**Last Updated:** May 9, 2026

