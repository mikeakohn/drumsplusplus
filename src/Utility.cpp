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
#include <string.h>

#include "Utility.h"

bool Utility::is_number(const char *s)
{
  int r = 0;

  while ((s[r] != ' '  && s[r] != '\t') &&
        ((s[r] != '\n' && s[r] != '\r') && s[r] != 0))
  {
    if (s[r] < '0' || s[r] > '9') { return false; }
    r++;
  }

  return r == 0 ? false : true;
}

