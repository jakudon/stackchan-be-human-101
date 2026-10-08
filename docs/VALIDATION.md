# CoreS3 Face Module Validation

This checklist helps maintainers test the **Be Human 101 face renderer** in an upstream-derived StackChan K151 firmware project.

## Environment

- Hardware: M5Stack StackChan K151 / CoreS3
- Development: Visual Studio Code + PlatformIO IDE
- PlatformIO environment: `m5stack-cores3`
- Firmware: a compatible upstream-derived project with its original `platformio.ini` and dependencies
- Source under test: `face/reference/EmotionController.cpp` and `face/reference/EmotionController.h`

This repository **does not contain a complete firmware or a standalone build project**.

## Static source validation

From the repository root, run:

```powershell
python tests/check_face_source.py
```

This checks that the published header and source define the expected 22 LCD modes and controller methods. It does **not** compile the firmware or exercise hardware.

## Build

In the firmware project's VS Code terminal:

```powershell
py -m platformio run -e m5stack-cores3
```

The maintainer reported `[SUCCESS]` on 2026-10-08 for their local firmware. A clean-room integration build of the published reference pair has **not** yet been performed.

## Hardware checklist

Test on a device only after taking a backup of known-good firmware and settings.

- [ ] Device boots and original screen/head behavior still works
- [ ] Neutral eyes display correctly without a mouth
- [ ] Listening, speaking, thinking, and looking animations work
- [ ] All 22 modes render without crash, freeze, or corrupted display
- [ ] LED effects work without interfering with real-time audio
- [ ] Speaking mouth animates smoothly during speech
- [ ] `holdDisplay()` preserves a camera preview without face overwrites
- [ ] `releaseDisplay()` restores face rendering after preview
- [ ] Repeated camera capture/preview cycles work (known area requiring verification)
- [ ] Sleep dims/turns off the display as designed, and waking restores it
- [ ] No excessive memory growth or audio dropouts during extended use

### 22 modes to exercise

`neutral`, `listening`, `speaking`, `thinking`, `looking`, `happy`, `excited`, `laughing`, `sad`, `crying`, `surprised`, `scared`, `angry`, `annoyed`, `bored`, `embarrassed`, `curious`, `dizzy`, `love`, `found`, `error`, `sleep`.

## Report a test

Record:

- Hardware revision and firmware commit/version
- Build environment and output
- Mode or action tested
- Expected behavior versus observed behavior
- Serial log excerpt with secrets and device identifiers removed
- Optional screenshot or video of the face

Do not share Wi-Fi credentials, API keys, Cloudflare secrets, or full unreviewed firmware backups.

## Maintainer on-device test report — 2026-10-08

- **Device:** StackChan K151 / M5Stack CoreS3
- **Build:** Maintainer reported PlatformIO `[SUCCESS]` for the temporary emotion-test firmware.
- **Flash:** Maintainer reported successful upload to the device.
- **Test:** Temporary emotion test mode cycled through all 22 expression modes, approximately 3 seconds each.
- **Result:** **PASS (maintainer-reported):** All 22 modes appeared in the on-device cycle.
- **Evidence:** Direct maintainer observation; no video or serial log attached.
- **Not yet verified:** Detailed correctness of each expression, long-duration stability, LED/audio interactions, camera preview handoff, and clean integration of the published reference files into a fresh firmware checkout.
- **Release note:** The temporary test mode must be disabled and the regular firmware rebuilt/reflashed for normal Gemini operation.

## Release criteria

A reusable release requires a clean integration build, on-device verification of all 22 modes, dependency and license review, and documented installation steps. Until then, this is a **reference integration**, not a stable plug-and-play release.
