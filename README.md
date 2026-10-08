# StackChan — Be Human 101

**An experimental, community-driven AI robot project by [Jakudon](https://github.com/jakudon).**

Be Human 101 explores expressive robotic behavior using a StackChan-style robot, real-time AI conversations, an animated face, camera input, and external environmental sensors. This repository is being prepared for a public open-source release.

> **Project status:** Active development. The features below describe the current working prototype and planned integrations; this is not yet a packaged, reproducible public firmware release.

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

The firmware and installation instructions will be added after configuration files, credentials, upstream licenses, and hardware-specific settings have been reviewed. Please do not treat this README as a ready-to-flash release.

## Support development

If you find the project interesting, you can support its development by starring the repository, sharing feedback, reporting issues, or contributing ideas. A verified donation link will be added after a donation channel has been configured. **No payment account or donation address is published yet.**

## Credits and licensing

Built around the StackChan / M5Stack ecosystem and other upstream open-source components. Upstream authors retain their respective rights. A project license and third-party notices will be published after reviewing the incorporated source and its license requirements.

---

## ภาษาไทย

**Be Human 101** คือโปรเจกต์ทดลองพัฒนาหุ่นยนต์ StackChan ให้มีบุคลิกและการแสดงออกที่เป็นธรรมชาติมากขึ้น โดยเน้นหน้าตาแสดงอารมณ์ 22 รูปแบบ การสนทนากับ Gemini Live การใช้กล้อง และการรับข้อมูลจากเซนเซอร์ภายนอกผ่าน Cloudflare Gateway

ขณะนี้ยังอยู่ระหว่างพัฒนาและทดสอบ การเปิดเผยซอร์สโค้ดและคู่มือติดตั้งจะดำเนินการหลังตรวจสอบข้อมูลลับ สิทธิ์ของโค้ดต้นฉบับ และความพร้อมของเฟิร์มแวร์

**ร่วมสนับสนุน:** กด Star โปรเจกต์ เสนอความคิดเห็น หรือช่วยทดสอบได้ ช่องทางรับบริจาคจะเพิ่มเมื่อพร้อมใช้งาน
