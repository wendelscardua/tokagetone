#pragma once

#include "common.hpp"
#include "ggsound.hpp"
enum class SongOpCode : u8 {
  C0,
  CS0,
  D0,
  DS0,
  E0,
  F0,
  FS0,
  G0,
  GS0,
  A0,
  AS0,
  B0,
  C1,
  CS1,
  D1,
  DS1,
  E1,
  F1,
  FS1,
  G1,
  GS1,
  A1,
  AS1,
  B1,
  C2,
  CS2,
  D2,
  DS2,
  E2,
  F2,
  FS2,
  G2,
  GS2,
  A2,
  AS2,
  B2,
  C3,
  CS3,
  D3,
  DS3,
  E3,
  F3,
  FS3,
  G3,
  GS3,
  A3,
  AS3,
  B3,
  C4,
  CS4,
  D4,
  DS4,
  E4,
  F4,
  FS4,
  G4,
  GS4,
  A4,
  AS4,
  B4,
  C5,
  CS5,
  D5,
  DS5,
  E5,
  F5,
  FS5,
  G5,
  GS5,
  A5,
  AS5,
  B5,
  C6,
  CS6,
  D6,
  DS6,
  E6,
  F6,
  FS6,
  G6,
  GS6,
  A6,
  AS6,
  B6,
  C7,
  CS7,
  D7,
  DS7,
  E7,
  F7,
  FS7,
  G7,
  GS7,
  A7,
  AS7,
  B7,
  // stream control opcodes

  // set length opcodes for standard note lengths
  SL1,
  SL2,
  SL3,
  SL4,
  SL5,
  SL6,
  SL7,
  SL8,
  SL9,
  SLA,
  SLB,
  SLC,
  SLD,
  SLE,
  SLF,
  SL0,

  // set length lo byte
  SLL,
  // set length hi byte
  SLH,

  // set instrument
  STI,

  // goto
  GOT,

  // call
  CAL,

  // return
  RET,

  // terminate
  TRM,

  // None,
  None,
};

enum class Instrument : u8 {
  AEIOU,
  Blah,
  BassKick,
  Click00,
  Click15,
  Click50,
  HeavyMajArp,
  HiHats,
  KickHatsCombo,
  Perc1,
  Perc2,
  Perc3,
  Plink,
  Powerchord,
  SnareHatsCombo,
  StarPlink,
  TriangleSnare,
  Voice1,
  Voice2,
  Voice3Heavy,
  Voice3HeavyOct,
  Voice3Slide,
  Silence,
  None, // subtle difference, Silence is a valid output
};

struct Entry {
  SongOpCode note;
  Instrument instrument;

  bool operator==(Entry other) {
    return note == other.note && instrument == other.instrument;
  }
};

struct Row {
  Entry square1;
  Entry square2;
  Entry triangle;
  Entry noise;
  Entry dpcm;

  Entry &channel_entry(GGSound::Channel channel);
};

class Maestro {
public:
  static constexpr u8 MAX_ROWS = 64;
  static constexpr u8 MAX_CHANNELS = 5;
  static constexpr u8 MAX_INSTRUCTIONS = 200;
  static constexpr u8 MAX_SFX_INSTRUCTIONS = 5;
  Row rows[MAX_ROWS];
  Maestro();

  void update_streams();
  void dynamic_sfx(GGSound::Channel channel, SongOpCode note,
                   Instrument instrument);
};