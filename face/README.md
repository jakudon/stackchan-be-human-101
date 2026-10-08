# Face & Emotion Module

This directory contains the **actual custom face-rendering changes** from the Be Human 101 StackChan prototype, plus a small standalone emotion-name helper.

## Files

- `EmotionModes.h`: dependency-free list of 22 emotion names and common Gemini aliases.
- `patches/EmotionController-22-emotions.patch`: source-level diff of the customized on-device face renderer and display-hold interface, based on the original StackChan Gemini firmware.

The patch includes custom cyan eyes, expressive eye shapes, solid animated speaking mouth, tears, blush, heart eyes, and emotion-specific rendering. It also adds display hold/release methods used while previewing camera captures.

## Important limitations

- The patch is **not** a standalone firmware, and is not automatically compatible with arbitrary firmware versions.
- It targets the `src/EmotionController.cpp` and `src/EmotionController.h` structure of the user's upstream-derived firmware. The full original source and build environment are **not included**.
- The patch contains only face-related source changes. Gemini tool schemas, camera preview wiring, and sensor gateway implementation are not part of this patch.
- Some animation and camera hold/release behavior remains under testing. No stable release is claimed.

## Integration outline

1. Obtain the upstream firmware and follow its own build and license instructions.
2. Review the patch and confirm that the target `EmotionController` version matches before attempting to apply it.
3. Port the new expression cases and display-hold methods as needed; do not blindly apply to newer versions.
4. Wire your own AI tool response to `setEmotion()` and camera preview state to `holdDisplay()` / `releaseDisplay()`.
5. Build and validate on your own hardware.

## Attribution

Custom expression and integration modifications: **Jakudon — Be Human 101 (2026)**.

Upstream firmware: **[taranton/stackchan-gemini-firmware](https://github.com/taranton/stackchan-gemini-firmware)**, MIT License. Preserve the upstream copyright and license notices when distributing derived code. M5Stack BSP and other dependencies are governed by their respective licenses. The GPL-licensed SCServo library is **not included** in this repository.

This is a source patch for research and integration, not a complete firmware distribution.
