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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MidiFile.h"
#include "Note.h"
#include "SongInfo.h"

MidiFile::MidiFile() :
  out       { NULL },
  marker    {    0 },
  divisions {  240 },
  tracks    {    0 }
{
}

MidiFile::~MidiFile()
{
}

int MidiFile::open(const char *filename)
{
  out = fopen(filename, "wb");

  if (out == NULL)
  {
    printf("Couldn't open file %s for output.\n", filename);
    return -1;
  }

  return 0;
}

void MidiFile::close()
{
  if (out != NULL)
  {
    fclose(out);
  }
}

void MidiFile::write_header()
{
  if (!is_open()) { return; }

  fprintf(out, "MThd");
  write_int32(6);
  write_int16(0);
  write_int16(tracks);
  write_int16(divisions);
}

void MidiFile::write_track_start(const std::string &track_name)
{
  if (!is_open()) { return; }

  add_track();

  const char *info = "Created by Drums++ 2.0 (https://www.mikekohn.net/).";

  fprintf(out, "MTrk");
  marker = ftell(out);
  write_int32(0);

  if (track_name != "")
  {
    write_var(0);
    putc(0xff, out);
    putc(0x03, out);
    write_var(track_name.size());
    fprintf(out, "%s", track_name.c_str());
  }

  write_var(0);
  putc(0xff, out);
  putc(0x01, out);
  write_var(strlen(info));
  fprintf(out,"%s", info);
}

void MidiFile::write_note(const SongInfo &song_info, const Note &note)
{
  int d;

  if (!is_open()) { return; }

  d = (int)((float)divisions * ((float)note.duration / (float)(60000000 / song_info.bpm)));

  write_var(0);
  putc(0x90 + note.midi_channel, out);

  putc(note.value, out);
  putc(note.volume, out);

  /* if (d != 0 || 1 == 1) */
  {
    write_var(d);
    putc(0x80 + note.midi_channel, out);
    putc(note.value, out);
    /* putc(0, out); */
    putc(64, out);
  }
}

void MidiFile::write_track_end()
{
  int i;

  if (!is_open()) { return; }

  // Add END_OF_TRACK MetaEvent.
  write_var(0);
  putc(0xff, out);
  putc(0x2f, out);
  putc(0x00, out);

  // Compute size of track and update track header.
  i = ftell(out);
  fseek(out, marker, 0);
  write_int32((i - marker) - 4);
  fseek(out, i, 0);
}

void MidiFile::write_bpm(const SongInfo &song_info)
{
  int d;

  if (!is_open()) { return; }

  write_var(0);
  putc(0xff, out);
  putc(0x51, out);
  putc(0x03, out);
  d = 60000000 / song_info.bpm;
  putc(d >> 16, out);
  putc((d >> 8) & 0xff,out);
  putc(d & 0xff, out);
}

void MidiFile::write_time_signature(const SongInfo &song_info)
{
  int d;

  if (!is_open()) { return; }

  d = song_info.time_signature_base;

  switch (song_info.time_signature_base)
  {
    case 32: d = 5; break;
    case 16: d = 4; break;
    case  8: d = 3; break;
    case  4: d = 2; break;
    case  2: d = 1; break;
    case  1: d = 0; break;
    default: return;
  }

  write_var(0);
  putc(0xff, out);
  putc(0x58, out);
  putc(0x04, out);

  putc(song_info.time_signature_beats, out);
  putc(d, out);

  if (d == 3)
  {
    putc(divisions / 3, out);
  }
    else
  {
    putc(divisions, out);
  }

  putc(8, out);
}

void MidiFile::add_track()
{
  tracks += 1;

  long marker = ftell(out);
  fseek(out, 10, SEEK_SET);
  write_int16(tracks);
  fseek(out, marker, SEEK_SET);
}

int MidiFile::write_int32(int n)
{
  putc(((n >> 24) & 0xff), out);
  putc(((n >> 16) & 0xff), out);
  putc(((n >> 8) & 0xff), out);
  putc((n & 0xff), out);

  return 0;
}

int MidiFile::write_int16(int n)
{
  putc(((n >> 8) & 0xff), out);
  putc((n & 0xff), out);

  return 0;
}

void MidiFile::write_var(int value)
{
  int t, k;

  // This really should never happen.
  if (value < 0) { value = 0; }

  t = 7;

  while ((value >> t) != 0)
  {
    t = t + 7;
  }

  t = t - 7;

  for (k = t; k >= 0; k = k - 7)
  {
    if (k != 0)
    {
      putc(((value >> k) & 127) + 128, out);
    }
      else
    {
      putc(((value >> k) & 127), out);
    }
  }
}

