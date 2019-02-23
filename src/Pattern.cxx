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

#include <stdio.h>
#include <stdlib.h>

#include "Pattern.h"

Pattern::Pattern() :
  duration(0),
  volume(0),
  channel(0)
{
}

Pattern::~Pattern()
{
}

void Pattern::print()
{
  printf("  -- Pattern -- \n");
  printf("          Name: %s\n", name.c_str());
}

