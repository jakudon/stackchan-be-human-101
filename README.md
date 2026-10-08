# StackChan — Be Human 101

**Custom Face & Emotion System and AI integrations developed and customized by [Jakudon](https://github.com/jakudon).**

Be Human 101 explores expressive robotic behavior using a StackChan-style robot, real-time AI conversations, an animated face, camera input, and external environmental sensors. This repository focuses on the custom expression layer and integration examples rather than redistributing the entire upstream firmware.

> **Project status:** Active development. The features below describe the current working prototype and planned integrations; this is not yet a packaged, reproducible public firmware release.

## What is original and what is upstream

**Jakudon / Be Human 101 contributions:** custom face rendering and 22 emotion modes, expression behavior, and extensions connecting camera, AI tools, and external sensor data. These are modifications and additions developed on top of an existing firmware architecture, **not a claim that the entire firmware was written from scratch**.

**Upstream foundation:** [StackChan Gemini Firmware by taranton](https://github.com/taranton/stackchan-gemini-firmware), plus the StackChan / M5Stack ecosystem. Copyright and license notices of upstream contributors remain applicable.

**Publication scope:** planned modules and examples for face rendering, Gemini integration, and sensor connectivity. Full firmware, SCServo library, local credentials, and experimental backups are not intended for inclusion in this repository.

## Highlights

### Expressive face — 22 emotion states
A custom on-device face renderer with these modes:

`neutral`, `listening`, `speaking`, `thinking`, `looking`, `happy`, `excited`, `laughing`, `sad`, `crying`, `surprised`, `scared`, `angry`, `annoyed`, `bored`, `embarrassed`, `curious`, `dizzy`, `love`, `found`, `error`, `sleep`.

The face is drawn on the robot's display. AI-selected expressions and conversational state transitions are still being refined.

### Gemini Live and camera
The prototype supports real-time Gemini Live conversation and a `look_with_camera` tool. A captured JPEG can be shown on the robot's screen while the image is sent to the AI for interpretation. Repeated captures and screen-state behavior remain under testing.

### External sensor gateway
A separate ESP8266 sensor node can report readings through a Cloudflare Worker gateway. Current experimental integration includes environmental sensor readings and capacitive touch input. The gateway keeps network services and hardware tools separate from the robot's core face and audio firmware.

### Motion
High-level head movement and gestures are controlled through bounded tool commands. Physical motion and servo torque activation have been tested on the prototype.

## Architecture

```text
                 Gemini Live (conversation / tool calling)
                               |
                    StackChan CoreS3 firmware
                   /         |          \
           Face / LCD    Camera       Motion
                               |
                     HTTPS Tool Gateway
                      (Cloudflare Worker)
                               |
                        ESP8266 sensor node
                   (environment / touch)
```

## Be Human 101: longer-term vision

The project's research direction includes long- and short-term memory, personality development, sensor-driven behavior, and increasingly natural social interaction. These are **goals**, not claims of human consciousness or achieved AGI.

## Source code and installation

**Development setup:** [VS Code + PlatformIO installation and build guide](face/README.md#development-environment-vs-code--platformio). **Hardware validation:** [CoreS3 test checklist](docs/VALIDATION.md).

**Available now:** [full face controller source pair](face/reference/), [22-emotion face renderer source patch](face/patches/EmotionController-22-emotions.patch), [emotion vocabulary header](face/EmotionModes.h), [face integration notes](face/README.md), and [AI/sensor architecture notes](integrations/README.md). The face patch is a diff against an upstream-derived firmware version, **not** a standalone ready-to-flash firmware. Gemini gateway and ESP8266 sensor implementations are not yet published.

## Support development

If you find the project interesting, you can support its development by starring the repository, sharing feedback, reporting issues, or contributing ideas. A verified donation link will be added after a donation channel has been configured. **No payment account or donation address is published yet.**

## Credits and licensing

- **Jakudon (2026):** Be Human 101 custom face, emotion behavior, and integration modifications.
- **[taranton — StackChan Gemini Firmware](https://github.com/taranton/stackchan-gemini-firmware):** upstream firmware foundation, MIT License.
- **M5Stack / StackChan BSP:** upstream hardware support components, subject to their respective notices.

The original MIT copyright and license notices must be retained for reused upstream code. The upstream firmware and BSP use MIT licensing; the bundled SCServo library has a GPL-3.0 license, so its code is **excluded from the planned published modules** pending dependency review. Final licensing for any extracted code will be verified before publication. This independent project is not endorsed by upstream maintainers.
