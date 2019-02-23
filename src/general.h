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
extern char defines[MAX_LITERAL_SPACE];
extern int line;
//extern struct SongInfo song_info;
extern unsigned char pattern[PATTERN_HEAP_SIZE];
extern unsigned int pattern_duration[PATTERN_HEAP_SIZE];
extern unsigned char pattern_volume[PATTERN_HEAP_SIZE];
extern unsigned char pattern_channel[PATTERN_HEAP_SIZE];
extern int sections[SECTIONS_HEAP_SIZE];
extern int song[MAX_SONG_SIZE];
extern int patterns_ptr;
extern int sections_ptr;
extern char pattern_names[MAX_LITERAL_SPACE];
extern char section_names[MAX_LITERAL_SPACE];
//extern unsigned char song_name[256];
//extern int midiout;
extern char interactive;
//extern FILE *out;
//extern const char *current_filename;

#endif

