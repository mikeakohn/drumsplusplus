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

#ifndef DPP_GENERAL_H
#define DPP_GENERAL_H

#include "SongInfo.h"

#define MAX_LITERAL_SPACE 16738
#define PATTERN_HEAP_SIZE 16738
#define SECTIONS_HEAP_SIZE 16738
#define MAX_SONG_SIZE 2048

// FIXME - OUCH
extern unsigned char pattern[PATTERN_HEAP_SIZE];
extern unsigned int pattern_duration[PATTERN_HEAP_SIZE];
extern unsigned char pattern_volume[PATTERN_HEAP_SIZE];
extern unsigned char pattern_channel[PATTERN_HEAP_SIZE];
extern int patterns_ptr;
extern char pattern_names[MAX_LITERAL_SPACE];
#endif

