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

#ifndef DPP_PARSER_H
#define DPP_PARSER_H

#include <sys/time.h>
#include <map>

#include "MidiFile.h"
#include "Pattern.h"
#include "Section.h"
#include "Tokens.h"

class Song
{
public:
  Song();
  ~Song();

  int parse(Tokens *tokens, MidiFile *midi_file);
  void print_song();
  void set_interactive() { interactive = 1; }

private:
  struct Beat
  {
    float value;
    int instrument;
    int channel;
    uint8_t volume;
  };

  int parse_set(Tokens *tokens);
  int parse_define(Tokens *tokens);
  int parse_include(Tokens *tokens, MidiFile *midi_file);
  int add_beats(Tokens *tokens, Beat *beats, int &ptr, int i, int midi_channel);
  int parse_section(Tokens *tokens);
  int parse_pattern(Tokens *tokens);
  int parse_song(Tokens *tokens, MidiFile *midi_file);
  int play_section(MidiFile *midi_file, std::string &section_name);
  void play_pattern(MidiFile *midi_file, std::string &pattern_name);
  char *dirname_m(char *dir);
  void print_error(Tokens *tokens, const char *expect, const char *got);
  static void signal_catch(int sig);

  SongInfo song_info;
  std::map<std::string, Section> sections;
  std::map<std::string, Pattern> patterns;
  std::map<std::string, std::string> defines;
  std::map<int, std::string> pattern_names;
#ifndef WINDOWS
  struct itimerval play_timer;
#else
  HMIDIOUT inHandle;
  DWORD play_timer;
#endif
  int interactive;

};

#endif

