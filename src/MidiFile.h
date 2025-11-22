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

#ifndef DPP_MIDI_FILE_H
#define DPP_MIDI_FILE_H

#include "Note.h"
#include "SongInfo.h"

class MidiFile
{
public:
  MidiFile();
  ~MidiFile();

  int open(const char *filename);
  void close();
  bool is_open() { return out != NULL; }

  void write_header(SongInfo *song_info);
  void write_note(SongInfo *song_info, Note *note);
  void write_footer();
  void write_bpm(SongInfo *song_info);
  void write_time_signature(SongInfo *song_info);

private:
  int write_int32(int n);
  int write_int16(int n);
  void write_var(int i);

  FILE *out;
  long marker;
};

#endif

