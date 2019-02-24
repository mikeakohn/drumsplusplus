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

#ifndef DPP_PATTERN_H
#define DPP_PATTERN_H

#include <string>
#include <vector>

class Pattern
{
public:
  Pattern();
  ~Pattern();

  struct Data
  {
    Data() : value(0), volume(0), channel(0), duration(0) { }
    uint8_t value;
    uint8_t volume;
    uint8_t channel;
    uint32_t duration;
  };

  void set_name(std::string &name) { this->name = name; }
  void set_index(int index) { this->index = index; }
  int get_index() { return index; }
  std::string &get_name() { return name; }
  int get_count() { return data.size(); }
  Data &get_data(int index) { return data[index]; }

  void add(uint8_t value, uint8_t volume, uint8_t channel, uint32_t duration);
  void print();

private:
  std::string name;
  std::vector<Data> data;
  int index;
};

#endif

