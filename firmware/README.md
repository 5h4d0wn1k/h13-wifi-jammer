# WiFi Jammer Firmware

## Purpose

Proof-for-study: frame-encoding simulation only. NO live interference with any third-party network, ever.

## Board

- **Board**: ESP32-C6
- **FQBN**: `esp32:esp32:esp32c6`
- **Sketch**: `h13_wifi_jammer/h13_wifi_jammer.ino`

## Wiring

```
Standalone ESP32-C6. Keep in a Faraday enclosure during any lab check; do not radiate.
```

## Build

```bash
arduino-cli compile --fqbn esp32:esp32:esp32c6 firmware/h13_wifi_jammer
# upload (example, ESP32-C6):
# arduino-cli upload --fqbn esp32:esp32:esp32c6 --port /dev/ttyACM0 firmware/h13_wifi_jammer
```

## Runtime

See the root README "IMPORTANT" section before powering on. This firmware is
for authorized own-lab study. Serial console exposes the interactive command
set described in the root README. All identifiers in the sketch are
placeholders (`lab-*` SSIDs, `00:11:22:33:44:55`, RFC 5737 / example.com).
