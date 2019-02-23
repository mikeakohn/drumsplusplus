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

class Pattern
{
public:
  Pattern();
  ~Pattern();

  void set_name(const char *name) { this->name = name; }
  void set_duration(int duration) { this->duration = duration; }
  void set_volume(int volume) { this->volume = volume; }
  void set_channel(int channel) { this->channel = channel; }
  void print();

private:
  std::string name;
  int duration;
  int volume;
  int channel;

};

#endif

