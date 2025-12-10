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

#ifndef DPP_NOTE_OFF_H
#define DPP_NOTE_OFF_H

#include <stdint.h>

struct NoteOff
{
  uint32_t location;
  int value;
  int channel;
};

struct CompareNoteOff
{
  bool operator()(const NoteOff &lhs, const NoteOff &rhs) const
  {
    if (lhs.location == rhs.location)
    {
      if (lhs.value == rhs.value) { return lhs.channel < rhs.channel; }
      return lhs.value < rhs.value;
    }
  
    return lhs.location < rhs.location;
  }
};

#endif

