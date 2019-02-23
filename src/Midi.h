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

#ifndef DPP_MIDI_H
#define DPP_MIDI_H

#include "Note.h"

void write_midi_header(FILE *out);
void write_midi_note(FILE *out, Note *note);
void write_midi_footer(FILE *out);
void write_midi_bpm(FILE *out);
void write_midi_timesignature(FILE *out);

#endif

