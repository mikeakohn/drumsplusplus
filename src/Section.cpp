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

#include <stdio.h>
#include <stdlib.h>

#include "Section.h"

Section::Section()
{
}

Section::~Section()
{
}

void Section::add_pattern(int index, int repeat)
{
  int n;

  for (n = 0; n < repeat; n++)
  {
    patterns.push_back(index);
  }
}

void Section::print()
{
  printf("  -- Section --\n");

  for (auto it = patterns.begin(); it != patterns.end(); it++)
  {
    printf(" %d", *it);
  }

  printf("\n");
}

