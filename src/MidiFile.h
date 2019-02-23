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

#ifndef DPP_MIDI_FILE_H
#define DPP_MIDI_FILE_H

#include "Note.h"

class MidiFile
{
public:
  MidiFile();
  ~MidiFile();

  int open(const char *filename);
  void close();
  bool is_open() { return out != NULL; }

  void write_header(const char *song_name);
  void write_note(Note *note);
  void write_footer();
  void write_bpm();
  void write_time_signature();

private:
  int write_int32(int n);
  int write_int16(int n);
  void write_var(int i);

  FILE *out;
  long marker;
};

#endif

