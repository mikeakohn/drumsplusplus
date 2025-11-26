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

#include <stdio.h>
#include <stdlib.h>

#include "Phrase.h"

Phrase::Phrase()
{
}

Phrase::~Phrase()
{
}

void Phrase::add(MidiData &data)
{
  midi_data.push_back(data);
}

void Phrase::add(
  uint8_t value,
  uint8_t volume,
  uint8_t channel,
  uint32_t duration,
  uint32_t location)
{
  MidiData midi_data;

  midi_data.value    = value;
  midi_data.volume   = volume;
  midi_data.channel  = channel;
  midi_data.duration = duration;
  midi_data.location = location;

  add(midi_data);
}

void Phrase::print()
{
  printf("  -- Phrase %s --\n", name.c_str());

  for (auto &it : midi_data)
  {
    printf("  value: %d, volume: %d, channel: %d, duration: %d, location: %d\n",
      it.value,
      it.volume,
      it.channel,
      it.duration,
      it.location);
  }
}

