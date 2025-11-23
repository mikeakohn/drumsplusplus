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

#ifndef DPP_BEATS_H
#define DPP_BEATS_H

#include <stdint.h>

#include <string>
#include <set>

class Beats
{
public:
  Beats();
  ~Beats();

  struct Beat
  {
    Beat() :
      value      { 0.0 },
      instrument { 0 },
      channel    { 0 },
      volume     { 0 }
    {
    }

    float value;
    int instrument;
    int channel;
    uint8_t volume;
  };

  void add(Beat &beat);
  void add(float value, int instrument, int channel, uint8_t volume);
  void print();

  struct Compare
  {
    bool operator()(const Beat &lhs, const Beat &rhs) const
    {
      if (lhs.value == rhs.value) { return lhs.instrument < rhs.instrument; }

      return lhs.value < rhs.value;
    }
  };

  std::set<Beat, Compare> beats;
};

#endif

