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
#include "MidiPlayer.h"
#include "Pattern.h"
#include "Section.h"
#include "Tokens.h"

class Song
{
public:
  Song();
  ~Song();

  void set_midi(MidiFile *midi_file, MidiPlayer *midi_player);
  int parse(Tokens *tokens);
  void print();
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
  int parse_include(Tokens *tokens);
  int add_beats(Tokens *tokens, Beat *beats, int &ptr, int i, int midi_channel);
  int parse_section(Tokens *tokens);
  int parse_pattern(Tokens *tokens);
  int parse_song(Tokens *tokens);
  int play_section(std::string &section_name);
  void play_pattern(std::string &pattern_name);
  char *dirname_m(char *dir);
  void print_error(Tokens *tokens, const char *expect, const char *got);
  static void signal_catch(int sig);

  SongInfo song_info;
  MidiFile *midi_file;
  MidiPlayer *midi_player;
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

