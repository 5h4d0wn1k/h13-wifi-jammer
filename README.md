> **⚠️ EDUCATIONAL USE ONLY — AUTHORIZED TESTING ONLY.**
> This project exists for education, research, and **defense of systems you own
> or hold explicit written authorization to assess**. Unauthorized use is
> prohibited and may be illegal. Read [ETHICS.md](ETHICS.md) and
> [SCOPE.md](SCOPE.md) before use. Use at your own risk; **AS IS**, no warranty.

# H13 — WiFi Jamming & Resilience Research (ESP32-C6)

![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)
![GitHub stars](https://img.shields.io/github/stars/5h4d0wn1k/h13-wifi-jammer)
![Last commit](https://img.shields.io/github/last-commit/5h4d0wn1k/h13-wifi-jammer)
![GitHub issues](https://img.shields.io/github/issues/5h4d0wn1k/h13-wifi-jammer)

**Wi-Fi denial-of-service and jamming research** firmware + host tooling for the ESP32-C6 — 802.11 deauth broadcast, channel flooding, beacon flood, and multi-channel sweep modes for authorized interference testing against your **own-lab networks** only.

## Why

Wireless networks are trivially disruptible by an attacker with a £5 radio. This project studies **wireless DoS resilience** from the defender's side: understanding how deauth frames, channel floods, and beacon storms degrade availability lets blue teams test their own detection and hardening. All interference scenarios are **proofs for study and simulation only** — emission against anything but your own hardware is unlawful under FCC 47 U.S.C. § 333 and the Wiretap Act. Live triggers require a `LAB_*` allowlist **and** an explicit `--yes`, and tests are confined to an isolated, shielded bench with attenuators.

## Features

- **Deauth broadcast** (802.11 management frames) at 10 Hz.
- **Channel data flood** at 100 Hz.
- **Multi-channel sweep** across all 13 channels at 20 Hz.
- **Beacon flood** with random SSIDs at 50 Hz.
- **Real-time packet count and status** over serial.
- **Offline host demo** (`host/h13_cli.py --demo`) with hex frame fixtures.

## Quickstart

### Firmware (ESP32-C6)

```bash
arduino-cli compile --fqbn esp32:esp32:esp32c6 firmware/
arduino-cli upload --fqbn esp32:esp32:esp32c6 --port /dev/ttyUSB0 firmware/
```

### Host helper (offline demo, no radio needed)

```bash
python3 host/h13_cli.py --demo
```

### Tests

```bash
python3 -m unittest discover -s tests -v
```

> ⚠️ See [ETHICS.md](ETHICS.md) and [SCOPE.md](SCOPE.md) before any bench work. Live triggers require the `LAB_*` allowlist **and** explicit `--yes`.

## Project structure

```
h13-wifi-jammer/
├── firmware/            # ESP32-C6 Arduino sketch (h13_wifi_jammer.ino)
├── host/                # h13_cli.py, hw_common.py (offline host demo)
├── fixtures/            # hex frame logs (e.g. frames.txt)
├── tests/               # offline unittest suite
└── ETHICS.md, SCOPE.md  # authorized-use & spectrum rules
```

## Documentation

- [ETHICS.md](ETHICS.md) — authorized-use policy
- [SCOPE.md](SCOPE.md) — wireless lab scope
- [SECURITY.md](SECURITY.md) — security policy
- [CONTRIBUTING.md](CONTRIBUTING.md) — contribution guide
- [firmware/README.md](firmware/README.md) — firmware build notes

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). No enhancements that enable third-party interference.

## License

MIT. See [LICENSE](LICENSE).