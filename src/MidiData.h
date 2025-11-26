/**
 *  Drums++
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2001-2025 by Michael Kohn
 *
 */

#ifndef DPP_MIDI_DATA_H
#define DPP_MIDI_DATA_H

#include "stdint.h"

struct MidiData
{
  MidiData() :
    value    { 0 },
    volume   { 0 },
    channel  { 0 },
    duration { 0 },
    location { 0 }
  {
  }

  uint8_t value;
  uint8_t volume;
  uint8_t channel;
  uint32_t duration;
  uint32_t location;
};

#endif

