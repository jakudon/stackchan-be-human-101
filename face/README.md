# Face & Emotion Module

This directory contains the **actual custom face-rendering changes** from the Be Human 101 StackChan prototype, plus a small standalone emotion-name helper.

## Files

- `EmotionModes.h`: dependency-free list of 22 emotion names and common Gemini aliases.
- `patches/EmotionController-22-emotions.patch`: source-level diff of the customized on-device face renderer and display-hold interface, based on the original StackChan Gemini firmware.

The patch includes custom cyan eyes, expressive eye shapes, solid animated speaking mouth, tears, blush, heart eyes, and emotion-specific rendering. It also adds display hold/release methods used while previewing camera captures.

## Important limitations

- The patch is **not** a standalone firmware, and is not automatically compatible with arbitrary firmware versions.
- It targets the `src/EmotionController.cpp` and `src/EmotionController.h` structure of the user's upstream-derived firmware. The full original source and build environment are **not included**.
- The full customized controller source and matching header are also available in [`reference/`](reference/). They require upstream libraries and hardware initialization.
- The patch contains only face-related source changes. Gemini tool schemas, camera preview wiring, and sensor gateway implementation are not part of this patch.
- Some animation and camera hold/release behavior remains under testing. No stable release is claimed.

## Development environment: VS Code + PlatformIO

The maintainer builds the StackChan CoreS3 firmware using **Visual Studio Code with PlatformIO**. This is **not** an Arduino IDE sketch and this repository does not contain a standalone `platformio.ini`.

1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Install the **PlatformIO IDE** extension in VS Code.
3. Obtain a compatible upstream firmware checkout with its original `platformio.ini`, dependencies, and StackChan board support.
4. Open the **firmware project folder** (the folder containing `platformio.ini`) in VS Code.
5. Review the existing `src/EmotionController.cpp` and `src/EmotionController.h` before integrating the reference files from this repository. Keep backups and do not overwrite unrelated firmware changes.
6. In the VS Code integrated terminal, run:

```powershell
py -m platformio run -e m5stack-cores3
```

Alternatively, use PlatformIO's **Build** command after selecting the `m5stack-cores3` environment. The `py -m platformio` command requires a working Python/PlatformIO installation; the VS Code extension may manage its own PlatformIO environment.

**Hardware target:** M5Stack CoreS3 / StackChan K151. A successful build does not prove correct behavior on hardware. Flashing and testing should follow the firmware project's own instructions. Do not flash this repository as-is.

## Quick integration example

Copy both files from `face/reference/` into the `src/` directory of a **compatible upstream-derived StackChan firmware project**. These files require `Arduino.h`, `M5StackChan.h`, and `M5Unified.h` and are not a standalone PlatformIO project.

```cpp
#include "EmotionController.h"

EmotionController face;

void setup() {
  // Initialize M5StackChan, display, power, and RGB hardware using
  // the upstream firmware's normal startup sequence BEFORE begin().
  face.begin();
  face.setEmotion("happy");
}

void loop() {
  face.loop();
}

// During camera preview: face.holdDisplay();
// After preview ends: face.releaseDisplay();
```

**Important:** This snippet is an API illustration, not a complete flashable sketch. In the real firmware, keep the upstream hardware initialization and scheduler; do not add a second `setup()`/`loop()`.

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

## Build verification

- **2026-10-08:** The project maintainer reported `[SUCCESS]` from `py -m platformio run -e m5stack-cores3` in their local upstream-derived firmware project.
- This confirms a successful **local firmware build**, not an independently reproduced build of this repository or a clean-install integration test.
- The uploaded `EmotionController.cpp` and `EmotionController.h` are published under `reference/`. Their exact integration into a fresh upstream checkout remains unverified.
- Hardware behavior, all 22 expression visuals, and repeated camera-preview transitions have not been validated by this build result alone.

