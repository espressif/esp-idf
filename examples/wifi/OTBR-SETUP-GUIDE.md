# OpenThread Border Router Setup Guide
## ESP32-H2 N4 OTBR (COM6) + Home Assistant

---

## Phase 1: Build & Flash OTBR Firmware on ESP32-H2 N4

### Step 1.1: Build OTBR for ESP32-H2 N4
```powershell
cd c:\Users\chuky\esp-idf\examples\openthread\ot_br
idf.py set-target esp32h2
idf.py menuconfig
# (No changes needed unless customizing WiFi settings)
idf.py build
```

### Step 1.2: Flash OTBR to ESP32-H2 N4
```powershell
# Identify ESP32-H2 N4 COM port (usually COM6)
idf.py -p COM6 flash
idf.py -p COM6 monitor
```

**Expected output:**
```
I (xxx) main: OpenThread Border Router started
I (xxx) openthread: [NOTE] ========================================
I (xxx) openthread: [NOTE] OpenThread Network Formed
I (xxx) openthread: [NOTE] ========================================
```

---

## Phase 2: Configure OTBR with WiFi

### Step 2.1: Connect to OTBR Serial Console
```powershell
idf.py -p COM6 monitor
```

### Step 2.2: Configure WiFi in OTBR Console

Use the WiFi priority configuration you created. Run these commands in the OTBR serial console:

```
# Priority 1: TP-Link_merge_MLO
wifi_set_sta_config --ssid "TP-Link_merge_MLO" --password "S2s5s0S1" --priority 1

# Priority 2: TP-Link_merge
wifi_set_sta_config --ssid "TP-Link_merge" --password "S1s0s5S2" --priority 2

# Priority 3: TP-Link_IoT_2.4
wifi_set_sta_config --ssid "TP-Link_IoT_2.4" --password "T1t0t5T2" --priority 3

# Priority 4: TP-Link_IoT_5G
wifi_set_sta_config --ssid "TP-Link_IoT_5G" --password "T2t5t0t1" --priority 4

# Connect WiFi
wifi_connect

# Check WiFi status
wifi_status
```

**Expected output after wifi_connect:**
```
I (xxx) esp_netif_handlers: sta ip: 192.168.1.XXX, mask: 255.255.255.0, gw: 192.168.1.1
I (xxx) mDNS_Advertise: ..._hap._tcp
```

### Step 2.3: Verify OTBR Network

In OTBR console:
```
state
# Should show: "leader" or "router"

channel
# Shows current Thread channel (default 11-26)

panid
# Shows Thread PAN ID (auto-generated if not set)

extpanid
# Shows extended PAN ID
```

---

## Phase 3: Verify RCP Connection (ESP32-H2)

### Step 3.1: Check RCP Status
In OTBR console:
```
rcp_version
# Should show RCP version info

rcp_state
# Should show: "RADIO_STATE_RECEIVE" or similar
```

If RCP doesn't respond:
1. Verify UART cable connection (TX/RX crossed)
2. Check RCP baud rate: `115200` by default
3. Reflash ESP32-H2 with latest RCP firmware

---

## Phase 4: Get OTBR Network Info

In OTBR console, collect these for Home Assistant:

```
# Get Thread Credentials (needed for Thread integration)
networkkey
extpanid
panid
channel

# Get OTBR IP address
ipaddr
# Look for global unicast address (starts with fd)

# Get Network Name (optional)
networkname
```

**Save this info:**
```
Thread Network ID (Extended PAN ID): ________________
Thread Channel: ________________
Thread Network Key: ________________
OTBR IPv6 Address (fd...): ________________
OTBR IPv4 Address (192.168...): ________________
```

---

## Phase 5: Home Assistant Configuration

### Step 5.1: Enable Thread Support in Home Assistant

1. **Update Home Assistant** (if needed)
   - Settings → System → Updates
   - Update to latest version (2023.8+ required)

2. **Install Silicon Labs Multiprotocol Add-on**
   - Settings → Add-ons → Add-on Store
   - Search: "Multiprotocol" or "Silicon Labs"
   - Install: "Silicon Labs Multiprotocol"

### Step 5.2: Add Thread Integration

1. **Add OpenThread Border Router**
   - Settings → Devices & Services
   - Click "+ Create Automation"
   - Search for "Thread"
   - Select "OpenThread Border Router"
   - Enter OTBR hostname/IP: `openthread-border-router.local` or `192.168.1.XXX`

### Step 5.3: Configure Thread Integration

After OTBR is discovered:
1. Click on the OTBR integration
2. Verify connection: Should show "Connected"
3. Note the Thread network credentials displayed

### Step 5.4: Enable Thread Support

1. Settings → Devices & Services → Integrations
2. Find "Thread" integration
3. Enable if not already enabled
4. Verify "Dataset" shows your Thread network credentials

---

## Phase 6: Test Thread Connectivity

### Step 6.1: Commission Thread Device

1. Have a Thread device ready (Philips Hue, Eve, Matter device, etc.)
2. Put device in commissioning mode
3. Home Assistant → Settings → Devices & Services → Thread
4. Click "+ Commission Device"
5. Scan QR code or enter 11-digit code

### Step 6.2: Verify in OTBR Console

After commissioning:
```
childlist
# Should show commissioned devices

state
# Should show "leader" or "router"

aahistory
# Shows auto-start history
```

---

## Troubleshooting

### OTBR won't connect to WiFi
- Check WiFi password spelling
- Verify SSID is correct
- Try different priority network
- Check WiFi signal strength

### OTBR not forming Thread network
```
# In OTBR console:
dataset init new
dataset set channelmask 0x07fff800
dataset set networkkey [KEY]
dataset set panid [PANID]
dataset set extpanid [EXTPANID]
dataset commit

# Restart OTBR
reset
```

### RCP not connecting
- Verify UART baud rate: 115200
- Check TX/RX wiring (should be crossed)
- Verify ESP32-H2 has RCP firmware
- Reflash ESP32-H2: `idf.py -p COM7 flash`

### Home Assistant can't find OTBR
- Verify OTBR has IPv4 address: `ipaddr` in console
- Ping OTBR from Home Assistant host: `ping openthread-border-router.local`
- Check firewall rules allow mDNS (port 5353 UDP)
- Restart Home Assistant integration

### Thread device won't commission
- Verify device is in commissioning mode
- Check Thread network credentials match
- Ensure sufficient OTBR signal strength
- Try moving device closer to OTBR

---

## Reference Commands

### OTBR CLI Commands (in serial console)

**Network Management:**
- `state` — Show current state (leader/router/child/detached)
- `channel` — Show/set Thread channel
- `networkkey [KEY]` — Show/set network key
- `panid [PANID]` — Show/set PAN ID
- `extpanid [PANID]` — Show/set extended PAN ID
- `networkname [NAME]` — Show/set network name

**Status:**
- `childlist` — List child nodes
- `neighbors` — List neighbor nodes
- `ipaddr` — Show IPv6 addresses
- `ifconfig` — Show network interface config

**WiFi Management:**
- `wifi_status` — Show WiFi connection status
- `wifi_connect` — Connect to WiFi
- `wifi_disconnect` — Disconnect from WiFi
- `wifi_set_sta_config` — Configure WiFi station

**Diagnostics:**
- `version` — Show firmware version
- `rcp_version` — Show RCP version
- `rcp_state` — Show RCP state

---

## Next Steps

1. ✅ Verify OTBR is online and connected to WiFi
2. ✅ Verify RCP (ESP32-H2) is connected
3. ✅ Add OTBR to Home Assistant
4. ✅ Commission first Thread device
5. ✅ Create Thread-based automations

---

## Support Resources

- [OpenThread Documentation](https://openthread.io/guides/border-router)
- [ESP-IDF OpenThread Examples](https://github.com/espressif/esp-idf/tree/master/examples/openthread)
- [Home Assistant Thread Setup](https://www.home-assistant.io/integrations/thread/)
- [Thread Network Overview](https://www.threadgroup.org)

