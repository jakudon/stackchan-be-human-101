# AI and Sensor Integration Notes

The Be Human 101 prototype connects the CoreS3 firmware to a separate HTTPS tool gateway and an ESP8266-based sensor node. This directory currently documents the architecture; it does **not** publish the original private gateway deployment or claim to provide a ready-to-run server.

## Tool boundaries

- **Face expression:** the AI may select a named emotion; the firmware validates it against supported states.
- **Camera:** the firmware captures a JPEG, displays a preview, and forwards the image to the AI. Repeated-capture display behavior is still under testing.
- **Sensors:** an external ESP8266 reports environmental readings and touch events to a gateway; the robot accesses normalized sensor readings through tool calls.
- **Motion:** servo limits and hardware safety remain enforced by the robot firmware. Raw unrestricted motor control is not exposed.

## Example tool contract

```json
{
  "name": "set_emotion",
  "parameters": {
    "type": "object",
    "properties": {
      "emotion": {
        "type": "string",
        "enum": ["neutral", "listening", "speaking", "thinking", "looking", "happy", "excited", "laughing", "sad", "crying", "surprised", "scared", "angry", "annoyed", "bored", "embarrassed", "curious", "dizzy", "love", "found", "error", "sleep"]
      }
    },
    "required": ["emotion"]
  }
}
```

This is an **illustrative contract**, not a claim that the current firmware exposes a tool with this exact name.

## Security

Never commit Wi-Fi passwords, API keys, Cloudflare credentials, device identifiers, or SD-card configuration. Use environment-specific settings outside the repository. Review dependencies and license obligations before distributing complete firmware images.

## Credits

Based on the StackChan Gemini Firmware architecture by taranton; integration extensions and project direction by Jakudon.
