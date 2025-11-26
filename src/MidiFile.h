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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>

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

  void write_header();
  void write_track_start(const std::string &track_name);
  void write_note(const SongInfo &song_info, const Note &note);
  void write_note_on(int value, int midi_channel, int divisions, int volume);
  void write_note_off(int value, int midi_channel, int divisions);
  void write_track_end();
  void write_bpm(const SongInfo &song_info);
  void write_time_signature(const SongInfo &song_info);

  int get_divisions() { return divisions; }

  void add_track();

private:
  int write_int32(int n);
  int write_int16(int n);
  void write_var(int value);

  FILE *out;
  long marker;
  int divisions;
  int tracks;
};

#endif

