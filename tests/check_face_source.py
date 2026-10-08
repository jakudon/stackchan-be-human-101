#!/usr/bin/env python3
"""Static source checks for the Be Human 101 reference face controller.

Run from the repository root: python tests/check_face_source.py
This is NOT a C++ compile, device test, or license/security audit.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "face/reference/EmotionController.h"
SOURCE = ROOT / "face/reference/EmotionController.cpp"

EXPECTED = (
    "Neutral", "Listening", "Speaking", "Thinking", "Looking",
    "Happy", "Excited", "Laughing", "Sad", "Crying", "Surprised",
    "Scared", "Angry", "Annoyed", "Bored", "Embarrassed",
    "Curious", "Dizzy", "Love", "Found", "Error", "Sleep",
)
PUBLIC_API = ("begin", "setEmotion", "loop", "holdDisplay", "releaseDisplay")
PRIVATE_METHODS = (
    "parseMode", "hsvToRgb", "wave8", "scale8", "externalPowerPresent",
    "sleepVisualOffDue", "enterSleepDisplayOff", "restoreSleepDisplay",
    "setLed", "setAll", "refresh", "drawLabel", "renderFace",
    "renderNeutral", "renderListening", "renderSpeaking", "renderThinking",
    "renderLooking", "renderHappy", "renderAngry", "renderFound",
    "renderError", "renderSleep",
)

def main() -> int:
    errors = []
    for path in (HEADER, SOURCE):
        if not path.is_file():
            errors.append(f"Missing: {path.relative_to(ROOT)}")
    if errors:
        print("\n".join(errors))
        return 1

    h = HEADER.read_text(encoding="utf-8")
    cpp = SOURCE.read_text(encoding="utf-8")

    enum_match = re.search(r"enum\s+class\s+Mode\s*\{([^}]+)\}", h, re.S)
    if not enum_match:
        errors.append("Mode enum not found in header")
    else:
        modes = tuple(x.strip() for x in enum_match.group(1).split(",") if x.strip())
        if modes != EXPECTED:
            errors.append(f"Mode list mismatch: expected {EXPECTED}, got {modes}")

    face_match = re.search(r"void\s+EmotionController::renderFace\s*\(\s*\)\s*\{", cpp)
    if not face_match:
        errors.append("renderFace() implementation missing")
    else:
        # renderFace contains all LCD expression cases; other switches handle LEDs.
        face_section = cpp[face_match.end():cpp.find("void EmotionController::renderNeutral()", face_match.end())]
        missing = [mode for mode in EXPECTED if not re.search(r"case\s+Mode::" + mode + r"\s*:", face_section)]
        if missing:
            errors.append(f"LCD renderer missing modes: {', '.join(missing)}")

    for method in PUBLIC_API + PRIVATE_METHODS:
        if not re.search(r"\b" + method + r"\s*\(", h):
            errors.append(f"Missing header declaration: {method}")
        if not re.search(r"\bEmotionController::" + method + r"\s*\(", cpp):
            errors.append(f"Missing source definition: {method}")

    if errors:
        for issue in errors:
            print("FAIL:", issue)
        return 1
    print(f"PASS: {len(EXPECTED)} face modes and {len(PUBLIC_API) + len(PRIVATE_METHODS)} controller method declarations/definitions found.")
    print("NOTE: Static source checks only; build and on-device validation still required.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
