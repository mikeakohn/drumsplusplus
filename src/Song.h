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

#ifndef DPP_PARSER_H
#define DPP_PARSER_H

#include <sys/time.h>
#include <map>

#include "Beats.h"
#include "MidiFile.h"
#include "Pattern.h"
#include "Section.h"
#include "Tokens.h"

class Song
{
public:
  Song();
  ~Song();

  void set_midi(MidiFile *midi_file);
  int parse(Tokens &tokens);
  void print();
  void set_interactive() { interactive = 1; }

private:
  int parse_set(Tokens &tokens);
  int parse_define(Tokens &tokens);
  int parse_include(Tokens &tokens);
  int add_beats(Tokens &tokens, Beats &beats, int i, int midi_channel);
  int parse_section(Tokens &tokens);
  int parse_pattern(Tokens &tokens);
  int parse_song(Tokens &tokens);
  int play_section(std::string &section_name);
  void play_pattern(std::string &pattern_name);
  char *dirname_m(char *dir);
  void print_error(Tokens &tokens, const char *expect, const char *got);

  SongInfo song_info;
  MidiFile *midi_file;
  std::map<std::string, Section> sections;
  std::map<std::string, Pattern> patterns;
  std::map<std::string, std::string> defines;
  std::map<int, std::string> pattern_names;

  bool interactive;
};

#endif

