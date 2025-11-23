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

#ifndef DPP_TONES_H
#define DPP_TONES_H

#include <stdint.h>

#include <string>
#include <set>

class Tones
{
public:
  Tones();
  ~Tones();

  struct Tone
  {
    Tone() :
      length  { 0.0 },
      value   { 0 },
      channel { 0 },
      volume  { 0 }
    {
    }

    void print() const
    {
      printf("  Tone: [ %f, %d, %d, %d ]\n",
        length,
        value,
        channel,
        volume);
    }

    float length;
    int value;
    int channel;
    uint8_t volume;
  };

  void add(Tone &tone);
  void add(float length, int instrument, int channel, uint8_t volume);
  void print();

  struct Compare
  {
    bool operator()(const Tone &lhs, const Tone &rhs) const
    {
      if (lhs.length == rhs.length) { return lhs.value < rhs.value; }

      return lhs.length < rhs.length;
    }
  };

  std::set<Tone, Compare> tones;
};

#endif

