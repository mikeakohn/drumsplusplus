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

#ifndef DPP_SONG_INFO_H
#define DPP_SONG_INFO_H

#include <string>

class SongInfo
{
public:
  SongInfo();
  ~SongInfo();

  void print();
  const char *get_song_name() { return song_name.c_str(); }

  std::string song_name;
  int bpm;
  int default_volume;
  int drift;
  int time_signature_beats;
  int time_signature_base;
  int midi_channel;
};

#endif

