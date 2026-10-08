// Be Human 101 - Face emotion vocabulary
// Copyright (c) 2026 Jakudon - Be Human 101 contributions
// MIT License. This is a standalone integration helper, not a complete renderer.
// Inspired by the expression extensions developed for StackChan Gemini Firmware.
// Upstream: https://github.com/taranton/stackchan-gemini-firmware

#pragma once
#include <stddef.h>
#include <string.h>

namespace behuman101 {

enum class Emotion {
  Neutral, Listening, Speaking, Thinking, Looking,
  Happy, Excited, Laughing, Sad, Crying, Surprised,
  Scared, Angry, Annoyed, Bored, Embarrassed,
  Curious, Dizzy, Love, Found, Error, Sleep
};

struct EmotionName { Emotion mode; const char* name; };

static constexpr EmotionName kEmotions[] = {
  {Emotion::Neutral, "neutral"}, {Emotion::Listening, "listening"},
  {Emotion::Speaking, "speaking"}, {Emotion::Thinking, "thinking"},
  {Emotion::Looking, "looking"}, {Emotion::Happy, "happy"},
  {Emotion::Excited, "excited"}, {Emotion::Laughing, "laughing"},
  {Emotion::Sad, "sad"}, {Emotion::Crying, "crying"},
  {Emotion::Surprised, "surprised"}, {Emotion::Scared, "scared"},
  {Emotion::Angry, "angry"}, {Emotion::Annoyed, "annoyed"},
  {Emotion::Bored, "bored"}, {Emotion::Embarrassed, "embarrassed"},
  {Emotion::Curious, "curious"}, {Emotion::Dizzy, "dizzy"},
  {Emotion::Love, "love"}, {Emotion::Found, "found"},
  {Emotion::Error, "error"}, {Emotion::Sleep, "sleep"}
};

static constexpr size_t kEmotionCount = sizeof(kEmotions) / sizeof(kEmotions[0]);

inline const char* emotionName(Emotion emotion) {
  for (const auto& item : kEmotions) {
    if (item.mode == emotion) return item.name;
  }
  return "neutral";
}

inline Emotion emotionFromName(const char* name) {
  if (!name) return Emotion::Neutral;
  for (const auto& item : kEmotions) {
    if (strcmp(item.name, name) == 0) return item.mode;
  }
  if (strcmp(name, "joy") == 0 || strcmp(name, "smile") == 0) return Emotion::Happy;
  if (strcmp(name, "laugh") == 0) return Emotion::Laughing;
  if (strcmp(name, "cry") == 0) return Emotion::Crying;
  if (strcmp(name, "fear") == 0 || strcmp(name, "afraid") == 0) return Emotion::Scared;
  if (strcmp(name, "mad") == 0) return Emotion::Angry;
  if (strcmp(name, "shy") == 0) return Emotion::Embarrassed;
  if (strcmp(name, "success") == 0) return Emotion::Found;
  return Emotion::Neutral;
}
} // namespace behuman101
