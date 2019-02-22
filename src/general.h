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

#define DLPLAYER_INFO "\nDrums++ Player Version 0.97 - Feburary 22, 2019\n"
#define COPYRIGHT "Copyright 2003-2019 Michael Kohn\n\n"
#define MAX_LITERAL_SPACE 16738
#define PATTERN_HEAP_SIZE 16738
#define SECTIONS_HEAP_SIZE 16738
#define MAX_SONG_SIZE 2048

struct song_info_t
{
  int bpm;
  int defaultvolume;
  int drift;
  int timesignature_beats;
  int timesignature_base;
  int midi_channel;
};

struct note_t
{
  unsigned char instrument;
  unsigned int volume;
  int duration;
  int midi_channel;
};

char defines[MAX_LITERAL_SPACE];
int line;
struct song_info_t song_info;
unsigned char pattern[PATTERN_HEAP_SIZE];
unsigned int pattern_duration[PATTERN_HEAP_SIZE];
unsigned char pattern_volume[PATTERN_HEAP_SIZE];
unsigned char pattern_channel[PATTERN_HEAP_SIZE];
int sections[SECTIONS_HEAP_SIZE];
int song[MAX_SONG_SIZE];
int patterns_ptr;
int sections_ptr;
unsigned char pattern_names[MAX_LITERAL_SPACE];
unsigned char section_names[MAX_LITERAL_SPACE];
unsigned char song_name[256];
int midiout;
char interactive;
FILE *out;
char *current_filename;

/* parser.c */

void error(char *expect, char *got);
int parse_set(FILE *in);
int main_parser(FILE *in);

/* utility.c */

int insert_define(char *buffer, char *token, char *value);
int insert_literal(char *buffer, char *token);
void print_all(char *buffer);
int get_define(char *buffer, char *token, char *value);
int get_literal(char *buffer, char *token);
int is_number(char *myString);
int search_end_buffer(int *buffer);
int find_section(int i);
int find_pattern(int i);
void print_name(char *buffer, int i);

/* midi.c */

void write_midi_header(FILE *out);
void write_midi_note(FILE *out, struct note_t *note);
void write_midi_footer(FILE *out);
void write_midi_bpm(FILE *out);
void write_midi_timesignature(FILE *out);

#endif

