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

#include "Beats.h"

Beats::Beats()
{
}

Beats::~Beats()
{
}

void Beats::add(Beat &beat)
{
  beats.insert(beat);
}

void Beats::add(float value, int instrument, int channel, uint8_t volume)
{
  Beat beat;

  beat.value      = value;
  beat.instrument = instrument;
  beat.channel    = channel;
  beat.volume     = volume;

  add(beat);
}

void Beats::print()
{
  printf("  -- Beats --\n");

  for (auto &beat : beats)
  {
    printf(" [ %f, %d, %d, %d ]\n",
      beat.value,
      beat.instrument,
      beat.channel,
      beat.volume);
  }

  printf("\n");
}

