// Be Human 101 - Customized StackChan face renderer (integration source)
// Modifications (c) 2026 Jakudon. Based on taranton/stackchan-gemini-firmware (MIT).
// Retain upstream copyright/license notices when redistributing derived code.
// Requires the matching EmotionController.h and upstream StackChan BSP; not standalone.

#include "EmotionController.h"

#include <M5StackChan.h>
#include <M5Unified.h>

namespace {
constexpr uint8_t kLedCount = 12;
constexpr uint32_t kFrameIntervalMs = 35;  // ~28 FPS for LEDs; smooth, but light on CPU/audio.
constexpr uint32_t kFaceIntervalMs = 90;   // ~11 FPS for idle LCD; avoid stealing time from audio.
constexpr uint32_t kSpeakingFaceIntervalMs = 40;  // Stable LCD cadence while speaking; keep audio safe.
constexpr uint32_t kSleepDisplayOffMs = 30000;  // Anti-retention: show sleep face briefly, then blank LCD.

String lowerTrimmed(String s) {
  s.trim();
  s.toLowerCase();
  return s;
}

int triWave(uint16_t phase, int amplitude) {
  uint8_t p = phase & 0xFF;
  int v = p < 128 ? p : 255 - p;
  return ((v * 2 * amplitude) / 255) - amplitude;
}
}  // namespace

void EmotionController::begin() {
  setEmotion("neutral");
}

void EmotionController::holdDisplay() {
  displayHold_ = true;
  Serial.println("Emotion: display hold");
}

void EmotionController::releaseDisplay() {
  if (!displayHold_) return;
  displayHold_ = false;
  lastFaceMs_ = 0;
  Serial.println("Emotion: display released");
}

bool EmotionController::setEmotion(const String& emotion) {
  String normalized;
  Mode next = parseMode(emotion, normalized);
  Mode previous = mode_;
  mode_ = next;
  current_ = normalized;
  frame_ = 0;
  lastFrameMs_ = 0;
  lastFaceMs_ = 0;
  sleepStartedMs_ = (mode_ == Mode::Sleep) ? millis() : 0;
  if (previous == Mode::Sleep && mode_ != Mode::Sleep) {
    restoreSleepDisplay();
  }
  drawLabel();
  loop();  // Render first LED frame immediately.
  Serial.printf("Emotion: %s\n", current_.c_str());
  return true;
}

void EmotionController::loop() {
  uint32_t now = millis();
  if (mode_ == Mode::Sleep && sleepStartedMs_ == 0) sleepStartedMs_ = now;
  if (mode_ != Mode::Sleep && sleepDisplayOff_) restoreSleepDisplay();

  bool externalPower = externalPowerPresent();
  if (mode_ == Mode::Sleep && !sleepDisplayOff_ && sleepVisualOffDue()) {
    enterSleepDisplayOff(externalPower);
  }

  uint32_t faceInterval = (mode_ == Mode::Speaking) ? kSpeakingFaceIntervalMs : kFaceIntervalMs;
  if (!sleepDisplayOff_ && (lastFaceMs_ == 0 || now - lastFaceMs_ >= faceInterval)) {
    lastFaceMs_ = now;
    if (!displayHold_) renderFace();
  }
  if (lastFrameMs_ != 0 && now - lastFrameMs_ < kFrameIntervalMs) return;
  lastFrameMs_ = now;
  ++frame_;

  switch (mode_) {
    case Mode::Neutral:   renderNeutral(); break;
    case Mode::Listening: renderListening(); break;
    case Mode::Speaking:  renderSpeaking(); break;
    case Mode::Thinking:  renderThinking(); break;
    case Mode::Looking:   renderLooking(); break;
    case Mode::Happy:     renderHappy(); break;
    case Mode::Excited:   renderHappy(); break;
    case Mode::Laughing:  renderHappy(); break;
    case Mode::Sad:       renderNeutral(); break;
    case Mode::Crying:    renderNeutral(); break;
    case Mode::Surprised: renderListening(); break;
    case Mode::Scared:    renderError(); break;
    case Mode::Angry:     renderAngry(); break;
    case Mode::Annoyed:   renderAngry(); break;
    case Mode::Bored:     renderNeutral(); break;
    case Mode::Embarrassed: renderHappy(); break;
    case Mode::Curious:   renderLooking(); break;
    case Mode::Dizzy:     renderError(); break;
    case Mode::Love:      renderHappy(); break;
    case Mode::Found:     renderFound(); break;
    case Mode::Error:     renderError(); break;
    case Mode::Sleep:
      if (sleepDisplayOff_ && !externalPower) setAll(0, 0, 0);
      else renderSleep();
      break;
  }
  refresh();
}

EmotionController::Mode EmotionController::parseMode(const String& emotion, String& normalized) {
  String e = lowerTrimmed(emotion);
  if (e == "listen" || e == "listening") { normalized = "listening"; return Mode::Listening; }
  if (e == "speak" || e == "speaking" || e == "talking") { normalized = "speaking"; return Mode::Speaking; }
  if (e == "think" || e == "thinking") { normalized = "thinking"; return Mode::Thinking; }
  if (e == "look" || e == "looking" || e == "seeing" || e == "camera") { normalized = "looking"; return Mode::Looking; }
  if (e == "happy" || e == "joy" || e == "smile") { normalized = "happy"; return Mode::Happy; }
  if (e == "excited" || e == "exciting") { normalized = "excited"; return Mode::Excited; }
  if (e == "laugh" || e == "laughing") { normalized = "laughing"; return Mode::Laughing; }
  if (e == "sad" || e == "unhappy") { normalized = "sad"; return Mode::Sad; }
  if (e == "cry" || e == "crying") { normalized = "crying"; return Mode::Crying; }
  if (e == "surprised" || e == "surprise" || e == "shocked") { normalized = "surprised"; return Mode::Surprised; }
  if (e == "scared" || e == "afraid" || e == "fear") { normalized = "scared"; return Mode::Scared; }
  if (e == "angry" || e == "mad") { normalized = "angry"; return Mode::Angry; }
  if (e == "annoyed" || e == "annoying") { normalized = "annoyed"; return Mode::Annoyed; }
  if (e == "bored" || e == "boring") { normalized = "bored"; return Mode::Bored; }
  if (e == "embarrassed" || e == "shy") { normalized = "embarrassed"; return Mode::Embarrassed; }
  if (e == "curious" || e == "curiosity") { normalized = "curious"; return Mode::Curious; }
  if (e == "dizzy") { normalized = "dizzy"; return Mode::Dizzy; }
  if (e == "love" || e == "loving") { normalized = "love"; return Mode::Love; }
  if (e == "found" || e == "success") { normalized = "found"; return Mode::Found; }
  if (e == "error" || e == "confused") { normalized = "error"; return Mode::Error; }
  if (e == "sleep" || e == "sleeping") { normalized = "sleep"; return Mode::Sleep; }
  normalized = "neutral";
  return Mode::Neutral;
}

void EmotionController::hsvToRgb(uint8_t h, uint8_t s, uint8_t v, uint8_t& r, uint8_t& g, uint8_t& b) {
  uint8_t region = h / 43;
  uint8_t remainder = (h - (region * 43)) * 6;
  uint8_t p = (v * (255 - s)) >> 8;
  uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
    default:
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
}

uint8_t EmotionController::wave8(uint16_t x, uint8_t minValue, uint8_t maxValue) {
  uint8_t phase = x & 0xFF;
  uint8_t tri = phase < 128 ? phase * 2 : (255 - phase) * 2;
  return minValue + ((uint16_t)(maxValue - minValue) * tri) / 255;
}

uint8_t EmotionController::scale8(uint8_t value, uint8_t scale) {
  return ((uint16_t)value * scale) / 255;
}

bool EmotionController::externalPowerPresent() {
  int16_t vbus = M5.Power.getVBUSVoltage();
  if (vbus > 4200) return true;
  return M5.Power.isCharging() == m5::Power_Class::is_charging;
}

bool EmotionController::sleepVisualOffDue() const {
  return mode_ == Mode::Sleep && sleepStartedMs_ != 0 && millis() - sleepStartedMs_ >= kSleepDisplayOffMs;
}

void EmotionController::enterSleepDisplayOff(bool externalPower) {
  if (sleepDisplayOff_) return;
  auto& display = M5StackChan.Display();
  savedDisplayBrightness_ = display.getBrightness();
  display.fillScreen(TFT_BLACK);
  display.sleep();
  sleepDisplayOff_ = true;
  // On USB/external power keep a tiny heartbeat in the external LEDs. On battery,
  // go visually dark after the same timeout to avoid wasting power.
  if (!externalPower) setAll(0, 0, 0);
  refresh();
  Serial.printf("Emotion: sleep display off, external_power=%s\n", externalPower ? "yes" : "no");
}

void EmotionController::restoreSleepDisplay() {
  if (!sleepDisplayOff_) return;
  auto& display = M5StackChan.Display();
  display.wakeup();
  display.setBrightness(savedDisplayBrightness_ == 0 ? 127 : savedDisplayBrightness_);
  sleepDisplayOff_ = false;
  lastFaceMs_ = 0;
  Serial.println("Emotion: sleep display restored");
}

void EmotionController::setLed(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index >= kLedCount) return;
  M5StackChan.setRgbColor(index, r, g, b);
}

void EmotionController::setAll(uint8_t r, uint8_t g, uint8_t b) {
  for (uint8_t i = 0; i < kLedCount; ++i) setLed(i, r, g, b);
}

void EmotionController::refresh() {
  M5StackChan.refreshRgb();
}

void EmotionController::drawLabel() {
  if (!displayHold_) renderFace();
}

void EmotionController::renderFace() {
  auto& display = M5StackChan.Display();
  static M5Canvas canvas(&display);
  static bool canvasReady = false;

  const int w = display.width();
  const int h = display.height();

  if (!canvasReady) {
    canvas.setColorDepth(8);
    canvasReady = canvas.createSprite(w, h) != nullptr;
    if (!canvasReady) {
      Serial.println("Emotion: canvas allocation failed");
      return;
    }
  }

  auto& d = canvas;
  const int cx = w / 2;
  const int cy = h / 2;

  d.fillSprite(TFT_BLACK);

  // Custom face rendering and expression animation.
  const uint16_t cyan = 0x07FF;       // Bright Vivid Cyan
  const uint16_t cyanSoft = 0x567F;   // Soft Cyan
  const uint16_t blushPink = 0xFD14;  // Pink

  // Custom face rendering and expression animation.
  const int eyeSpacing = 58;
  const int lx = cx - eyeSpacing;
  const int rx = cx + eyeSpacing;
  const int eyeY = cy - 6;            // Custom face rendering and expression animation.
  const int mouthY = cy + 46;         // Custom face rendering and expression animation.

  // -------------------------------------------------------------
  // Eilik Accurate Primitives
  // -------------------------------------------------------------

  // Custom face rendering and expression animation.
  auto drawEilikEye = [&](int x, int y, int ew, int eh) {
    d.fillRoundRect(x - ew / 2, y - eh / 2, ew, eh, ew / 2, cyan);
  };

  // Custom face rendering and expression animation.
  auto drawHappyArc = [&](int x, int y, int ew, int eh) {
    d.fillRoundRect(x - ew / 2, y - eh / 2, ew, eh, eh / 2, cyan);
    d.fillRoundRect(x - ew / 2, y, ew, eh, eh / 2, TFT_BLACK);
  };

  // Custom face rendering and expression animation.
  auto drawSqueezeEye = [&](int x, int y, bool isLeft) {
    const int len = 18;
    const int thick = 5;
    if (isLeft) {
      for (int t = -thick/2; t <= thick/2; t++) {
        d.drawLine(x - len, y - 10 + t, x + 6, y + t, cyan);
        d.drawLine(x - len, y + 10 + t, x + 6, y + t, cyan);
      }
      d.fillCircle(x + 6, y, thick/2 + 1, cyan);
      d.fillCircle(x - len, y - 10, thick/2 + 1, cyan);
      d.fillCircle(x - len, y + 10, thick/2 + 1, cyan);
    } else {
      for (int t = -thick/2; t <= thick/2; t++) {
        d.drawLine(x + len, y - 10 + t, x - 6, y + t, cyan);
        d.drawLine(x + len, y + 10 + t, x - 6, y + t, cyan);
      }
      d.fillCircle(x - 6, y, thick/2 + 1, cyan);
      d.fillCircle(x + len, y - 10, thick/2 + 1, cyan);
      d.fillCircle(x + len, y + 10, thick/2 + 1, cyan);
    }
  };

  // Custom face rendering and expression animation.
  auto drawAngryDroplet = [&](int x, int y, bool isLeft) {
    drawEilikEye(x, y, 48, 56);
    if (isLeft) {
      d.fillTriangle(x - 26, y - 30, x + 26, y - 30, x + 26, y + 2, TFT_BLACK);
    } else {
      d.fillTriangle(x + 26, y - 30, x - 26, y - 30, x - 26, y + 2, TFT_BLACK);
    }
  };

  // Custom face rendering and expression animation.
  // Custom face rendering and expression animation.
  auto drawTalkingBlobMouth = [&](int mw, int mh) {
    d.fillRoundRect(cx - mw / 2, mouthY - mh / 2, mw, mh, mh / 2, cyan);
    // Custom face rendering and expression animation.
    d.fillCircle(cx - mw / 4, mouthY, mh / 2, cyan);
    d.fillCircle(cx + mw / 4, mouthY, mh / 2, cyan);
  };

  // Custom face rendering and expression animation.
  auto drawHeart = [&](int x, int y, int sz) {
    int r = sz / 4;
    d.fillCircle(x - r, y - r, r, cyan);
    d.fillCircle(x + r, y - r, r, cyan);
    d.fillTriangle(x - sz / 2, y - r + 1, x + sz / 2, y - r + 1, x, y + sz / 2, cyan);
  };

  // -------------------------------------------------------------
  // Custom face rendering and expression animation.
  // -------------------------------------------------------------

  switch (mode_) {

    // Custom face rendering and expression animation.
    case Mode::Neutral: {
      bool blink = ((frame_ / 45) % 16) == 0;
      if (!blink) {
        drawEilikEye(lx, eyeY, 48, 60);
        drawEilikEye(rx, eyeY, 48, 60);
      } else {
        // Custom face rendering and expression animation.
        d.fillRoundRect(lx - 22, eyeY, 44, 6, 3, cyan);
        d.fillRoundRect(rx - 22, eyeY, 44, 6, 3, cyan);
      }
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Speaking: {
      drawEilikEye(lx, eyeY, 48, 60);
      drawEilikEye(rx, eyeY, 48, 60);

      // Custom face rendering and expression animation.
      const int talkW[] = {24, 38, 48, 34};
      const int talkH[] = {12, 22, 28, 16};
      int step = (frame_ / 4) % 4;
      drawTalkingBlobMouth(talkW[step], talkH[step]);
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Happy:
      drawHappyArc(lx, eyeY - 2, 48, 36);
      drawHappyArc(rx, eyeY - 2, 48, 36);
      drawTalkingBlobMouth(42, 18);
      break;

    // Custom face rendering and expression animation.
    case Mode::Excited: {
      int bounce = (frame_ % 6 < 3) ? -3 : 0;
      drawSqueezeEye(lx, eyeY + bounce, true);
      drawSqueezeEye(rx, eyeY + bounce, false);
      drawTalkingBlobMouth(46, 24);
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Laughing: {
      int shake = (frame_ % 4 < 2) ? -2 : 2;
      drawSqueezeEye(lx, eyeY + shake, true);
      drawSqueezeEye(rx, eyeY - shake, false);
      drawTalkingBlobMouth(54, 30);
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Sad:
      drawEilikEye(lx, eyeY + 6, 46, 54);
      drawEilikEye(rx, eyeY + 6, 46, 54);
      d.fillTriangle(lx - 25, eyeY - 26, lx + 25, eyeY - 26, lx - 25, eyeY + 4, TFT_BLACK);
      d.fillTriangle(rx + 25, eyeY - 26, rx - 25, eyeY - 26, rx + 25, eyeY + 4, TFT_BLACK);
      break;

    // Custom face rendering and expression animation.
    case Mode::Crying: {
      drawHappyArc(lx, eyeY + 4, 46, 32);
      drawHappyArc(rx, eyeY + 4, 46, 32);
      int drop = (frame_ * 3) % 20;
      d.fillRoundRect(lx - 6, eyeY + 16, 12, 24 + drop, 6, cyan);
      d.fillRoundRect(rx - 6, eyeY + 16, 12, 24 + drop, 6, cyan);
      drawTalkingBlobMouth(44, 20);
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Surprised:
      drawEilikEye(lx, eyeY - 6, 52, 68);
      drawEilikEye(rx, eyeY - 6, 52, 68);
      d.fillCircle(cx, mouthY, 14, cyan);
      break;

    // Custom face rendering and expression animation.
    case Mode::Scared: {
      int tremor = (frame_ % 4 < 2) ? -2 : 2;
      drawEilikEye(lx + tremor, eyeY, 34, 44);
      drawEilikEye(rx + tremor, eyeY, 34, 44);
      d.fillCircle(cx, mouthY, 8, cyan);
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Angry:
      drawAngryDroplet(lx, eyeY, true);
      drawAngryDroplet(rx, eyeY, false);
      break;

    // Custom face rendering and expression animation.
    case Mode::Annoyed:
      drawEilikEye(lx, eyeY, 48, 58);
      drawEilikEye(rx, eyeY, 48, 58);
      d.fillRect(lx - 26, eyeY - 32, 52, 25, TFT_BLACK);
      d.fillRect(rx - 26, eyeY - 32, 52, 25, TFT_BLACK);
      break;

    // Custom face rendering and expression animation.
    case Mode::Bored:
      d.fillRoundRect(lx - 22, eyeY, 44, 14, 7, cyan);
      d.fillRoundRect(rx - 22, eyeY, 44, 14, 7, cyan);
      break;

    // Custom face rendering and expression animation.
    case Mode::Embarrassed:
      drawEilikEye(lx, eyeY + 4, 40, 50);
      drawEilikEye(rx, eyeY + 4, 40, 50);
      d.fillRoundRect(lx - 24, eyeY + 28, 18, 8, 4, blushPink);
      d.fillRoundRect(rx + 6, eyeY + 28, 18, 8, 4, blushPink);
      break;

    // Custom face rendering and expression animation.
    case Mode::Curious:
      drawEilikEye(lx, eyeY - 8, 50, 66);
      drawEilikEye(rx, eyeY + 2, 38, 48);
      break;

    // Custom face rendering and expression animation.
    case Mode::Dizzy: {
      for (int r = 24; r >= 8; r -= 7) {
        d.drawCircle(lx, eyeY, r, cyan);
        d.drawCircle(rx, eyeY, r, cyan);
      }
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Love: {
      int pulse = (frame_ % 14 < 7) ? 4 : 0;
      drawHeart(lx, eyeY, 48 + pulse);
      drawHeart(rx, eyeY, 48 + pulse);
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Sleep:
      d.fillRoundRect(lx - 22, eyeY + 8, 44, 8, 4, cyanSoft);
      d.fillRoundRect(rx - 22, eyeY + 8, 44, 8, 4, cyanSoft);
      d.setTextSize(2);
      d.setTextColor(cyanSoft);
      d.setCursor(cx + 64, cy - 42 - ((frame_ / 10) % 8));
      d.print("zZ");
      break;

    // Custom face rendering and expression animation.
    case Mode::Thinking: {
      int driftX = triWave(frame_ * 3, 10);
      drawEilikEye(lx + driftX, eyeY - 14, 42, 52);
      drawEilikEye(rx + driftX, eyeY - 14, 42, 52);
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Listening:
      drawEilikEye(lx, eyeY - 2, 48, 60);
      drawEilikEye(rx, eyeY - 2, 48, 60);
      break;

    // Custom face rendering and expression animation.
    case Mode::Looking: {
      int shiftX = (frame_ % 80 < 40) ? -16 : 16;
      drawEilikEye(lx + shiftX, eyeY, 46, 58);
      drawEilikEye(rx + shiftX, eyeY, 46, 58);
      break;
    }

    // Custom face rendering and expression animation.
    case Mode::Found:
      drawEilikEye(lx, eyeY - 4, 50, 66);
      drawEilikEye(rx, eyeY - 4, 50, 66);
      drawTalkingBlobMouth(46, 22);
      break;

    // Custom face rendering and expression animation.
    case Mode::Error: {
      auto drawBoldX = [&](int x, int y) {
        for (int i = -14; i <= 14; i++) {
          d.fillCircle(x + i, y + i, 4, cyan);
          d.fillCircle(x + i, y - i, 4, cyan);
        }
      };
      drawBoldX(lx, eyeY);
      drawBoldX(rx, eyeY);
      break;
    }
  }

  canvas.pushSprite(&display, 0, 0);
}
void EmotionController::renderNeutral() {
  uint8_t v = wave8(frame_ * 2, 4, 18);
  setAll(0, 0, v);
}

void EmotionController::renderListening() {
  uint8_t v = wave8(frame_ * 3, 10, 55);
  setAll(0, scale8(80, v), v);
}

void EmotionController::renderSpeaking() {
  uint8_t v = wave8(frame_ * 10, 25, 95);
  for (uint8_t i = 0; i < kLedCount; ++i) {
    uint8_t local = (i % 2 == 0) ? v : scale8(v, 120);
    setLed(i, scale8(60, local), local, scale8(120, local));
  }
}

void EmotionController::renderThinking() {
  uint8_t head = (frame_ / 2) % kLedCount;
  for (uint8_t i = 0; i < kLedCount; ++i) {
    uint8_t dist = (i + kLedCount - head) % kLedCount;
    uint8_t v = 2;
    if (dist == 0) v = 95;
    else if (dist == 1 || dist == kLedCount - 1) v = 42;
    else if (dist == 2 || dist == kLedCount - 2) v = 14;
    setLed(i, 0, scale8(120, v), v);
  }
}

void EmotionController::renderLooking() {
  // Camera assist light: bright neutral-white LEDs to signal capture/analyze and
  // add a little illumination. No servo/head motion is triggered from this state.
  uint8_t pulse = wave8(frame_ * 4, 120, 210);
  uint8_t sweep = (frame_ / 3) % kLedCount;
  for (uint8_t i = 0; i < kLedCount; ++i) {
    uint8_t v = pulse;
    if (i == sweep || i == (sweep + kLedCount / 2) % kLedCount) v = 245;
    setLed(i, v, v, v);
  }
}

void EmotionController::renderHappy() {
  uint8_t baseHue = frame_ * 3;
  for (uint8_t i = 0; i < kLedCount; ++i) {
    uint8_t r, g, b;
    hsvToRgb(baseHue + i * 21, 230, 85, r, g, b);
    setLed(i, r, g, b);
  }
}

void EmotionController::renderAngry() {
  uint8_t v = wave8(frame_ * 9, 25, 110);
  for (uint8_t i = 0; i < kLedCount; ++i) {
    uint8_t local = (i == ((frame_ / 3) % kLedCount)) ? 130 : v;
    setLed(i, local, 0, 0);
  }
}

void EmotionController::renderFound() {
  uint8_t v = wave8(frame_ * 8, 25, 120);
  for (uint8_t i = 0; i < kLedCount; ++i) {
    if ((i + frame_ / 4) % 3 == 0) setLed(i, v, scale8(220, v), 0);
    else setLed(i, scale8(30, v), scale8(70, v), 0);
  }
}

void EmotionController::renderError() {
  bool on = ((frame_ / 8) % 2) == 0;
  setAll(on ? 110 : 0, on ? 12 : 0, 0);
}

void EmotionController::renderSleep() {
  uint8_t v = wave8(frame_, 0, 14);
  setAll(0, 0, v);
}