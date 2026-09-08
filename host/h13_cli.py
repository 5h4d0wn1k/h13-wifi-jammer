#!/usr/bin/env python3
"""H13 - WiFi Jammer host helper: SIMULATION-ONLY 802.11 frame builder.
No live interference is ever generated. Proof-for-study only.
Educational/authorized own-lab use only (see README "IMPORTANT").
"""
import argparse
import os
import sys

MOD = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, MOD)
from hw_common import DEMO_TAG, read_target


def build_deauth(dst, src, reason=7):
    fc = 0x00C0
    hdr = bytearray(26)
    hdr[0] = fc & 0xFF
    hdr[1] = fc >> 8
    hdr[2:4] = (0x3A, 0x01)  # duration
    hdr[4:10] = dst
    hdr[10:16] = src
    hdr[16:22] = src  # BSSID
    hdr[22:24] = (0x00, 0x00)  # sequence control
    hdr[24] = reason & 0xFF
    hdr[25] = reason >> 8
    return bytes(hdr)


def build_disassoc(dst, src, reason=8):
    fc = 0x00A0
    hdr = bytearray(26)
    hdr[0] = fc & 0xFF
    hdr[1] = fc >> 8
    hdr[2:4] = (0x3A, 0x01)
    hdr[4:10] = dst
    hdr[10:16] = src
    hdr[16:22] = src
    hdr[22:24] = (0x00, 0x00)
    hdr[24] = reason & 0xFF
    hdr[25] = reason >> 8
    return bytes(hdr)


def parse_frame(raw):
    if len(raw) < 26:
        return None
    fc = raw[0] | (raw[1] << 8)
    ftype = (fc >> 2) & 0x3
    fsub = (fc >> 4) & 0xF
    reason = None
    if ftype == 0 and fsub in (10, 12):
        reason = raw[24] | (raw[25] << 8)
    return {"type": ftype, "subtype": fsub,
            "dst": raw[4:10].hex(), "src": raw[10:16].hex(),
            "reason": reason}


def analyze(text):
    frames = []
    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        try:
            raw = bytes.fromhex(line)
        except ValueError:
            continue
        f = parse_frame(raw)
        if f:
            frames.append(f)
    return frames


def run_demo():
    print("=== H13 802.11 frame construction (SIMULATION ONLY) ===")
    dst = bytes.fromhex("FF:FF:FF:FF:FF:FF".replace(":", ""))
    src = bytes.fromhex("00:11:22:33:44:55".replace(":", ""))
    d = build_deauth(dst, src)
    a = build_disassoc(dst, src)
    for raw, name in ((d, "DEAUTH"), (a, "DISASSOC")):
        f = parse_frame(raw)
        print("  %s type=%d sub=%d src=%s dst=%s reason=%s (%d bytes)"
              % (name, f["type"], f["subtype"], f["src"], f["dst"],
                 f["reason"], len(raw)))
    print("  sweep sim: ch1..13 hop interval 50ms (predict 260ms/full sweep)")
    print(DEMO_TAG)
    return 0


def main(argv=None):
    p = argparse.ArgumentParser(
        description="H13 WiFi jammer - simulation-only frame builder")
    p.add_argument("--demo", action="store_true", help="offline demo (exit 0)")
    p.add_argument("--file", help="hex frame log")
    args = p.parse_args(argv)
    if args.demo or not args.file:
        return run_demo()
    for f in analyze(open(args.file).read()):
        print(f)
    return 0


if __name__ == "__main__":
    sys.exit(main())
