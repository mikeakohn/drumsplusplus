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

#ifndef DPP_MIDI_PLAYER_H
#define DPP_MIDI_PLAYER_H

#include <stdint.h>

class MidiPlayer
{
public:
  MidiPlayer();
  ~MidiPlayer();

  int open(const char *filename);
  void play(uint8_t *midi_data, int length);
  void close();

private:
  int midiout;
#ifdef WINDOWS
  HMIDIOUT inHandle;
#endif
};

#endif

