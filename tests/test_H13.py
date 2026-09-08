import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "host"))
import h13_cli as m


class TestFrames(unittest.TestCase):
    def test_build_deauth(self):
        raw = m.build_deauth(b"\xff" * 6, b"\x00\x11\x22\x33\x44\x55")
        f = m.parse_frame(raw)
        self.assertEqual(f["type"], 0)
        self.assertEqual(f["subtype"], 12)
        self.assertEqual(f["reason"], 7)
        self.assertEqual(f["dst"], "ffffffffffff")

    def test_build_disassoc(self):
        raw = m.build_disassoc(b"\xff" * 6, b"\x00\x11\x22\x33\x44\x55")
        f = m.parse_frame(raw)
        self.assertEqual(f["subtype"], 10)
        self.assertEqual(f["reason"], 8)

    def test_short(self):
        self.assertIsNone(m.parse_frame(b"\x01"))


if __name__ == "__main__":
    unittest.main()
