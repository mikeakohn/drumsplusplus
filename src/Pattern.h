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

#ifndef DPP_PATTERN_H
#define DPP_PATTERN_H

#include "stdint.h"

#include <string>
#include <vector>

#include "MidiData.h"

class Pattern
{
public:
  Pattern();
  ~Pattern();

  void set_name(std::string &value) { name = value; }
  void set_index(int value)         { index = value; }

  int get_index()         { return index; }
  std::string &get_name() { return name; }
  int get_count()         { return midi_data.size(); }

  MidiData &get_data(int index) { return midi_data[index]; }

  void add(MidiData &midi_data);
  void add(uint8_t value, uint8_t volume, uint8_t channel, uint32_t duration);
  void print();

private:
  std::string name;
  std::vector<MidiData> midi_data;
  int index;
};

#endif

