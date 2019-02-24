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

#include "Tokens.h"

/*

Token Types:

1: alpha
2: number
3: symbol
4: string in ""

*/

Tokens::Tokens() :
  in(NULL),
  pushback(0),
  line(1)
{
}

Tokens::~Tokens()
{
}

int Tokens::open(const char *filename)
{
  in = fopen(filename, "rb");

  if (in == NULL)
  {
    return -1;
  }

  this->filename = filename;

  return 0;
}

void Tokens::close()
{
  fclose(in);
  in = NULL;
}

int Tokens::get(char *token)
{
  int token_type = 0;
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

      if (token_type != 0) { break; }
      continue;
    } 

    ch = tolower(ch);

    if (token_type == 0)
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
          token_type = TOKEN_SYMBOL;
          break;
        }
      }
        else
      if (ch >= 'a' && ch <= 'z')
      {
        token[ptr++] = ch;
        token_type = TOKEN_ALPHA;
      }
        else
      if (ch >= '0' && ch <= '9')
      {
        token[ptr++] = ch;
        token_type = TOKEN_NUMBER;
      }
        else
      if (ch == '.')
      {
        token[ptr++] = '0';
        token[ptr++] = ch;
        token_type = TOKEN_NUMBER;
      }
        else
      if (ch == '"')
      {
        token_type = TOKEN_STRING;
      }
        else
      {
        token[ptr++] = ch;
        token_type = TOKEN_SYMBOL;
        break;
      }
    }
      else
    if (token_type == TOKEN_ALPHA)
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
    if (token_type == TOKEN_NUMBER)
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
    if (token_type == TOKEN_STRING)
    {
      if (ch == '"') { break; }
      token[ptr++] = ch;
    }
  }

  if (token_type == TOKEN_STRING && ch != '"')
  {
    printf(">> In file: %s\n", get_filename());
    printf("Error: Unterminated string on line %d.\n", line);
    return -1;
  }

  token[ptr] = 0;
  if (ptr == 0) { return -1; }

  return token_type;
}

