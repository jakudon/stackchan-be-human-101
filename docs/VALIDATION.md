# CoreS3 Face Module Validation

This checklist helps maintainers test the **Be Human 101 face renderer** in an upstream-derived StackChan K151 firmware project.

## Environment

- Hardware: M5Stack StackChan K151 / CoreS3
- Development: Visual Studio Code + PlatformIO IDE
- PlatformIO environment: `m5stack-cores3`
- Firmware: a compatible upstream-derived project with its original `platformio.ini` and dependencies
- Source under test: `face/reference/EmotionController.cpp` and `face/reference/EmotionController.h`

This repository **does not contain a complete firmware or a standalone build project**.

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

## Release criteria

A reusable release requires a clean integration build, on-device verification of all 22 modes, dependency and license review, and documented installation steps. Until then, this is a **reference integration**, not a stable plug-and-play release.
