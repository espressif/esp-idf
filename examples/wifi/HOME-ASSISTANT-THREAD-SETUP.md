# Home Assistant Thread & OpenThread Border Router Setup

## Prerequisites

✅ Home Assistant installed and running (2023.8+)  
✅ ESP32-H2 N4 on COM6 flashed with OTBR firmware  
✅ ESP32-H2-mini flashed with RCP firmware  
✅ OTBR connected to WiFi  
✅ RCP paired with OTBR via UART

---

## Phase 1: Home Assistant Add-ons Setup

### Step 1.1: Install Silicon Labs Multiprotocol Add-on

**Why?** The Multiprotocol add-on enables Thread radio hardware abstraction in Home Assistant.

1. Open **Home Assistant**
2. Go to **Settings → Add-ons & Integrations → Add-on Store** (bottom right)
3. Search for `"multiprotocol"` or `"Silicon Labs"`
4. Click the **Multiprotocol** add-on by Silicon Labs
5. Click **Install**
6. After installation:
   - Click **Start** 
   - Enable **Start on boot** (toggle)
   - Click **Configuration** tab and verify settings
   - Click **Logs** to see if it started without errors

**Expected log output:**
```
[info] Starting Silicon Labs Multiprotocol Radio...
[info] Radio type: thread-rcp (autodetected)
[info] Serial port: /dev/ttyUSB0 or similar
[info] Radio initialized successfully
```

---

## Phase 2: Thread Integration Setup

### Step 2.1: Add Thread Integration

1. Go to **Settings → Devices & Services**
2. Click **Create Automation** (blue button, bottom right)
3. Click **Create Entry** (if Thread not already detected)
4. Search for **"Thread"**
5. Select **Thread** from results
6. If auto-detected:
   - Click **Submit**
   - System will find Thread network automatically
7. If manual entry needed:
   - Thread Network ID: [Your Extended PAN ID from OTBR]
   - Channel: [From OTBR `channel` command]
   - Network Key: [From OTBR `networkkey` command]
   - Click **Submit**

**Expected result:** Integration shows "1 device" (OTBR itself)

---

## Phase 3: OpenThread Border Router Integration

### Step 3.1: Auto-discovery (Recommended)

If your OTBR supports mDNS (default):

1. Go to **Settings → Devices & Services**
2. You should see **"OpenThread Border Router"** auto-discovered
3. Click to add
4. Verify connection status shows **"Connected"**
5. Review Thread network credentials shown

### Step 3.2: Manual Addition (If auto-discovery fails)

1. Go to **Settings → Devices & Services**
2. Click **Create Entry**
3. Search for **"OpenThread Border Router"**
4. Select from results
5. Enter OTBR details:
   - **Hostname or IP:** `openthread-border-router.local` (preferred) or `192.168.1.XXX`
   - Click **Submit**
6. Wait for connection verification
7. If successful, click **Finish**

---

## Phase 4: Thread Device Commissioning

### Step 4.1: Prepare Device for Commissioning

1. Have a Thread device ready:
   - Examples: Philips Hue bulb, Eve Home device, Matter device with Thread
   - Device must support Thread protocol
2. Put device in **commissioning mode**:
   - Instructions vary by device (usually hold button for 5-10 seconds)
   - Device should indicate ready status (LED blinking, etc.)

### Step 4.2: Commission Device to Home Assistant

1. Go to **Settings → Devices & Services → Thread**
2. Click **Commission Device** (blue button)
3. You'll see two options:
   - **Scan QR code:** Use device camera/QR code reader
   - **Manual entry:** Type 11-digit code manually
4. Scan or enter code from your Thread device
5. Wait for commissioning (typically 30-60 seconds)

**Expected result:** Device appears in Thread integration and Home Assistant

---

## Phase 5: Verification & Testing

### Step 5.1: Verify Thread Network Status

1. Go to **Settings → Devices & Services → Thread**
2. Check **"Dataset"** section shows:
   - ✓ Network Name
   - ✓ Extended PAN ID
   - ✓ Channel
   - ✓ Network Key

3. Click **OTBR device** to see:
   - Network state
   - Connected devices
   - Link quality

### Step 5.2: Test Thread Communication

1. Commission a simple Thread device (Hue bulb recommended)
2. After commissioning, device should appear in Home Assistant
3. Try controlling the device:
   - If successful, Thread is working
   - If fails, check:
     - Device power/network
     - OTBR has clear WiFi signal
     - No physical obstructions

### Step 5.3: Verify OTBR via Console

Connect to OTBR serial console to verify:

```powershell
idf.py -p COM6 monitor
```

In OTBR console:
```
# Check state
state
# Should output: "leader" or "router"

# Check child nodes (commissioned devices)
childlist
# Should list commissioned devices

# Check WiFi connectivity
wifi_status
# Should show connected with IP address
```

---

## Common Issues & Solutions

### Issue: Thread Integration not appearing

**Symptom:** Thread integration doesn't auto-discover or can't add manually

**Solutions:**
1. Restart Home Assistant:
   - Settings → System → Restart Home Assistant
2. Check Multiprotocol add-on:
   - Verify it's running: Settings → Add-ons → Multiprotocol
   - Check logs for errors
   - Restart if needed
3. Check OTBR access:
   - Ping OTBR from Home Assistant host
   - `ping openthread-border-router.local`
   - If fails, check mDNS (port 5353 UDP) in firewall

---

### Issue: OpenThread Border Router Integration fails to connect

**Symptom:** "Connection refused" or "Unable to connect to OTBR"

**Solutions:**
1. Verify OTBR is running:
   - Connect to serial console: `idf.py -p COM6 monitor`
   - Type `state` — should show "leader" or "router"
2. Verify WiFi connection:
   - Type `wifi_status` in console
   - Should show IP address
3. Check network access:
   - From Home Assistant, ping OTBR: `ping 192.168.1.XXX`
   - If no response, check firewall rules
4. Restart OTBR:
   - Restart from console: `reset`
   - Or power cycle ESP32-H2 N4 OTBR board

---

### Issue: Thread device won't commission

**Symptom:** Commissioning times out or fails

**Solutions:**
1. Verify device is in commissioning mode:
   - Re-enter commissioning mode on device
   - Some devices have limited commissioning window (5-10 minutes)
2. Check Thread network:
   - Verify OTBR state: `state` (should be "leader")
   - Check channel: `channel`
   - Verify RCP is connected
3. Try manual commissioning code:
   - Use 11-digit code from device instead of QR scan
4. Move device closer to OTBR
5. Check WiFi interference:
   - Both WiFi and Thread use 2.4 GHz band
   - Reduce channel overlap if possible

---

### Issue: Thread device loses connection periodically

**Symptom:** Device connects but frequently goes offline

**Solutions:**
1. Check OTBR signal strength:
   - Verify WiFi signal to OTBR is strong
   - Consider moving OTBR antenna or adding WiFi extender
2. Check Thread network health:
   - Monitor OTBR logs: `idf.py -p COM6 monitor`
   - Look for errors or warnings
3. Reduce interference:
   - Change WiFi channel away from Thread channels (11-26)
   - Thread uses 2.4 GHz, so WiFi channels 1-6 recommended
4. Check device power:
   - Thread devices need adequate power
   - Battery devices may need replacement batteries
5. Verify RCP stability:
   - Check ESP32-H2 power supply
   - Verify UART cable connection (no loose wires)

---

### Issue: Cannot reach Home Assistant from OTBR or vice versa

**Symptom:** Firewall/network blocks communication

**Solutions:**
1. Check Home Assistant network:
   - Verify HA can reach WiFi network
   - Check firewall isn't blocking port 5353 (mDNS)
2. Check OTBR network:
   - Verify OTBR WiFi connection: `wifi_status`
   - Ensure OTBR is on same network as Home Assistant
3. Allow mDNS discovery:
   - If using custom DNS, add mDNS (224.0.0.251:5353)
   - Consider using `.local` hostnames for easier setup

---

## Advanced Configuration

### Customizing Thread Network

To customize Thread network settings from OTBR console:

```
# Set network name
networkname "My Thread Network"

# Set specific channel (11-26)
channel 15

# Set specific PAN ID (hex)
panid 1234

# Set specific Extended PAN ID
extpanid 0123456789abcdef

# Set custom network key
networkkey fedcba9876543210fedcba9876543210

# Commit changes
dataset commit
```

---

### Monitoring Thread Network

Create Home Assistant automations to monitor Thread health:

```yaml
- alias: "Thread Network Alert"
  trigger:
    platform: state
    entity_id: thread.operational_dataset  # Monitor Thread dataset changes
  action:
    service: notify.notify
    data:
      title: "Thread Network Changed"
      message: "Thread network has changed: {{ trigger.to_state.state }}"
```

---

## Security Notes

⚠️ **Important:** Protect your Thread Network Key!

- The network key is a 32-character hex string
- Sharing this exposes your Thread network to eavesdropping
- Treat like a WiFi password (keep confidential)
- Store backups in secure location (password manager, encrypted USB)
- If compromised, regenerate with: `dataset init new` then `dataset commit`

---

## Support & Resources

- **Home Assistant Thread Docs:** https://www.home-assistant.io/integrations/thread/
- **OpenThread Documentation:** https://openthread.io/guides/border-router
- **Thread Protocol Overview:** https://www.threadgroup.org
- **Home Assistant Community:** https://community.home-assistant.io

---

## Next Steps Checklist

- [ ] Install Multiprotocol add-on
- [ ] Add Thread integration
- [ ] Add OpenThread Border Router integration
- [ ] Commission first Thread device
- [ ] Test device control from Home Assistant
- [ ] Create automation using Thread device
- [ ] Monitor Thread network health
- [ ] Plan Thread device expansion

