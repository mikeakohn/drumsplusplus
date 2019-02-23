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

int insert_define(char *buffer, char *token, char *value)
{
  int p;

  p = 0;

  while (buffer[p] != 0)
  {
    while (buffer[p] != 0) { p++; }

    p++;
  }

  if (p + strlen(token) + strlen(value) + 3 >= MAX_LITERAL_SPACE)
  {
    return -1;
  }

  strcpy(buffer + p,token);
  p = p + strlen(token) + 1;
  strcpy(buffer + p, value);
  p = p + strlen(value) + 1;
  buffer[p] = 0;

  return 0;
}

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

void print_all(char *buffer)
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

int get_define(char *buffer, char *token, char *value)
{
  int p, i;

  p = 0;
  i = 0;

  while (buffer[p] != 0)
  {
    if ((i % 2) == 0 && strcmp(&buffer[p], token) == 0)
    {
#ifdef DEBUG
printf("%s\n", &buffer[p]);
#endif
      while (buffer[p] != 0) { p++; }
      p++;

      strcpy(value, &buffer[p]);
      return 0;
    }

    while (buffer[p] != 0) { p++; }
    p++;

    i++;
  }

  return -1;
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

int find_section(int i)
{
  int t, ptr;

  t = 0;
  ptr = 0;

  while (1)
  {
    if (i == t) { break; }
    if (sections[ptr++] == -1) { t++; }
  }

  return ptr;
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

