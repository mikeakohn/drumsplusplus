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
      location { 0.0 },
      value    { 0 },
      length   { 0 },
      channel  { 0 },
      volume   { 0 }
    {
    }

    void print() const
    {
      printf("  Tone: [ %f, %d, %d, %d, %d ]\n",
        location,
        value,
        length,
        channel,
        volume);
    }

    float location;
    int value;
    int length;
    int channel;
    uint8_t volume;
  };

  void add(Tone &tone);
  void add(float location, int value, int length, int channel, uint8_t volume);
  void print();

  struct Compare
  {
    bool operator()(const Tone &lhs, const Tone &rhs) const
    {
      if (lhs.location == rhs.location) { return lhs.value < rhs.value; }

      return lhs.location < rhs.location;
    }
  };

  std::set<Tone, Compare> tones;
};

#endif

