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

#ifndef DPP_SECTION_H
#define DPP_SECTION_H

#include <string>
#include <vector>

class Section
{
public:
  Section();
  ~Section();

  void add_pattern(int index, int repeat);
  std::vector<int> &get_patterns() { return patterns; }
  void print();

private:
  std::vector<int> patterns;

};

#endif

