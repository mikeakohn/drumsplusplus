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
#include "Tokens.h"
#include "MidiFile.h"
#include "MidiPlayer.h"
#include "Note.h"
#include "Song.h"
#include "Utility.h"

Song::Song() : interactive(0)
{
}

Song::~Song()
{
}

int Song::parse(Tokens *tokens, MidiFile *midi_file)
{
  int token_type;
  char token[1024];
#ifdef DEBUG
  int t = 0;
#endif

#ifndef WINDOWS
  signal_catch(0);
  play_timer.it_value.tv_sec = 0;
  play_timer.it_value.tv_usec = 0;
#endif

  while(1)
  {
    token_type = tokens->get(token);

    if (token_type == -1) { break; }

    if (strcmp(token, "set") == 0)
    {
      if (parse_set(tokens) == -1) { return -1; }
    }
      else
    if (strcmp(token, "define") == 0)
    {
      if (parse_define(tokens) == -1) { return -1; }
    }
      else
    if (strcmp(token, "include") == 0)
    {
      if (parse_include(tokens, midi_file) == -1) { return -1; }
    }
      else
    if (strcmp(token, "pattern") == 0)
    {
      if (parse_pattern(tokens) == -1) { return -1; }
    }
      else
    if (strcmp(token, "section") == 0)
    {
      if (parse_section(tokens) == -1) { return -1; }
    }
      else
    if (strcmp(token, "song") == 0)
    {
      if (parse_song(tokens, midi_file) == -1) { return -1; }
    }
      else
    {
      printf(">> In file: %s\n", tokens->get_filename());
      printf("Error: Unknown token '%s' on line %d.\n",
        token, tokens->get_line());
      return -1;
    }
  }

#ifdef WINDOWS
  Sleep(1000);
#endif

  return 0;
}

void Song::print_song()
{
  printf("Defines:\n");

  for (auto it = defines.begin(); it != defines.end(); it++)
  {
    printf("  %s = %s\n", it->first.c_str(), it->second.c_str());
  }

  printf("Patterns:\n");

  const int count = pattern_names.size();

  for (int n; n < count; n++)
  {
    std::string &pattern_name = pattern_names[n];

    patterns[pattern_name].print();
  }

  printf("Sections:\n");

  for (auto it = sections.begin(); it != sections.end(); it++)
  {
    printf("  name: %s\n", it->first.c_str());
    it->second.print();
  }

  printf("song_name=%s\n", song_info.get_song_name());
}

int Song::parse_set(Tokens *tokens)
{
  int token_type;
  char token[1024];
  char equals[1024];
  char value[1024];

  token_type = tokens->get(token);

  if (token_type != TOKEN_ALPHA)
  {
    print_error(tokens, "setting name", token);
    return -1;
  }

  token_type = tokens->get(equals);

  if (strcmp(equals, "=") != 0)
  {
    print_error(tokens, "=", equals);
    return -1;
  }

  token_type = tokens->get(value);

  if (token_type != TOKEN_NUMBER)
  {
    print_error(tokens, "number",equals);
    return -1;
  }

  if (strcmp(token, "bpm") == 0)
  {
    song_info.bpm = atoi(value);

    if (song_info.bpm == 0)
    {
      printf(">> In file: %s\n", tokens->get_filename());
      printf("Error: Tempo (bpm) must be non-zero; line %d\n",
        tokens->get_line());
      return -1;
    }
  }
    else
  if (strcmp(token, "default_volume") == 0 ||
      strcmp(token, "defaultvolume") == 0)
  {
    song_info.default_volume = atoi(value);

    if (song_info.default_volume > 127)
    {
      printf("Warning: default volume is higher than the max 127; line %d\n",
        tokens->get_line());
      song_info.default_volume = 127;
    }
  }
    else
  if (strcmp(token, "drift") == 0)
  {
     song_info.drift = atoi(value);
  }
    else
  if (strcmp(token, "time_signature") == 0 ||
      strcmp(token, "timesignature") == 0)
  {
    song_info.time_signature_beats = atoi(value);

    token_type = tokens->get(equals);

    if (strcmp(equals, "/") != 0)
    {
      print_error(tokens, "/", equals);
      return -1;
    }

    token_type = tokens->get(value);

    if (token_type != TOKEN_NUMBER)
    {
      print_error(tokens, "number", value);
      return -1;
    }

    song_info.time_signature_base = atoi(value);
    /* midi_file->write_time_signature(); */

    if (song_info.time_signature_beats == 0)
    {
      printf(">> In file: %s\n", tokens->get_filename());
      printf("Error: Time signature numerator must be non-zero; line %d\n",
        tokens->get_line());
      return -1;
    }
      else
    if (song_info.time_signature_base == 0)
    {
      printf(">> In file: %s\n", tokens->get_filename());
      printf("Error: Timesignature denominator must be non-zero; line %d\n",
        tokens->get_line());
      return -1;
    }
  }
    else
  if (strcmp(token, "midi_channel") == 0)
  {
    song_info.midi_channel = atoi(value);

    if (song_info.midi_channel > 15)
    {
      printf("Warning: MIDI channel %d is higher than the maximum 15; line %d\n", song_info.midi_channel, tokens->get_line());
      song_info.midi_channel = 9;
    }
  }
    else
  {
    printf(">> In file: %s\n", tokens->get_filename());
    printf("Error: Unknown setting '%s' on line %d.\n",
      token, tokens->get_line());
    return -1;
  }

  token_type = tokens->get(equals);

  if (strcmp(equals, ";") != 0)
  {
    print_error(tokens, ";", equals);
    return -1;
  }

  return 0;
}

int Song::parse_define(Tokens *tokens)
{
  int token_type;
  char name[1024];
  char value[1024];

  token_type = tokens->get(name);

  if (token_type != TOKEN_ALPHA)
  {
    print_error(tokens, "define", name);
    return -1;
  }

  token_type = tokens->get(value);

  if (defines.find(name) != defines.end())
  {
    print_error(tokens, "already defined", name);
  }

  defines[name] = value;

  return 0;
}

int Song::parse_include(Tokens *tokens, MidiFile *midi_file)
{
  int token_type;
  char token[1024];
  char filename[1024];
  char lastfilename[1024];
  Tokens *tokens_include;

  tokens_include = new Tokens();

  token_type = tokens->get(token);

  if (token_type != TOKEN_STRING)
  {
    print_error(tokens, "filename", token);
    return -1;
  }

  filename[0] = 0;

  if (token[0] != '/')
  {
    strcpy(lastfilename, tokens->get_filename());

    if (dirname_m(lastfilename) != 0)
    {
      strcpy(filename, lastfilename);
      strcat(filename, "/");
    }
  }

  strcat(filename, token);

  if (tokens_include->open(filename) != 0)
  {
    printf(">> In file: %s\n", tokens->get_filename());
    printf("Error: included file not found: %s on line %d.\n",
      filename, tokens->get_line());

    delete tokens_include;

    return -1;
  }
    else
  {
    if (parse(tokens_include, midi_file) == -1) { return -1; }
  }

  tokens_include->close();
  delete tokens_include;

  return 0;
}

int Song::add_beats(
  Tokens *tokens,
  Beat *beats,
  int &ptr,
  int i,
  int midi_channel)
{
  int token_type;
  char token[1024];
  int m, modifier, c;

  // This will read an entire line of beats from a pattern for
  // a single instrument in that pattern.

  c = 0;

  token_type = tokens->get(token);

  // Allows a different channel number to be used (another instrument
  // other than drums.
  if (strcmp(token, "/") == 0)
  {
    token_type = tokens->get(token);

    if (token_type != TOKEN_NUMBER)
    {
      print_error(tokens, "Channel number", token);
      return -1;
    }

    midi_channel = atoi(token);

    if (midi_channel > 15)
    {
      printf("Warning: MIDI channel %d is higher than the max 15; line %d\n",
        midi_channel, tokens->get_line());
      midi_channel = 9;
    }

    token_type = tokens->get(token);
  }

  if (strcmp(token, ":") != 0)
  {
    print_error(tokens, ":", token);
    return -1;
  }

  while(1)
  {
    token_type = tokens->get(token);

    if (strcmp(token, ";") == 0) return 0;

    if (strcmp(token, ":") == 0)
    {
      if (c == 0)
      {
        printf(">> In file: %s\n", tokens->get_filename());
        printf("Parse Error: Unexpected token %s on line %d.\n",
          token, tokens->get_line());
        return -1;
      }

      token_type = tokens->get(token);

      if (strcmp(token, "%") == 0)
      {
        // Allow the volume of the drum to drift.
        srand(time(NULL));
        modifier = rand() % song_info.drift;
        if ((rand() % 2) == 0) { modifier = -modifier; }
        token_type = tokens->get(token);
      }

      if (token_type != TOKEN_NUMBER)
      {
        print_error(tokens, "volume integer", token);
        return -1;
      }

      m = (int)atof(token);

      if (m > 127)
      {
        printf("Warning: Volume is higher than 127 on line %d.\n",
          tokens->get_line());
      }

      m = m + modifier;

      if (m < 0) { m = 0; }
      if (m > 127) { m = 127; }

      // FIXME: This is a bad way to do this.  Would be better to check
      // if a : exist after reading the beat values.
      beats[ptr - 1].volume = m;
      continue;
    }

    if (token_type != TOKEN_NUMBER)
    {
      print_error(tokens, "a beat number", token);
      continue;
    }

    beats[ptr].value = atof(token);
    beats[ptr].instrument = i;
    beats[ptr].channel = midi_channel;

    if (beats[ptr].value < 1)
    {
      printf(">> In file: %s\n", tokens->get_filename());
      printf("Error: Beat is less than 1 on line %d.  Ignoring.\n",
        tokens->get_line());
      beats[ptr].value = 0;
    }

    if (beats[ptr].value >= song_info.time_signature_beats + 1)
    {
      printf(">> In file: %s\n", tokens->get_filename());
      printf("Error: Beat exceeds beats per measure on line %d.  Ignoring.\n",
        tokens->get_line());
      beats[ptr].value = 0;
    }

    beats[ptr].volume = song_info.default_volume;

    ptr++;
    c++;
  }
}

int Song::parse_pattern(Tokens *tokens)
{
  int token_type;
  char token[1024], token1[1024];
  Beat beats[256];
  int ptr;
  char value[1024];
  float low_beat = 0;
  float next_beat = 0;
  int i, count, num_notes;
  int beat_time;
  int time_signature_beats;
  //int time_signature_base;
  int midi_channel;

  beat_time = 60000000 / song_info.bpm;
  time_signature_beats = song_info.time_signature_beats;
  //time_signature_base = song_info.time_signature_base;
  midi_channel = song_info.midi_channel;

  token_type = tokens->get(token);

  if (token_type != TOKEN_ALPHA)
  {
    printf(">> In file: %s\n", tokens->get_filename());
    printf("Error: Pattern is not alphanumeric on line %d\n",
      tokens->get_line());
    return -1;
  }

  if (patterns.find(token) != patterns.end())
  {
    printf(">> In file: %s\n", tokens->get_filename());
    printf("Error: Pattern %s is already defined on line %d\n",
      token, tokens->get_line());
    return -1;
  }

  Pattern &pattern = patterns[token];

  const int index = patterns.size();

  pattern_names[index] = token;

#ifdef DEBUG
printf("parsing pattern: %s\n", token);
#endif

  token_type = tokens->get(token);

  if (strcmp(token, "{") != 0)
  {
    print_error(tokens, "{", token);
    return -1;
  }

  ptr = 0;
  num_notes = 0;

  while(1)
  {
    token_type = tokens->get(token);

    if (strcmp(token, "}") == 0) { break;}

    // Check if there are some modifications to the global song values.
    if (strcmp(token, "set") == 0)
    {
      token_type = tokens->get(token);

      if (token_type != TOKEN_ALPHA)
      {
        print_error(tokens, "variable to set", token);
        return -1;
      }

      token_type = tokens->get(token1);

      if (strcmp(token1, "=") != 0)
      {
        print_error(tokens, "=", token1);
        return -1;
      }

      token_type = tokens->get(token1);

      if (token_type != TOKEN_NUMBER)
      {
        print_error(tokens, "a number", token1);
        return -1;
      }

      i = atoi(token1);

      if (i == 0)
      {
        print_error(tokens, "a non-zero integer", token1);
        return -1;
      }

      if (strcmp(token, "bpm") == 0)
      {
        beat_time = 60000000 / i;
      }
        else
      if (strcmp(token, "timesignature") == 0)
      {
        time_signature_beats = i;

        token_type = tokens->get(token1);

        if (strcmp(token1, "/") != 0)
        {
          print_error(tokens, "/", token1);
          return -1;
        }

        token_type = tokens->get(token1);

        if (token_type != TOKEN_NUMBER)
        {
          print_error(tokens, "a number", token1);
          return -1;
        }

        i = atoi(token1);

        if (i == 0)
        {
          print_error(tokens, "a non-zero integer", token1);
          return -1;
        }

        //time_signature_base = i;
      }
        else
      if (strcmp(token, "midi_channel") == 0)
      {
        midi_channel = i;

        if (midi_channel > 15)
        {
          printf("Warning: MIDI channel %d is higher than max 15; line %d\n",
            midi_channel, tokens->get_line());
          midi_channel = 9;
        }
      }
        else
      {
        printf("Warning: Cannot set '%s' in pattern.\n", token);
      }

      token_type = tokens->get(token);

      if (strcmp(token, ";") != 0)
      {
        print_error(tokens, ";", token);
        return -1;
      }

      continue;
    }

    // Read next line of beat information for an instrument from a pattern.
    if (token_type == TOKEN_NUMBER)
    {
      beats[ptr].channel = midi_channel;

      if (add_beats(tokens, beats, ptr, atoi(token), midi_channel) == -1)
      {
        return -1;
      }
    }
      else
    {
      if (defines.find(token) == defines.end())
      {
        printf(">> In file: %s\n", tokens->get_filename());
        printf("Error: %s is undefined on line %d\n",
          token, tokens->get_line());
        return -1;
      }

      strcpy(value, defines[token].c_str());

      if (is_number(value) == 0)
      {
        printf(">> In file: %s\n", tokens->get_filename());
        printf("Error: %s is not a number on line %d\n",
          token, tokens->get_line());
        return -1;
      }

      beats[ptr].channel = midi_channel;

      if (add_beats(tokens, beats, ptr, atoi(value), midi_channel) == -1)
      {
        return -1;
      }
    }
  }

#ifdef DEBUG
for (int n = 0; n < ptr; n++)
{
  printf("beat: %d %d %f %d\n",
    beats[n].instrument,
    beats[n].channel,
    beats[n].value,
    beats[n].volume);
}
#endif

  // Convert beat information to the pattern format.  This looks like
  // basically a slow sorting algorithm.
  while(1)
  {
    low_beat = time_signature_beats + 1;
    next_beat = time_signature_beats + 1;
    count = 0;

    // Find the next lowest beat value and a count of how many
    // times that beat value exists.
    for (i = 0; i < ptr; i++)
    {
      if (beats[i].value > 0)
      {
        if (beats[i].value < low_beat)
        {
          low_beat = beats[i].value;
          count = 0;
        }

        if (beats[i].value == low_beat) { count++; }
      }
    }

    // If no more beats were found, break from adding to the pattern list.
    if (count == 0)
    {
      if (ptr == 0)
      {
        // If this is the first beat, add an empty pattern?
        int duration = (int)((time_signature_beats) * beat_time);

#if 0
        pattern[patterns_ptr] = 0;
        pattern_duration[patterns_ptr] = duration;
        pattern_volume[patterns_ptr] = 0;
        pattern_channel[patterns_ptr] = midi_channel;
        patterns_ptr++;
#endif
        pattern.add(0, 0, midi_channel, duration);
      }

      break;
    }

    // Find the next lowest value.  It seems like this whole process
    // could be done without doing this.
    for (i = 0; i < ptr; i++)
    {
      if (beats[i].value > 0)
      {
        if (beats[i].value < next_beat && beats[i].value != low_beat)
        {
          next_beat = beats[i].value;
        }
      }
    }

    // Scan through all the beats, and if it is equal to the current
    // lowest, add it to the pattern.  Then clear it out so it doesn't
    // get counted again.
    for (i = 0; i < ptr; i++)
    {
      if (beats[i].value == low_beat)
      {
        if (num_notes == 0 && low_beat != 1)
        {
          int duration = (int)((low_beat - 1) * beat_time);

#if 0
          pattern[patterns_ptr] = 0;
          pattern_duration[patterns_ptr] = duration;
          pattern_volume[patterns_ptr] = 0;
          pattern_channel[patterns_ptr] = midi_channel;
          patterns_ptr++;
#endif
          pattern.add(0, 0, midi_channel, duration);
        }

        int duration = 0;


        if (count == 1)
        {
          duration = (int)((next_beat - low_beat) * beat_time);

          //pattern_duration[patterns_ptr] = duration;
        }

#if 0
        pattern[patterns_ptr] = beats[i].instrument;
        pattern_volume[patterns_ptr] = beats[i].volume;
        pattern_channel[patterns_ptr] = beats[i].channel;
        patterns_ptr++;
#endif
        pattern.add(beats[i].instrument,
                    beats[i].volume,
                    beats[i].channel,
                    duration);

        beats[i].value = 0;

        count--;
      }
    }

    num_notes++;
  }

  //pattern[patterns_ptr++] = 255;

  return 0;
}

int Song::parse_section(Tokens *tokens)
{
  std::string section_name;
  Section section;
  int token_type;
  char token[1024];
  int repeat, i;

  token_type = tokens->get(token);

  if (token_type != TOKEN_ALPHA)
  {
    printf(">> In file: %s\n", tokens->get_filename());
    printf("Error: Section is not alphanumeric on line %d\n",
      tokens->get_line());
    return -1;
  }

#ifdef DEBUG
printf("parsing section: %s\n", token);
#endif

  section_name = token;

  token_type = tokens->get(token);

  if (strcmp(token, "{") != 0)
  {
    print_error(tokens, "{", token);
    return -1;
  }

  while(1)
  {
    token_type = tokens->get(token);

    if (strcmp(token, "}") == 0)
    {
      break;
    }

    if (strcmp(token, "play") == 0)
    {
      token_type = tokens->get(token);

      if (strcmp(token, ":") != 0)
      {
        print_error(tokens, ":", token);
        return -1;
      }

      while(1)
      {
        repeat = 1;
        token_type = tokens->get(token);

        if (token_type == TOKEN_NUMBER)
        {
          repeat = atoi(token);
          token_type = tokens->get(token);
        }

        if (patterns.find(token) == patterns.end())
        {
          printf(">> In file: %s\n", tokens->get_filename());
          printf("Error:  Undefined pattern '%s' on line %d.  Ignoring.\n",
            token, tokens->get_line());
          return -1;
        }

        i = patterns[token].get_index();

        section.add_pattern(i, repeat);

        token_type = tokens->get(token);

        if (strcmp(token, ";") == 0)
        {
          break;
        }
          else
        if (strcmp(token, ",") != 0)
        {
          print_error(tokens, ",", token);
          return -1;
        }
      }
    }
  }

  sections[section_name] = section;

  return 0;
}

int Song::parse_song(Tokens *tokens, MidiFile *midi_file)
{
  int token_type;
  char token[1024];
  int repeat;
  int x;

  token_type = tokens->get(token);

  if (strcmp(token, "{") != 0)
  {
    if (token_type == TOKEN_ALPHA)
    {
      song_info.set_song_name(token);

      token_type = tokens->get(token);

      if (strcmp(token, "{") != 0)
      {
        print_error(tokens, "{", token);
        return -1;
      }
    }
      else
    {
      printf(">> In file: %s\n", tokens->get_filename());
      printf("Error: Expecting song name or '{' but got '%s' on line %d.\n",
        token, tokens->get_line());
      return -1;
    }
  }

  midi_file->write_header(&song_info);
  midi_file->write_bpm(&song_info);

  while(1)
  {
    token_type = tokens->get(token);

    if (strcmp(token, "}") == 0)
    {
      break;
    }

    if (strcmp(token, "play") == 0)
    {
      token_type = tokens->get(token);

      if (strcmp(token, ":") != 0)
      {
        print_error(tokens, ":", token);
        return -1;
      }

      while(1)
      {
        repeat = 1;

        token_type = tokens->get(token);

        if (token_type == TOKEN_NUMBER)
        {
          repeat = atoi(token);
          token_type = tokens->get(token);
        }

        std::string section_name = token;

        if (sections.find(section_name) != sections.end())
        {
          for (x = 0; x < repeat; x++)
          {
            play_section(midi_file, section_name);
          }
        }
          else
        {
          if (patterns.find(token) == patterns.end())
          {
            printf(">> In file: %s\n", tokens->get_filename());
            printf("Error:  Undefined pattern '%s' on line %d.  Ignoring.\n",
              token, tokens->get_line());
          }

          std::string pattern_name = token;

          for (x = 0; x < repeat; x++)
          {
            play_pattern(midi_file, pattern_name);
          }
        }

        token_type = tokens->get(token);

        if (strcmp(token, ";") == 0)
        {
          break;
        }
          else
        if (strcmp(token, ",") != 0)
        {
          print_error(tokens, ",", token);
          return -1;
        }
      }
    }
  }

  midi_file->write_footer();

  return 0;
}

int Song::play_section(MidiFile *midi_file, std::string &section_name)
{
  int ptr;

  if (sections.find(section_name) == sections.end())
  {
    printf("Could not find section %s\n", section_name.c_str());
    return -1;
  }

  Section &section = sections[section_name];

  if (interactive == 1)
  {
    printf("Section: %s\n", section_name.c_str());
  }

#ifdef DEBUG
printf("playing section: %s\n", section_name.c_str());
#endif

  std::vector<int> &patterns = section.get_patterns();

  for (auto it = patterns.begin(); it != patterns.end(); it++)
  {
    std::string &pattern_name = pattern_names[*it];

    play_pattern(midi_file, pattern_name);
    ptr++;
  }

  return 0;
}

void Song::play_pattern(MidiFile *midi_file, std::string &pattern_name)
{
  int index, count;
  uint8_t midi_data[256];
  Note note;
  uint32_t k;
  uint32_t r;
#ifdef WINDOWS
  int n;
#endif

  Pattern &pattern = patterns[pattern_name];

  if (interactive == 1)
  {
    printf("Pattern: %s", pattern_name.c_str());
  }

#ifdef DEBUG
printf("  Playing pattern %d\n",i);
printf("[ ");
#endif

  k = 0;
  count = pattern.get_count();

  for (index = 0; index < count; index++)
  {
    Pattern::Data &data = pattern.get_data(index);

#ifdef DEBUG
printf("%x %x %x, ", 0x90 + pattern_channel[ptr], pattern[ptr], pattern_volume[ptr]);
#endif

    if (!midi_file->is_open())
    {
      midi_data[k++] = 0x90 + data.channel;
      midi_data[k++] = data.value;
      midi_data[k++] = data.volume;

      if (data.duration != 0)
      {
        // FIXME: Enable this later.
        //midi_player->play(midi_data, k);

        for (r = 0; r < k; r++)
        {
          printf(" %02x\n", midi_data[r]);
        }

#ifdef DEBUG
printf("usleep(%d) ", data.duration);
#endif

#ifndef WINDOWS
        if (play_timer.it_value.tv_sec == 0 && play_timer.it_value.tv_usec == 0)
        {
          usleep(data.duration);
        }
          else
        {
          getitimer(ITIMER_PROF, &play_timer);

          if (play_timer.it_value.tv_usec != 0)
          {
            r = play_timer.it_value.tv_usec;
          }
            else
          {
            r = 0;
          }

          if (r > data.duration) { r = data.duration; }
#ifdef DEBUG
printf("currtime %d %d\n", play_timer.it_value.tv_sec, play_timer.it_value.tv_usec);
printf("interval %d %d\n", play_timer.it_interval.tv_sec, play_timer.it_interval.tv_usec);
printf("%d %d\n", data.duration, r);
#endif
          usleep(data.duration -
                 (((20 - play_timer.it_value.tv_sec) * 1000000) + r));
        }

        play_timer.it_value.tv_sec = 20;
        play_timer.it_value.tv_usec = 0;

        /* setitimer(ITIMER_REAL,&play_timer, 0); */
        setitimer(ITIMER_PROF,&play_timer, 0);
#else
        if (play_timer == 0)
        {
          Sleep(data.duration / 1000);
        }
          else
        {
          Sleep((data.duration / 1000) - (timeGetTime() - play_timer));
        }

        play_timer = timeGetTime();
#endif
        k = 0;
      }
    }
      else
    {
      note.value = data.value;
      note.volume = data.volume;
      note.duration = data.duration;
      note.midi_channel = data.channel;

      midi_file->write_note(&song_info, &note);
    }

    //ptr++;
  }

#ifdef DEBUG
printf(" ]\n");
#endif
}

char *Song::dirname_m(char *dir)
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

void Song::print_error(Tokens *tokens, const char *expect, const char *got)
{
  printf(">> In file: %s\n", tokens->get_filename());
  printf("Parse Error:  Expected '%s' and got '%s' on line %d.\n",
    expect, got, tokens->get_line());
}

void Song::signal_catch(int sig)
{
  signal(SIGPROF, signal_catch);
}

