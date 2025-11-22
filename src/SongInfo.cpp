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

#include "SongInfo.h"

SongInfo::SongInfo() :
  bpm                  { 120 },
  default_volume       { 127 },
  drift                {   5 },
  time_signature_beats {   4 },
  time_signature_base  {   4 },
  midi_channel         {   9 }
{
  song_name = "undefined";
}

SongInfo::~SongInfo()
{
}

void SongInfo::print()
{
  printf("     Song Name: %s\n", song_name.c_str());
  printf("           BMP: %d\n", bpm);
  printf("Default Volume: %d\n", default_volume);
  printf("         Drift: %d\n", drift);
  printf(" Timesignature: %d/%d\n", time_signature_beats, time_signature_base);
  printf("  MIDI Channel: %d\n", midi_channel);
}

