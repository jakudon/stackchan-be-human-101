# Be Human 101 — v0.1.0 Release Notes

**Release type:** Public source preview — face module only

## Included
- 22 expressive face states for StackChan / M5Stack CoreS3
- `face/reference/EmotionController.cpp` and `.h` source reference
- `face/EmotionModes.h` and an integration patch
- Integration guidance for existing StackChan firmware
- Static source validation via GitHub Actions

## Validation
- Maintainer-reported on-device test: all 22 face modes displayed in sequence successfully
- GitHub Actions static face-reference check: passed on the initial workflow run
- Clean-room build against a fresh firmware checkout: **not yet verified**
- Full firmware, Gemini credentials, and third-party servo libraries: **not included**

## Installation scope
This repository is **not a standalone PlatformIO firmware project** and does not provide a flashable firmware image. Integrate the face reference with a compatible upstream StackChan firmware, following [face/README.md](face/README.md).

## Licensing
Original Be Human 101 contributions are available under the [MIT License](LICENSE). Upstream-derived material retains its copyright and license notices in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## Credits
Developed by [Jakudon](https://github.com/jakudon). Built upon the StackChan ecosystem and [taranton/stackchan-gemini-firmware](https://github.com/taranton/stackchan-gemini-firmware).

## Known limitations
- No independently reproducible full firmware build is claimed.
- External sensors, AI integrations, long-term memory, and advanced behavior are ongoing work, not part of this source preview.
