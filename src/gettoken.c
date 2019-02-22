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
#include <ctype.h>

#include "general.h"

int pushback = 0;

/*

Token Types:

1: alpha
2: number
3: symbol
4: string in ""

*/

int gettoken(FILE *in, char *token)
{
  int tokentype = 0;
  int ch, ptr;
  int dotflag;

  ptr = 0;
  dotflag = 0;

  while (1)
  {
    if (pushback == 0)
    {
      ch=getc(in);
    }
      else
    {
      ch = pushback;
      pushback = 0;
    }
    
    if (ch == EOF) { break; }
    if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
    {
      if (ch == '\r' || ch == '\n') { line++; }
      if (ch == '\r')
      {
        ch = getc(in);
        if (ch!='\n') { pushback = ch; }
      }

      if (tokentype != 0) { break; }
      continue;
    } 

    ch = tolower(ch);

    if (tokentype == 0)
    {
      if (ch == '/')
      {
        ch = getc(in);

        if (ch == '/')
        {
          while (1)
          {
            ch = getc(in);
            if (ch == '\n' || ch == '\r' || ch == EOF) { break; }
          }

          line++;

          if (ch == '\r')
          {
            ch = getc(in);
            if (ch != '\n') { pushback = ch; }
          }
          continue;
        }
          else
        if (ch == '*')
        {
          while (1)
          {
            ch = getc(in);

            if (ch == EOF) { return -1; }
            if (ch == '*')
            {
              // FIXME: Get rid of this goto :(
again:
              ch = getc(in);

              if (ch == '/') { break; }
              if (ch == '*') { goto again; }
            }
          }

          continue;
        }
          else
        {
          pushback = ch;
          token[ptr++] = '/';
          tokentype = 3;
          break;
        }
      }
        else
      if (ch >= 'a' && ch <= 'z')
      {
        token[ptr++] = ch;
        tokentype = 1;
      }
        else
      if (ch >= '0' && ch <= '9')
      {
        token[ptr++] = ch;
        tokentype = 2;
      }
        else
      if (ch == '.')
      {
        token[ptr++] = '0';
        token[ptr++] = ch;
        tokentype = 2;
      }
        else
      if (ch == '"')
      {
        /* token[ptr++] = ch; */
        tokentype = 4;
      }
        else
      {
        token[ptr++] = ch;
        tokentype = 3;
        break;
      }
    }
      else
    if (tokentype == 1)
    {
      if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_')
      {
        token[ptr++] = ch;
      }
        else
      {
        pushback = ch;
        break;
      }
    }
      else
    if (tokentype == 2)
    {
      if (ch >= '0' && ch <= '9')
      {
        token[ptr++] = ch;
      }
        else
      if (ch == '.' && dotflag == 0)
      {
        token[ptr++] = ch;
        dotflag++;
      }
        else
      {
        pushback = ch;
        break;
      }
    }
      else
    if (tokentype == 4)
    {
      if (ch == '"') { break; }
      token[ptr++] = ch;
    }
  }

  if (tokentype == 4 && ch != '"')
  {
    printf(">> In file: %s\n", current_filename);
    printf("Error: Unterminated string on line %d.\n", line);
    return -1;
  }

  token[ptr] = 0;
  if (ptr == 0) { return -1; }

  return tokentype;
}

