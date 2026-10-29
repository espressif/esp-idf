| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 |
| ----------------- | ----- | -------- | -------- |

# `bt` component unit tests

When adding new test cases, check if the `depends_components` list in `.build-test-rules.yml` needs to be updated to include additional components. The test app will only be built and tested when these components are modified.

To build and run this test app, using esp32c3 target for example:

```bash
idf.py set-target esp32c3
idf.py build flash monitor
```

To run tests using pytest:

```bash
idf.py set-target esp32c3
idf.py build
pytest --target=esp32c3
```

## Bond reuse regression

The `[bond_reuse]` Unity cases exercise the real `smp_check_auth_req()` action
with a synthetic device record. Linker wrappers isolate device lookup, key
clearing and subsequent state-machine events while the action runs. Calls outside
this fixture pass through to the real stack. No Bluetooth controller or peer is
needed for these cases.

Build for ESP32 as above, then enter `[bond_reuse]` in the Unity test menu.
The cases cover both roles, preservation of existing keys on re-encryption,
replacement after successful new pairing (legacy and Secure Connections), failed
encryption and a missing device record. With the unconditional key clear restored,
`SMP retains reused bond keys for both roles` fails because the clear count is one.

This action-level test does not emulate NVS persistence or over-the-air SMP.
For a hardware regression check:

1. Pair and bond once with a BLE peer that requires authenticated GATT access.
2. Reboot without erasing NVS, connect using the retained bond, and verify a
   protected GATT operation succeeds with its actual completion status.
3. Disconnect normally, reconnect and request encryption again. Verify that no
   new pairing confirmation is requested and the protected operation still works.
4. Repeat disconnect/reconnect, then reboot and repeat. Check bond presence as
   well as protected GATT responses. A bond list entry alone does not prove the
   in-memory keys remain usable.
