/**
 *  Drums++
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2001-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "Pattern.h"

Pattern::Pattern()
{
}

Pattern::~Pattern()
{
}

void Pattern::add(
  uint8_t value,
  uint8_t volume,
  uint8_t channel,
  uint32_t duration)
{
  Data current;

  current.value = value;
  current.volume = volume;
  current.channel = channel;
  current.duration = duration;

  data.push_back(current);
}

void Pattern::print()
{
  printf("  -- Pattern %s -- \n", name.c_str());

  for (auto it = data.begin(); it != data.end(); it++)
  {
    printf("   value: %d, volume: %d, channel: %d, duration: %d\n",
      it->value,
      it->volume,
      it->channel,
      it->duration);
  }
}

