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
#include <string.h>

#include "general.h"
#include "Utility.h"

int insert_literal(char *buffer, char *token)
{
  int p;

  p = 0;

  while (buffer[p] != 0)
  {
    while (buffer[p] != 0) { p++; }

    p++;
  }

  if (p + strlen(token) + 2 >= MAX_LITERAL_SPACE)
  {
    return -1;
  }

  strcpy(buffer + p, token);
  p = p + strlen(token) + 1;
  buffer[p] = 0;

  return 0;
}

void print_all(const char *buffer)
{
  int p;

  p = 0;
  while (buffer[p] != 0)
  {
    printf("%s\n", &buffer[p]);

    while (buffer[p] != 0) { p++; }

    p++;
  }
}

void print_name(char *buffer, int i)
{
 int p,k;

  p = 0;
  k = 0;

  while (buffer[p] != 0)
  {
    if (k == i) { printf("%s\n",&buffer[p]); }

    while (buffer[p] != 0) { p++; }

    p++;
    k++;
  }
}

int get_literal(char *buffer, char *token)
{
  int p,i;

  p = 0;
  i = 0;

  while (buffer[p] != 0)
  {
    if (strcmp(&buffer[p], token) == 0)
    {
      return i;
    }

    while (buffer[p] != 0) { p++; }
    p++;

    i++;
  }

  return -1;
}

int is_number(char *s)
{
  int r = 0;

  while ((s[r] != ' '  && s[r] != '\t') &&
        ((s[r] != '\n' && s[r] != '\r') && s[r] != 0))
  {
    if (s[r] < '0' || s[r] > '9') { return 0; }
    r++;
  }

  if (r == 0) { return 0; }

  return 1;
}

int find_pattern(int i)
{
  int t, ptr;

  t = 0;
  ptr = 0;

  while(1)
  {
    if (i == t) { break; }
    if (pattern[ptr++] == 255) { t++; }
  }

  return ptr;
}

