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

#ifndef DPP_TOKENS_H
#define DPP_TOKENS_H

#include <string>

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

private:
  FILE *in;
  std::string filename;
  int pushback;
  int line;
};

#endif

