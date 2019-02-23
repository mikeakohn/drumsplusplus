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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef WINDOWS
#include <windows.h>
#include <mmsystem.h>
#endif

#include "general.h"
#include "MidiPlayer.h"

#define DIVISIONS 240

MidiPlayer::MidiPlayer() : midiout(0)
{
}

MidiPlayer::~MidiPlayer()
{
}

int MidiPlayer::open(const char *filename)
{
#ifndef WINDOWS
    midiout = ::open(filename, O_WRONLY, 0);

    if (midiout < 0)
    {
      printf("Error opening up sequencer.\n");
      return -1;
    }
#else
    int result = midiOutOpen(&inHandle, (UINT) -1, 0, 0, CALLBACK_NULL);

    if (result)
    {
      printf("Couldn't open MIDI device.\n");
      return -1;
    }
#endif

  return 0;
}

void MidiPlayer::play(uint8_t *midi_data, int length)
{
#ifndef WINDOWS
  //if (pattern_volume[ptr] != 0)
  {
    int n = 0;

    while (n < length)
    {
      int i = write(midiout, midi_data + n, length - n);
      if (i < 0) { break; }
      n += i;
    }
  }
#else
  for (r = 0; r < length; r = r + 3)
  {
    n = midi_data[r] + (midi_data[r + 1] << 8) + (midi_data[r + 2] << 16);
    midiOutShortMsg(inHandle, n);
  }
#endif
}

void MidiPlayer::close()
{
#ifndef WINDOWS
    ::close(midiout);
#else
    midiOutClose(inHandle);
#endif
}

