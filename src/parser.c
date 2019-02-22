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
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#ifndef WINDOWS
#include <libgen.h>
#include <signal.h>
#else
#include <windows.h>
#include <mmsystem.h>
#endif

#include "general.h"
#include "gettoken.h"
#include "midi.h"
#include "parser.h"
#include "utility.h"

#ifndef WINDOWS

struct itimerval mr_timer;

void signal_catch()
{
  signal(SIGPROF, signal_catch);
}
#else

HMIDIOUT inHandle;
DWORD mr_timer = 0;

#endif

char *dirname_m(char *dir)
{
  int i;

  i = strlen(dir);

  while (i >= 0)
  {
    if (dir[i] == '/' || dir[i] == '\\') { break; }
    i--;
  }

  if (i < 0) { return NULL; }

  dir[i] = 0;

  return dir + i;
}

void error(char *expect, char *got)
{
  printf(">> In file: %s\n", current_filename);
  printf("Parse Error:  Expected '%s' and got '%s' on line %d.\n",
    expect, got, line);
}

int parse_set(FILE *in)
{
  int tokentype;
  char token[1024];
  char equals[1024];
  char value[1024];

  tokentype = gettoken(in, token);

  if (tokentype != 1)
  {
    error("setting name", token);
    return -1;
  }

  tokentype = gettoken(in, equals);

  if (strcmp(equals, "=") != 0)
  {
    error("=", equals);
    return -1;
  }

  tokentype = gettoken(in, value);

  if (tokentype != 2)
  {
    error("number",equals);
    return -1;
  }

  if (strcmp(token, "bpm") == 0)
  {
    song_info.bpm = atoi(value);

    if (song_info.bpm == 0)
    {
      printf(">> In file: %s\n", current_filename);
      printf("Error: Tempo (bpm) must be non-zero; line %d\n", line);
      return -1;
    }

    /* write_midi_bpm(out); */
  }
    else
  if (strcmp(token, "defaultvolume") == 0)
  {
    song_info.defaultvolume = atoi(value);

    if (song_info.defaultvolume > 127)
    {
      printf("Warning: defaultvolume is higher than the maximum 127; line %d\n", line);
      song_info.defaultvolume = 127;
    }
  }
    else
  if (strcmp(token, "drift") == 0)
  {
     song_info.drift = atoi(value);
  }
    else
  if (strcmp(token, "timesignature") == 0)
  {
    song_info.timesignature_beats = atoi(value);

    tokentype = gettoken(in, equals);

    if (strcmp(equals, "/") != 0)
    {
      error("/", equals);
      return -1;
    }

    tokentype = gettoken(in, value);

    if (tokentype != 2)
    {
      error("number", value);
      return -1;
    }

    song_info.timesignature_base = atoi(value);
    /* write_midi_timesignature(out); */

    if (song_info.timesignature_beats == 0)
    {
      printf(">> In file: %s\n", current_filename);
      printf("Error: Timesignature numerator must be non-zero; line %d\n", line);
      return -1;
    }
      else
    if (song_info.timesignature_base == 0)
    {
      printf(">> In file: %s\n", current_filename);
      printf("Error: Timesignature denominator must be non-zero; line %d\n", line);
      return -1;
    }
  }
    else
  if (strcmp(token, "midi_channel") == 0)
  {
    song_info.midi_channel = atoi(value);

    if (song_info.midi_channel > 15)
    {
      printf("Warning: MIDI channel %d is higher than the maximum 15; line %d\n", song_info.midi_channel, line);
      song_info.midi_channel = 9;
    }
  }
    else
  {
    printf(">> In file: %s\n", current_filename);
    printf("Error: Unknown setting '%s' on line %d.\n", token, line);
    return -1;
  }

  tokentype = gettoken(in, equals);

  if (strcmp(equals, ";") != 0)
  {
    error(";", equals);
    return -1;
  }

  return 0;
}

int parse_define(FILE *in)
{
  int tokentype;
  char token[1024];
  char value[1024];

  tokentype = gettoken(in, token);

  if (tokentype != 1)
  {
    error("define", token);
    return -1;
  }

  tokentype = gettoken(in, value);

  insert_define(defines, token, value);

  return 0;
}

int parse_include(FILE *in)
{
  int tokentype;
  char token[1024];
  char filename[1024];
  char lastfilename[1024];
  char *old_filename;
  int old_line;
  FILE *include_in;

  tokentype = gettoken(in, token);

  if (tokentype != 4)
  {
    error("filename", token);
    return -1;
  }

  filename[0] = 0;

  if (token[0] != '/')
  {
    strcpy(lastfilename, current_filename);

    if (dirname_m(lastfilename) != 0)
    {
      strcpy(filename,lastfilename);
      strcat(filename, "/");
    }
  }
  strcat(filename, token);

  include_in=fopen(filename, "rb");
  if (include_in == 0)
  {
    printf(">> In file: %s\n", current_filename);
    printf("Error: included file not found: %s on line %d.\n",filename, line);
  }
    else
  {
    old_line = line;
    line = 1;
    old_filename = current_filename;
    current_filename = filename;

    if (main_parser(include_in) == -1) { return -1; }

    current_filename = old_filename;
    line = old_line;
  }

  return 0;
}

int add_beats(
  FILE *in,
  int *instrument,
  float *beat,
  unsigned char *volume,
  int *ptr,
  int i,
  int *channel,
  int midi_channel)
{
  int tokentype;
  char token[1024];
  int m, modifier, c;

  c = 0;

  tokentype = gettoken(in, token);

  if (strcmp(token, "/") == 0)
  {
    tokentype = gettoken(in, token);

    if (tokentype != 2)
    {
      error("Channel number", token);
      return -1;
    }

    midi_channel = atoi(token);

    if (midi_channel > 15)
    {
      printf("Warning: MIDI channel %d is higher than the maximum 15; line %d\n",midi_channel, line);
      midi_channel=9;
    }
    tokentype = gettoken(in, token);
  }

  if (strcmp(token, ":") != 0)
  {
    error(":", token);
    return -1;
  }

  while(1)
  {
    tokentype = gettoken(in, token);

    if (strcmp(token, ";") == 0) return 0;

    if (strcmp(token, ":") == 0)
    {
      if (c == 0)
      {
        printf(">> In file: %s\n", current_filename);
        printf("Parse Error: Unexpected token %s on line %d.\n", token, line);
        return -1;
      }

      tokentype = gettoken(in, token);

      if (strcmp(token, "%") == 0)
      {
        srand(time(NULL));
        modifier = rand() % song_info.drift;
        if ((rand() % 2) == 0) { modifier = -modifier; }
        tokentype = gettoken(in, token);
      }

      if (tokentype != 2)
      {
        error("volume integer", token);
        return -1;
      }

      m = (int)atof(token);

      if (m > 127)
      {
        printf("Warning: Volume is higher than 127 on line %d.\n", line);
      }

      m = m + modifier;
      if (m < 0) { m = 0; }

      if (m > 127)
      {
        m=127;
      }

      volume[(*ptr) - 1] = m;
      continue;
    }

    if (tokentype != 2)
    {
      error("a beat number", token);
      continue;
    }

    instrument[*ptr] = i;
    beat[*ptr] = atof(token);
    channel[*ptr] = midi_channel;

    if (beat[*ptr] < 1)
    {
      printf(">> In file: %s\n", current_filename);
      printf("Error: Beat is less than 1 on line %d.  Ignoring.\n", line);
      beat[*ptr] = 0;
    }

    if (beat[*ptr] >= song_info.timesignature_beats + 1)
    {
      printf(">> In file: %s\n", current_filename);
      printf("Error: Beat is more than beats per measure on line %d.  Ignoring.\n", line);
      beat[*ptr] = 0;
    }

    volume[*ptr] = song_info.defaultvolume;
    (*ptr)++;
    c++;
  }
}

int parse_pattern(FILE *in)
{
  int tokentype;
  char token[1024], token1[1024];
  int instrument[256];
  int channel[256];
  float beat[256];
  unsigned char volume[256];
  int ptr;
  char value[1024];
  float low_beat = 0;
  float next_beat = 0;
  int i, count, num_notes;
  int beat_time;
  int timesignature_beats, timesignature_base;
  int midi_channel;

  beat_time = 60000000 / song_info.bpm;
  timesignature_beats = song_info.timesignature_beats;
  timesignature_base = song_info.timesignature_base;
  midi_channel = song_info.midi_channel;

  tokentype = gettoken(in, token);

  if (tokentype != 1)
  {
    printf(">> In file: %s\n", current_filename);
    printf("Error: Pattern is not alphanumeric on line %d\n", line);
    return -1;
  }

  insert_literal((char *)pattern_names, token);

#ifdef DEBUG
printf("parsing pattern: %s\n", token);
#endif

  tokentype = gettoken(in, token);

  if (strcmp(token, "{") != 0)
  {
    error("{", token);
    return -1;
  }

  ptr = 0;
  num_notes = 0;

  while(1)
  {
    tokentype = gettoken(in, token);

    if (strcmp(token, "}") == 0) { break;}

    if (strcmp(token, "set") == 0)
    {
      tokentype = gettoken(in, token);

      if (tokentype!=1)
      {
        error("variable to set", token);
        return -1;
      }

      tokentype = gettoken(in, token1);

      if (strcmp(token1, "=") != 0)
      {
        error("=", token1);
        return -1;
      }

      tokentype = gettoken(in, token1);

      if (tokentype != 2)
      {
        error("a number", token1);
        return -1;
      }

      i = atoi(token1);

      if (i == 0)
      {
        error("a non-zero integer", token1);
        return -1;
      }

      if (strcmp(token, "bpm") == 0)
      {
        beat_time = 60000000 / i;
      }
        else
      if (strcmp(token, "timesignature") == 0)
      {
        timesignature_beats = i;

        tokentype = gettoken(in, token1);

        if (strcmp(token1, "/") != 0)
        {
          error("/", token1);
          return -1;
        }

        tokentype = gettoken(in, token1);

        if (tokentype != 2)
        {
          error("a number", token1);
          return -1;
        }

        i = atoi(token1);

        if (i == 0)
        {
          error("a non-zero integer", token1);
          return -1;
        }

        timesignature_base = i;
      }
        else
      if (strcmp(token, "midi_channel") == 0)
      {
        midi_channel = i;

        if (midi_channel > 15)
        {
          printf("Warning: MIDI channel %d is higher than the maximum 15; line %d\n",midi_channel, line);
          midi_channel = 9;
        }
      }
        else
      {
        printf("Warning: Cannot set '%s' in pattern.\n", token);
      }

      tokentype = gettoken(in, token);

      if (strcmp(token, ";") != 0)
      {
        error(";", token);
        return -1;
      }

      continue;
    }

    if (tokentype==2)
    {
      channel[ptr] = midi_channel;
      if (add_beats(in, instrument, beat, volume, &ptr, atoi(token), channel, midi_channel) == -1)
      {
        return -1;
      }
    }
      else
    {
      if (get_define(defines, token, value) == -1)
      {
        printf(">> In file: %s\n", current_filename);
        printf("Error: %s is undefined on line %d\n", token, line);
        return -1;
      }

      if (is_number(value) == 0)
      {
        printf(">> In file: %s\n", current_filename);
        printf("Error: %s is undefined is not defined as a number line %d\n", token, line);
        return -1;
      }

      channel[ptr] = midi_channel;

      if (add_beats(in, instrument, beat, volume, &ptr, atoi(value), channel, midi_channel) == -1) { return -1; }
    }
  }

  while(1)
  {
    low_beat = timesignature_beats + 1;
    next_beat = timesignature_beats + 1;
    count = 0;

    for (i = 0; i < ptr; i++)
    {
      if (beat[i] > 0)
      {
        if (low_beat > beat[i])
        {
          low_beat = beat[i];
          count = 0;
        }

        if (beat[i] == low_beat) { count++; }
      }
    }

    if (count == 0)
    {
      if (ptr == 0)
      {
        pattern[patterns_ptr] = 0;
        pattern_duration[patterns_ptr] = (int)((timesignature_beats)*beat_time);
        pattern_volume[patterns_ptr] = 0;
        pattern_channel[patterns_ptr] = midi_channel;
        patterns_ptr++;
      }

      break;
    }

    for (i = 0; i < ptr; i++)
    {
      if (beat[i] > 0)
      {
        if (next_beat > beat[i] && beat[i] != low_beat)
        {
          next_beat = beat[i];
        }
      }
    }

    for (i = 0; i < ptr; i++)
    {
      if (beat[i] == low_beat)
      {
        if (num_notes == 0 && low_beat != 1)
        {
          pattern[patterns_ptr] = 0;
          pattern_duration[patterns_ptr] = (int)((low_beat - 1) * beat_time);
          pattern_volume[patterns_ptr] = 0;
          pattern_channel[patterns_ptr] = midi_channel;
          patterns_ptr++;
        }

        pattern[patterns_ptr] = instrument[i];

        if (count == 1)
        {
          pattern_duration[patterns_ptr] = (int)((next_beat - low_beat) * beat_time);
        }
          else
        {
          pattern_duration[patterns_ptr] = 0;
        }

#ifdef DEBUG
printf("%f %f %d %d\n",low_beat,next_beat,pattern_duration[patterns_ptr],pattern[patterns_ptr]);
#endif
        pattern_volume[patterns_ptr] = volume[i];
        pattern_channel[patterns_ptr] = channel[i];
        patterns_ptr++;

        beat[i] = 0;

        count--;
      }
    }

    num_notes++;
  }

  pattern[patterns_ptr++] = 255;

  /* printf("%d %f %d\n",instrument[i],beat[i],volume[i]); */

  return 0;
}

int parse_section(FILE *in)
{
  int tokentype;
  char token[1024];
  int i,x;
  int repeat;

  tokentype = gettoken(in, token);

  if (tokentype != 1)
  {
    printf(">> In file: %s\n", current_filename);
    printf("Error: Section is not alphanumeric on line %d\n", line);
    return -1;
  }

#ifdef DEBUG
printf("parsing section: %s\n", token);
#endif

  insert_literal((char *)section_names, token);

  tokentype = gettoken(in, token);

  if (strcmp(token, "{") != 0)
  {
    error("{", token);
    return -1;
  }

  while(1)
  {
    tokentype = gettoken(in, token);

    if (strcmp(token, "}") == 0)
    {
      break;
    }

    if (strcmp(token, "play") == 0)
    {
      tokentype = gettoken(in, token);

      if (strcmp(token, ":") != 0)
      {
        error(":", token);
        return -1;
      }

      while(1)
      {
        repeat = 1;
        tokentype = gettoken(in, token);

        if (tokentype == 2)
        {
          repeat = atoi(token);
          tokentype = gettoken(in, token);
        }

        i = get_literal((char *)pattern_names, token);

        if (i == -1)
        {
          printf(">> In file: %s\n", current_filename);
          printf("Error:  Undefined pattern '%s' on line %d.  Ignoring.\n", token,line);
        }
          else
        {
          for (x = 0; x < repeat; x++) { sections[sections_ptr++] = i; }
        }

        tokentype = gettoken(in, token);

        if (strcmp(token, ";") == 0)
        {
          break;
        }
          else
        if (strcmp(token, ",") != 0)
        {
          error(",", token);
          return -1;
        }
      }
    }
  }

  sections[sections_ptr++] = -1;

  return 0;
}

void play_pattern(int i)
{
  int ptr;
  unsigned char midi_data[256];
  struct note_t note;
  int k;
  unsigned int r;
#ifdef WINDOWS
  unsigned int n;
#endif

  ptr = find_pattern(i);

  if (interactive == 1)
  {
    printf("Pattern: ");
    print_name((char *)pattern_names, i);
    fflush(out);
  }

#ifdef DEBUG
printf("  Playing pattern %d\n",i);
printf("[ ");
#endif

  k = 0;

  while(1)
  {
    if (pattern[ptr] == 255) break;

#ifdef DEBUG
printf("%x %x %x, ", 0x90 + pattern_channel[ptr], pattern[ptr], pattern_volume[ptr]);
#endif

    if (out == 0)
    {
      midi_data[k++] = 0x90 + pattern_channel[ptr];
      midi_data[k++] = pattern[ptr];
      midi_data[k++] = pattern_volume[ptr];

      if (pattern_duration[ptr] != 0)
      {
#ifndef WINDOWS
        if (pattern_volume[ptr] != 0) { write(midiout, midi_data, k); }
#else
        for (r = 0; r < k; r = r + 3)
        {
          n = midi_data[r] + (midi_data[r + 1] << 8) + (midi_data[r + 2] << 16);
          midiOutShortMsg(inHandle, n);
        }
#endif

#ifdef DEBUG
printf("k=%d\n", k);
printf("usleep(%d) ", pattern_duration[ptr]);
#endif

#ifndef WINDOWS
        if (mr_timer.it_value.tv_sec == 0 && mr_timer.it_value.tv_usec == 0)
        {
          usleep(pattern_duration[ptr]);
        }
          else
        {
          getitimer(ITIMER_PROF, &mr_timer);

          if (mr_timer.it_value.tv_usec != 0)
          {
            r = mr_timer.it_value.tv_usec;
          }
            else
          {
            r = 0;
          }

          if (r > pattern_duration[ptr]) { r = pattern_duration[ptr]; }
#ifdef DEBUG
printf("currtime %d %d\n",mr_timer.it_value.tv_sec,mr_timer.it_value.tv_usec);
printf("interval %d %d\n",mr_timer.it_interval.tv_sec,mr_timer.it_interval.tv_usec);
printf("%d %d\n",pattern_duration[ptr], r);
#endif
          usleep(pattern_duration[ptr]-
                 (((20 - mr_timer.it_value.tv_sec) * 1000000) + r));
                 /* (1000000 - mr_timer.it_value.tv_usec))); */
        }

        mr_timer.it_value.tv_sec = 20;
        mr_timer.it_value.tv_usec = 0;

        /* setitimer(ITIMER_REAL,&mr_timer, 0); */
        setitimer(ITIMER_PROF,&mr_timer, 0);
#else
        if (mr_timer == 0)
        {
          Sleep(pattern_duration[ptr] / 1000);
        }
          else
        {
          Sleep((pattern_duration[ptr] / 1000) - (timeGetTime() - mr_timer));
        }

        mr_timer = timeGetTime();
#endif
        k = 0;
      }
    }
      else
    {
      note.instrument = pattern[ptr];
      note.volume = pattern_volume[ptr];
      note.duration = pattern_duration[ptr];
      note.midi_channel = pattern_channel[ptr];
      write_midi_note(out, &note);
    }

    ptr++;
  }

#ifdef DEBUG
printf(" ]\n");
#endif

}

void play_section(int i)
{
  int ptr;

  ptr = find_section(i);

  if (interactive == 1)
  {
    printf("Section: ");
    print_name((char *)section_names, i);
  }

#ifdef DEBUG
printf("playing section: %d\n",i);
#endif

  while(1)
  {
    if (sections[ptr] == -1) { break; }
    play_pattern(sections[ptr]);
    ptr++;
  }
}

int parse_song(FILE *in)
{
  int tokentype;
  char token[1024];
  int repeat;
  int i, x;

#ifdef DEBUG
printf("playing song\n");
#endif

  tokentype = gettoken(in, token);

  if (strcmp(token, "{") != 0)
  {
    if (tokentype == 1)
    {
      song_name[255] = 0;
      strncpy((char *)song_name, token, 254);

      tokentype = gettoken(in, token);

      if (strcmp(token, "{") != 0)
      {
        error("{", token);
        return -1;
      }
    }
      else
    {
      printf(">> In file: %s\n", current_filename);
      printf("Error: Expecting song name or '{' but got '%s' on line %d.\n",
        token, line);
      return -1;
    }
  }

  if (out != 0)
  {
    write_midi_header(out);
    write_midi_bpm(out);
  }

  while(1)
  {
    tokentype = gettoken(in, token);

    if (strcmp(token, "}") == 0)
    {
      break;
    }

    if (strcmp(token, "play") == 0)
    {
      tokentype = gettoken(in, token);

      if (strcmp(token, ":") != 0)
      {
        error(":", token);
        return -1;
      }

      while(1)
      {
        repeat = 1;

        tokentype = gettoken(in, token);
        if (tokentype==2)
        {
          repeat = atoi(token);
          tokentype = gettoken(in, token);
        }

        i = get_literal((char *)section_names, token);

        if (i != -1)
        {
          for (x = 0; x < repeat; x++) { play_section(i); }
        }
          else
        {
          i = get_literal((char *)pattern_names, token);

          if (i == -1)
          {
            printf(">> In file: %s\n", current_filename);
            printf("Error:  Undefined pattern '%s' on line %d.  Ignoring.\n", token, line);
          }
            else
          {
            for (x = 0; x < repeat; x++) { play_pattern(i); }
          }
        }

        tokentype = gettoken(in, token);

        if (strcmp(token, ";") == 0)
        {
          break;
        }
          else
        if (strcmp(token, ",") != 0)
        {
          error(",", token);
          return -1;
        }
      }
    }
  }

  if (out != 0)
  {
    write_midi_footer(out);
  }

  return 0;
}

int main_parser(FILE *in)
{
  int tokentype, i;
  char token[1024];
#ifdef DEBUG
  int t = 0;
#endif

#ifndef WINDOWS
  signal_catch();
  mr_timer.it_value.tv_sec = 0;
  mr_timer.it_value.tv_usec = 0;
#endif

  while(1)
  {
    tokentype = gettoken(in, token);
    if (tokentype == -1) { break; }

    if (strcmp(token, "set") == 0)
    {
      i = parse_set(in);
      if (i == -1) { return -1; }
    }
      else
    if (strcmp(token, "define") == 0)
    {
      i = parse_define(in);
      if (i == -1) { return -1; }
    }
      else
    if (strcmp(token, "include") == 0)
    {
      i = parse_include(in);
      if (i == -1) { return -1; }
    }
      else
    if (strcmp(token, "pattern") == 0)
    {
      i = parse_pattern(in);
      if (i == -1) { return -1; }
    }
      else
    if (strcmp(token, "section") == 0)
    {
      i = parse_section(in);
      if (i == -1) { return -1; }
    }
      else
    if (strcmp(token, "song") == 0)
    {
      i = parse_song(in);
      if (i == -1) { return -1; }
    }
      else
    {
      printf(">> In file: %s\n", current_filename);
      printf("Error: Unknown token '%s' on line %d.\n", token, line);
      return -1;
    }
  }

#ifdef DEBUG
printf("Defines:\n");
print_all(defines);
printf("Patterns:\n");
print_all(pattern_names);
printf("Sections:\n");
print_all(section_names);

  i = 0;
  t = 0;

  while(1)
  {
    if (i == sections_ptr) { break; }

    if (sections[i++] == -1)
    {
      printf("\n");
      t++;
      continue;
    }

    printf("%d ", sections[i - 1]);
  }

printf("song_name=%s\n", song_name);
#endif

#ifdef WINDOWS
  Sleep(1000);
#endif

  return 0;
}

