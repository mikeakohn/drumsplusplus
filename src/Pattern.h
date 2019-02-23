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

  void print();

  std::string name;

};

#endif

