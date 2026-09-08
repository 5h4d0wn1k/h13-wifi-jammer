# H13 — WiFi Jammer

Channel flooding, deauth broadcast, and WiFi DoS testing for ESP32-C6.

## Overview

WiFi denial-of-service testing tool for authorized security assessments:
- Deauth broadcast (802.11 management frames)
- Channel data flooding
- Multi-channel sweep mode
- Beacon frame flooding with random SSIDs
- Real-time packet count and status monitoring

## Hardware

| Component | Connection | Role |
|-----------|------------|------|
| ESP32-C6 DevKit | Main board | WiFi packet injection |

## Jamming Modes

| Mode | Description | Speed |
|------|-------------|-------|
| 1 | Deauth broadcast | 10 Hz |
| 2 | Channel data flood | 100 Hz |
| 3 | Channel sweep (all 13) | 20 Hz |
| 4 | Beacon flood | 50 Hz |

## Serial Output

```
[JAM] Mode: DEAUTH
[DEAUTH] Sent: 100 (ch 6)
[DEAUTH] Sent: 200 (ch 6)
[STOP] Packets sent: 347
```

## Build & Flash

```bash
arduino-cli compile --fqbn esp32:esp32:esp32c6 firmware/
arduino-cli upload --fqbn esp32:esp32:esp32c6 --port /dev/ttyUSB0 firmware/
```

## Legal Disclaimer

## IMPORTANT: Read before use.

This project is provided for **educational and authorized security testing purposes only**.

### Authorization Requirements
- You MUST have explicit written permission from the network owner before using this tool
- Unauthorized interception of network communications is illegal under federal and state laws
- This tool should ONLY be used on networks you own or have written authorization to test

### Spectrum Regulatory Notes (2.4 GHz ISM)
- 2.4 GHz ISM channels 1–13 are license-exempt but subject to
  no-harmful-interference and power/duty rules (FCC Part 15, ETSI
  EN 300 328). Deliberate interference (deauth/flood emission) against
  anything but your own hardware is unlawful (FCC 47 U.S.C. § 333).
- Emissions must be confined to an isolated shielded bench, own APs
  and clients only, with attenuators on any transmit path.

### No Third-Party Disruption
Real deauth/flood emission against any third-party network, AP, client,
or spectrum user is out of scope. Proofs here are 802.11 frame
fixtures and offline simulation only; live triggers additionally
require the `LAB_*` allowlist AND explicit `--yes` confirmation.

### Legal Framework
- **Computer Fraud and Abuse Act (CFAA)**: Unauthorized access to computer systems is a federal crime
- **Wiretap Act (18 U.S.C. § 2511)**: Interception of electronic communications without consent is illegal
- **State Laws**: Many states have additional computer crime and wiretapping statutes
- **GDPR/CCPA**: Data collection may be subject to privacy regulations

### Acceptable Use
- Testing security of your own networks
- Authorized penetration testing with written scope
- Academic research in controlled lab environments
- Security education and training

### Prohibited Use
- Intercepting communications on networks you don't own
- Attacking infrastructure without authorization
- Any activity that violates applicable laws or regulations
- Commercial use without proper licensing

### No Warranty
This software is provided "AS IS" without warranty of any kind. The author is not responsible for any misuse or damage caused by this software.

### Responsible Disclosure
If you discover vulnerabilities using this tool, follow responsible disclosure practices:
1. Report to the vendor/owner privately
2. Allow reasonable time for remediation
3. Do not exploit beyond proof of concept

## Live Lab Test Plan

Run ONLY on an isolated, authorized own-lab bench against devices, networks,
and spectrum **you own**. No third-party callers, bystanders, or spectrum users
may be within range of any test transmission.

1. **Isolate** - Put the DUT in a shielded/Faraday enclosure or a room with no
   third-party devices in range. Use attenuators on any transmit path.
2. **Own devices only** - Every target (AP, remote, tag, GPS module, drone FC,
   receiver) must be your own hardware.
3. **Lowest power, shortest duration** - Start at minimum TX power / duty cycle
   and use only the seconds needed.
4. **Record** - Save before/after logs to `reports/` (git-ignored). Never
   capture or store third-party traffic.
5. **Cleanup** - Restore placeholder SSIDs (`lab-*`), MACs (`00:11:22:33:44:55`),
   example.com / RFC5737 addresses, and clear any captured data from the device.

> Jammer / spoofer / replay projects are **proofs for study and simulation**
> only. They refuse live interference scenarios: a live bench trigger requires
> the `LAB_*` allowlist environment variable AND explicit `--yes` confirmation,
> and even then only against your own hardware in a shielded bench.

## Metrics

| Metric | Target | Where |
|---|---|---|
| Firmware compile | `arduino-cli compile --fqbn esp32:esp32:esp32c6 firmware/h13_wifi_jammer` PASS | CI/local |
| Host helper | `python3 host/h13_cli.py --demo` exits 0 (offline) | host/ |
| Unit tests | `python3 -m unittest discover -s tests` passes | tests/ |
| py_compile | every `host/*.py` compiles clean | CI/local |

## License

MIT
