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

#ifndef DPP_NOTE_H
#define DPP_NOTE_H

#include <stdint.h>

struct Note
{
  uint8_t value;
  uint32_t volume;
  int duration;
  int midi_channel;
};

#endif

