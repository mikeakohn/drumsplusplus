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

#ifndef DPP_TOKENS_H
#define DPP_TOKENS_H

#include <string>

#define TOKEN_LEN 1024

enum
{
  TOKEN_UNKNOWN = 0,
  TOKEN_ALPHA   = 1,
  TOKEN_NUMBER  = 2,
  TOKEN_SYMBOL  = 3,
  TOKEN_STRING  = 4
};

class Tokens
{
public:
  Tokens();
  ~Tokens();

  int open(const char *filename);
  void close();
  int get(char *token);
  int get_line() { return line; }
  const char *get_filename() { return filename.c_str(); }

  const char *show_token_type(int token_type);

  void push(const char *value, int type)
  {
    pushed_token      = value;
    pushed_token_type = type;
  }

private:
  FILE *in;
  std::string filename;
  std::string pushed_token;
  int pushed_token_type;
  int pushback;
  int line;
};

#endif

