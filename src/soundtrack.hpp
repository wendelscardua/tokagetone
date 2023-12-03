#pragma once
#include "common.hpp"
enum class Song : u8 {
  Synthetic = 0,
  Lalala = 0,
};
enum class SFX : u8 {
  SynthSquare1,
  SynthSquare2,
  SynthTriangle,
  SynthNoise,
  SynthDMCA,
};
const u8 NUM_SONGS = 1;
const u8 NUM_SFXS = 5;
