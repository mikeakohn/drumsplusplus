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

class Pattern
{
public:
  Pattern();
  ~Pattern();

private:
  std::string name;
  int duration;
  int volume;
  int channel;

};

#endif

