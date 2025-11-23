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

#include "Tones.h"

Tones::Tones()
{
}

Tones::~Tones()
{
}

void Tones::add(Tone &tone)
{
  tones.insert(tone);
}

void Tones::add(float length, int value, int channel, uint8_t volume)
{
  Tone tone;

  tone.length  = length;
  tone.value   = value;
  tone.channel = channel;
  tone.volume  = volume;

  add(tone);
}

void Tones::print()
{
  printf("  -- Tones --\n");

  for (auto &tone : tones)
  {
    printf(" [ %f, %d, %d, %d ]\n",
      tone.length,
      tone.value,
      tone.channel,
      tone.volume);
  }

  printf("\n");
}

