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

#include "MidiFile.h"
#include "Note.h"
#include "NoteOff.h"
#include "Song.h"
#include "Tokens.h"
#include "Utility.h"

Song::Song() : interactive { false }
{
}

Song::~Song()
{
}

void Song::set_midi(MidiFile *midi_file)
{
  this->midi_file = midi_file;
}

int Song::parse(Tokens &tokens)
{
  int token_type;
  char token[TOKEN_LEN];

  while (true)
  {
    token_type = tokens.get(token);

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
      if (parse_include(tokens) == -1) { return -1; }
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
    if (strcmp(token, "phrase") == 0)
    {
      if (parse_phrase(tokens) == -1) { return -1; }
    }
      else
    if (strcmp(token, "song") == 0)
    {
      if (parse_song(tokens) == -1) { return -1; }
    }
      else
    {
      printf("Error: Unknown token '%s' at %s:%d.\n",
        token, tokens.get_filename(), tokens.get_line());
      return -1;
    }
  }

#ifdef WINDOWS
  Sleep(1000);
#endif

  return 0;
}

void Song::print()
{
  printf("Defines:\n");

  for (auto it : defines)
  {
    printf("  %s = %s\n", it.first.c_str(), it.second.c_str());
  }

  printf("Patterns:\n");

  for (auto it : pattern_names)
  {
    printf("  %d) %s\n", it.first, it.second.c_str());
  }

  const int pattern_count = pattern_names.size();

  for (int n = 0; n < pattern_count; n++)
  {
    std::string &pattern_name = pattern_names[n];

    patterns[pattern_name].print();
  }

  printf("Sections:\n");

  for (auto it : sections)
  {
    printf("  name: %s\n", it.first.c_str());
    it.second.print();
  }

  printf("Phrases:\n");

  for (auto it : phrase_names)
  {
    printf("  %d) %s\n", it.first, it.second.c_str());
  }

  const int phrase_count = phrase_names.size();

  for (int n = 0; n < phrase_count; n++)
  {
    std::string &phrase_name = phrase_names[n];

    phrases[phrase_name].print();
  }

  printf("song_name=%s\n", song_info.get_song_name());
}

int Song::parse_set(Tokens &tokens)
{
  int token_type;
  char token[TOKEN_LEN];
  char equals[TOKEN_LEN];
  char value[TOKEN_LEN];

  token_type = tokens.get(token);

  if (token_type != TOKEN_ALPHA)
  {
    print_error(tokens, "setting name", token);
    return -1;
  }

  token_type = tokens.get(equals);

  if (strcmp(equals, "=") != 0)
  {
    print_error(tokens, "=", equals);
    return -1;
  }

  token_type = tokens.get(value);

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
      printf("Error: Tempo (bpm) must be non-zero at %s:%d\n",
        tokens.get_filename(), tokens.get_line());
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
        tokens.get_line());
      song_info.default_volume = 127;
    }
  }
    else
  if (strcmp(token, "drift") == 0)
  {
     song_info.drift = atoi(value);
  }
    else
  if (strcmp(token, "time_signature") == 0)
  {
    song_info.time_signature_beats = atoi(value);

    token_type = tokens.get(equals);

    if (strcmp(equals, "/") != 0)
    {
      print_error(tokens, "/", equals);
      return -1;
    }

    token_type = tokens.get(value);

    if (token_type != TOKEN_NUMBER)
    {
      print_error(tokens, "number", value);
      return -1;
    }

    song_info.time_signature_base = atoi(value);
    /* midi_file->write_time_signature(); */

    if (song_info.time_signature_beats == 0)
    {
      printf("Error: Time signature numerator must be non-zero at %s:%d\n",
        tokens.get_filename(), tokens.get_line());
      return -1;
    }
      else
    if (song_info.time_signature_base == 0)
    {
      printf("Error: Timesignature denominator must be non-zero at %s:%d\n",
        tokens.get_filename(), tokens.get_line());
      return -1;
    }
  }
    else
  if (strcmp(token, "midi_channel") == 0)
  {
    song_info.midi_channel = atoi(value);

    if (song_info.midi_channel > 15)
    {
      printf("Warning: MIDI channel %d is higher than the maximum 15; line %d\n", song_info.midi_channel, tokens.get_line());
      song_info.midi_channel = 9;
    }
  }
    else
  {
    printf("Error: Unknown setting '%s' at %s:%d.\n",
      token, tokens.get_filename(), tokens.get_line());
    return -1;
  }

  token_type = tokens.get(equals);

  if (strcmp(equals, ";") != 0)
  {
    print_error(tokens, ";", equals);
    return -1;
  }

  return 0;
}

int Song::parse_define(Tokens &tokens)
{
  int token_type;
  char name[TOKEN_LEN];
  char value[TOKEN_LEN];

  token_type = tokens.get(name);

  if (token_type != TOKEN_ALPHA)
  {
    print_error(tokens, "define", name);
    return -1;
  }

  token_type = tokens.get(value);

  if (defines.find(name) != defines.end())
  {
    print_error(tokens, "already defined", name);
  }

  defines[name] = value;

  return 0;
}

int Song::parse_include(Tokens &tokens)
{
  int token_type;
  char token[TOKEN_LEN];
  char filename[1024];
  char lastfilename[1024];
  Tokens tokens_include;

  token_type = tokens.get(token);

  if (token_type != TOKEN_STRING)
  {
    print_error(tokens, "filename", token);
    return -1;
  }

  filename[0] = 0;

  if (token[0] != '/')
  {
    strcpy(lastfilename, tokens.get_filename());

    if (dirname_m(lastfilename) != 0)
    {
      strcpy(filename, lastfilename);
      strcat(filename, "/");
    }
  }

  strcat(filename, token);

  if (tokens_include.open(filename) != 0)
  {
    printf("Error: included file not found: %s at %s:%d.\n",
      filename, tokens.get_filename(), tokens.get_line());

    return -1;
  }
    else
  {
    if (parse(tokens_include) == -1) { return -1; }
  }

  tokens_include.close();

  return 0;
}

int Song::add_beats(
  Tokens &tokens,
  Beats &beats,
  int instrument,
  int midi_channel)
{
  int token_type;
  char token[TOKEN_LEN];
  int m, modifier;

  // This will read an entire line of beats from a pattern for
  // a single instrument in that pattern.

  modifier = 0;

  token_type = tokens.get(token);

  // Allows a different channel number to be used (another instrument
  // other than drums.
  if (strcmp(token, "/") == 0)
  {
    token_type = tokens.get(token);

    if (token_type != TOKEN_NUMBER)
    {
      print_error(tokens, "Channel number", token);
      return -1;
    }

    midi_channel = atoi(token);

    if (midi_channel > 15)
    {
      printf("Warning: MIDI channel %d is higher than the max 15; line %d\n",
        midi_channel, tokens.get_line());
      midi_channel = 9;
    }

    token_type = tokens.get(token);
  }

  if (strcmp(token, ":") != 0)
  {
    print_error(tokens, ":", token);
    return -1;
  }

  while (true)
  {
    token_type = tokens.get(token);

    if (strcmp(token, ";") == 0) { return 0; }

    if (token_type != TOKEN_NUMBER)
    {
      print_error(tokens, "a beat number", token);
      continue;
    }

    Beats::Beat beat;

    beat.value      = atof(token);
    beat.instrument = instrument;
    beat.channel    = midi_channel;

    if (beat.value < 1)
    {
      printf("Error: Beat is less than 1 at %s:%d.  Ignoring.\n",
        tokens.get_filename(), tokens.get_line());
      continue;
    }

    if (beat.value >= song_info.time_signature_beats + 1)
    {
      printf("Error: Beat exceeds beats per measure at %s:%d.  Ignoring.\n",
        tokens.get_filename(), tokens.get_line());
      continue;
    }

    beat.volume = song_info.default_volume;

    token_type = tokens.get(token);

    if (strcmp(token, ":") == 0)
    {
      token_type = tokens.get(token);

      if (strcmp(token, "%") == 0)
      {
        // Allow the volume of the drum to drift.
        srand(time(NULL));
        modifier = rand() % song_info.drift;
        if ((rand() % 2) == 0) { modifier = -modifier; }
        token_type = tokens.get(token);
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
          tokens.get_line());
      }

      m = m + modifier;

      if (m < 0)   { m = 0;   }
      if (m > 127) { m = 127; }

      beat.volume = m;
    }
      else
    {
      tokens.push(token, token_type);
    }

    beats.add(beat);
  }
}

int Song::parse_pattern(Tokens &tokens)
{
  int token_type;
  char token[TOKEN_LEN], token1[TOKEN_LEN];
  char value[TOKEN_LEN];
  Beats beats;
  int i, count;
  int time_signature_beats;
  int midi_channel;

  int beat_time = 60000000 / song_info.bpm;

  time_signature_beats = song_info.time_signature_beats;
  midi_channel = song_info.midi_channel;

  token_type = tokens.get(token);

  if (token_type != TOKEN_ALPHA)
  {
    printf("Error: Pattern is not alphanumeric at %s:%d\n",
      tokens.get_filename(), tokens.get_line());
    return -1;
  }

  if (patterns.find(token) != patterns.end())
  {
    printf("Error: Pattern %s is already defined at %s:%d\n",
      token, tokens.get_filename(), tokens.get_line());
    return -1;
  }

  std::string pattern_name = token;

  const int index = patterns.size();
  Pattern &pattern = patterns[pattern_name];

  pattern_names[index] = token;

  pattern.set_name(pattern_name);
  pattern.set_index(index);

  token_type = tokens.get(token);

  if (strcmp(token, "{") != 0)
  {
    print_error(tokens, "{", token);
    return -1;
  }

  while (true)
  {
    token_type = tokens.get(token);

    if (strcmp(token, "}") == 0) { break;}

    // Check if there are some modifications to the global song values.
    if (strcmp(token, "set") == 0)
    {
      token_type = tokens.get(token);

      if (token_type != TOKEN_ALPHA)
      {
        print_error(tokens, "variable to set", token);
        return -1;
      }

      token_type = tokens.get(token1);

      if (strcmp(token1, "=") != 0)
      {
        print_error(tokens, "=", token1);
        return -1;
      }

      token_type = tokens.get(token1);

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
      if (strcmp(token, "time_signature") == 0)
      {
        time_signature_beats = i;

        token_type = tokens.get(token1);

        if (strcmp(token1, "/") != 0)
        {
          print_error(tokens, "/", token1);
          return -1;
        }

        token_type = tokens.get(token1);

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
      }
        else
      if (strcmp(token, "midi_channel") == 0)
      {
        midi_channel = i;

        if (midi_channel > 15)
        {
          printf("Warning: MIDI channel %d is higher than max 15; line %d\n",
            midi_channel, tokens.get_line());
          midi_channel = 9;
        }
      }
        else
      {
        printf("Warning: Cannot set '%s' in pattern.\n", token);
      }

      token_type = tokens.get(token);

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
      if (add_beats(tokens, beats, atoi(token), midi_channel) == -1)
      {
        return -1;
      }
    }
      else
    {
      if (defines.find(token) == defines.end())
      {
        printf("Error: %s is undefined at %s:%d\n",
          token, tokens.get_filename(), tokens.get_line());
        return -1;
      }

      strcpy(value, defines[token].c_str());

      if (Utility::is_number(value) == false)
      {
        printf("Error: %s is not a number at %s:%d\n",
          token, tokens.get_filename(), tokens.get_line());
        return -1;
      }

      if (add_beats(tokens, beats, atoi(value), midi_channel) == -1)
      {
        return -1;
      }
    }
  }

  // After reading all the beats, build the pattern based on a
  // sorted version of what was read in from the .dpp source.

  //beats.print();

  const float end_beat  = time_signature_beats + 1;

  Beats::Beat temp_beat;

  count = 0;

  for (auto &beat : beats.beats)
  {
    if (count != 0)
    {
      int duration = (int)((beat.value - temp_beat.value) * beat_time);

      pattern.add(
        temp_beat.instrument,
        temp_beat.volume,
        temp_beat.channel,
        duration);
    }

    temp_beat = beat;

    count++;
  }

  // Add the last beat to the pattern.
  if (temp_beat.value != 0.0)
  {
    int duration = (int)((end_beat - temp_beat.value) * beat_time);

    pattern.add(
      temp_beat.instrument,
      temp_beat.volume,
      temp_beat.channel,
      duration);
  }

  //pattern.print();

  return 0;
}

int Song::parse_section(Tokens &tokens)
{
  std::string section_name;
  Section section;
  int token_type;
  char token[TOKEN_LEN];
  int repeat, i;

  token_type = tokens.get(token);

  if (token_type != TOKEN_ALPHA)
  {
    printf("Error: Section is not alphanumeric on line %s:%d\n",
      tokens.get_filename(), tokens.get_line());
    return -1;
  }

#ifdef DEBUG
printf("parsing section: %s\n", token);
#endif

  section_name = token;

  token_type = tokens.get(token);

  if (strcmp(token, "{") != 0)
  {
    print_error(tokens, "{", token);
    return -1;
  }

  while (true)
  {
    token_type = tokens.get(token);

    if (strcmp(token, "}") == 0)
    {
      break;
    }

    if (strcmp(token, "play") == 0)
    {
      token_type = tokens.get(token);

      if (strcmp(token, ":") != 0)
      {
        print_error(tokens, ":", token);
        return -1;
      }

      while (true)
      {
        repeat = 1;
        token_type = tokens.get(token);

        if (token_type == TOKEN_NUMBER)
        {
          repeat = atoi(token);
          token_type = tokens.get(token);
        }

        if (patterns.find(token) == patterns.end())
        {
          printf("Error: Undefined pattern '%s' at %s:%d.\n",
            token, tokens.get_filename(), tokens.get_line());
          return -1;
        }

        i = patterns[token].get_index();

        section.add_pattern(i, repeat);

        token_type = tokens.get(token);

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

int Song::parse_voice(Tokens &tokens, Tones &tones, int i, int midi_channel)
{
  const int divisions = midi_file->get_divisions();
  float location = 0;

  char token[TOKEN_LEN];
  int token_type;

  while (true)
  {
    tokens.get(token);

    float duration;

    if (strcmp(token, "16") == 0)
    {
      // 1/4 of a quarter note.
      duration = 0.25;
    }
      else
    if (strcmp(token, "32") == 0)
    {
      // 1/8 of a quarter note.
      duration = 0.125;
    }
      else
    {
      // Check the token is only 1 char.
      if (token[1] != 0)
      {
        print_error(tokens, "Note Length", token);
        return -1;
      }

      switch (token[0])
      {
        case 'w': duration = 4;   break;
        case '1': duration = 4;   break;
        case 'h': duration = 2;   break;
        case '2': duration = 2;   break;
        case 'q': duration = 1;   break;
        case '4': duration = 1;   break;
        case 'e': duration = 0.5; break;
        case '8': duration = 0.5; break;
        default:
          print_error(tokens, "Note Length", token);
          return -1;
      }
    }

    int value = 0;
    int mod = 0;
    int octave = 0;

    tokens.get(token);

    bool is_flat = false;
    int len = strlen(token);

    if (len != 0 && token[len - 1] == 'b')
    {
      token[len - 1] = 0;
      is_flat = true;
    }

    switch (token[0])
    {
      case 'r': value =  0; break;
      case 'a': value = 21; break;
      case 'b': value = 23; break;
      case 'h': value = 23; break;
      case 'c': value = 24; break;
      case 'd': value = 26; break;
      case 'e': value = 28; break;
      case 'f': value = 29; break;
      case 'g': value = 31; break;
      default:
        print_error(tokens, "tone", token);
        return -1;
    }

    if (token[1] >= '0' && token[1] <= '9')
    {
      octave = token[1] - '0';

      if (token[2] != 0)
      {
        print_error(tokens, "tone", token);
        return -1;
      }
    }
      else
    if (token[1] != 0)
    {
      print_error(tokens, "tone", token);
      return -1;
    }

    if (is_flat)
    {
      tokens.push("b", TOKEN_ALPHA);
    }

    while (true)
    {
      token_type = tokens.get(token);

      if (strcmp(token, "#") == 0)
      {
        mod = +1;
      }
        else
      if (strcmp(token, "b") == 0)
      {
        mod = -1;
      }
        else
      if (strcmp(token, "0.") == 0)
      {
        // The 0. is a screwiness in the tokenizer, probably there on
        // on purpose to deal with numbers .something instead of
        // 0.something. Could fix that later.
        duration += duration / 2;
      }
        else
      {
        tokens.push(token, token_type);
        break;
      }
    }

    value = value + (octave * 12);
    value += mod;

    Tones::Tone tone;

    tone.location = location;
    tone.value    = value;
    tone.length   = divisions * duration;
    tone.channel  = midi_channel;
    tone.volume   = value != 0 ? song_info.default_volume : 0;
    tones.add(tone);

    location += duration;

    tokens.get(token);

    if (strcmp(token, ";") == 0) { break; }

    if (strcmp(token, ",") != 0)
    {
      print_error(tokens, ",", token);
      return -1;
    }
  }

  return 0;
}

int Song::parse_phrase(Tokens &tokens)
{
  int token_type;
  char token[TOKEN_LEN], token1[TOKEN_LEN];
  Tones tones;
  int i, count;
  int midi_channel;
  int divisions = midi_file->get_divisions();

  midi_channel = 1;

  token_type = tokens.get(token);

  if (token_type != TOKEN_ALPHA)
  {
    printf("Error: Phrase is not alphanumeric at %s:%d\n",
      tokens.get_filename(), tokens.get_line());
    return -1;
  }

  if (phrases.find(token) != phrases.end())
  {
    printf("Error: Phrase %s is already defined at %s:%d\n",
      token, tokens.get_filename(), tokens.get_line());
    return -1;
  }

  std::string phrase_name = token;

  const int index = phrases.size();
  Phrase &phrase = phrases[phrase_name];

  phrase_names[index] = token;

  phrase.set_name(phrase_name);
  phrase.set_index(index);

  token_type = tokens.get(token);

  if (strcmp(token, "{") != 0)
  {
    print_error(tokens, "{", token);
    return -1;
  }

  while (true)
  {
    token_type = tokens.get(token);

    if (strcmp(token, "}") == 0) { break;}

    // Check if there are some modifications to the global song values.
    if (strcmp(token, "set") == 0)
    {
      token_type = tokens.get(token);

      if (token_type != TOKEN_ALPHA)
      {
        print_error(tokens, "variable to set", token);
        return -1;
      }

      token_type = tokens.get(token1);

      if (strcmp(token1, "=") != 0)
      {
        print_error(tokens, "=", token1);
        return -1;
      }

      token_type = tokens.get(token1);

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

#if 0
      if (strcmp(token, "bpm") == 0)
      {
        beat_time = 60000000 / i;
      }
        else
      if (strcmp(token, "time_signature") == 0)
      {
        time_signature_beats = i;

        token_type = tokens.get(token1);

        if (strcmp(token1, "/") != 0)
        {
          print_error(tokens, "/", token1);
          return -1;
        }

        token_type = tokens.get(token1);

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
      }
        else
#endif
      if (strcmp(token, "midi_channel") == 0)
      {
        midi_channel = i;

        if (midi_channel > 15)
        {
          printf("Warning: MIDI channel %d is higher than max 15; line %d\n",
            midi_channel, tokens.get_line());
          midi_channel = 9;
        }
      }
        else
      {
        printf("Warning: Cannot set '%s' in phrase.\n", token);
      }

      token_type = tokens.get(token);

      if (strcmp(token, ";") != 0)
      {
        print_error(tokens, ";", token);
        return -1;
      }

      continue;
    }

    if (strcmp(token, "voice") != 0)
    {
      print_error(tokens, "voice", token);
    }

    token_type = tokens.get(token);

    if (strcmp(token, ":") != 0)
    {
      print_error(tokens, ":", token);
    }

    if (parse_voice(tokens, tones, atoi(token), midi_channel) == -1)
    {
      return -1;
    }
  }

  // After reading all the notes, build the phrase based on a
  // sorted version of what was read in from the .dpp source.

  //tones.print();

  Tones::Tone temp_tone;

  count = 0;

  for (auto &tone : tones.tones)
  {
    if (count != 0)
    {
      phrase.add(
        temp_tone.value,
        temp_tone.volume,
        temp_tone.channel,
        temp_tone.length,
        temp_tone.location * divisions);
    }

    temp_tone = tone;

    count++;
  }

  // Add the last tone to the phrase.
  if (count != 0)
  {
    phrase.add(
      temp_tone.value,
      temp_tone.volume,
      temp_tone.channel,
      temp_tone.length,
      temp_tone.location * divisions);
  }

  //phrase.print();

  return 0;
}

int Song::parse_melody(Tokens &tokens)
{
  char token[TOKEN_LEN];
  int token_type;
  int repeat;
  std::string track_name = song_info.song_name;

  token_type = tokens.get(token);

  if (token_type == TOKEN_ALPHA)
  {
    track_name = token;
    token_type = tokens.get(token);
  }

  if (strcmp(token, ":") != 0)
  {
    print_error(tokens, ":", token);
    return -1;
  }

  midi_file->write_track_start(track_name);
  midi_file->write_bpm(song_info);

  while (true)
  {
    repeat = 1;

    token_type = tokens.get(token);

    if (token_type == TOKEN_NUMBER)
    {
      repeat = atoi(token);

      token_type = tokens.get(token);
    }

    std::string phrase_name = token;

    for (int i = 0; i < repeat; i++)
    {
      play_phrase(phrase_name);
    }

    token_type = tokens.get(token);

    if (strcmp(token, ";") == 0) { break; }

    if (strcmp(token, ",") != 0)
    {
      print_error(tokens, ",", token);
      return -1;
    }
  }

  midi_file->write_track_end();

  return 0;
}

int Song::parse_play(Tokens &tokens)
{
  int token_type;
  char token[TOKEN_LEN];
  int repeat;
  std::string track_name = song_info.song_name;

  token_type = tokens.get(token);

  if (token_type == TOKEN_ALPHA)
  {
    track_name = token;
    token_type = tokens.get(token);
  }

  if (strcmp(token, ":") != 0)
  {
    print_error(tokens, ":", token);
    return -1;
  }

  midi_file->write_track_start(track_name);
  midi_file->write_bpm(song_info);

  while (true)
  {
    repeat = 1;

    token_type = tokens.get(token);

    if (token_type == TOKEN_NUMBER)
    {
      repeat = atoi(token);
      token_type = tokens.get(token);
    }

    std::string section_name = token;

    if (sections.find(section_name) != sections.end())
    {
      for (int x = 0; x < repeat; x++)
      {
        play_section(section_name);
      }
    }
      else
    {
      if (patterns.find(token) == patterns.end())
      {
        printf("Error:  Undefined pattern '%s' at %s:%d.\n",
          token, tokens.get_filename(), tokens.get_line());
        return -1;
      }

      std::string pattern_name = token;

      for (int x = 0; x < repeat; x++)
      {
        play_pattern(pattern_name);
      }
    }

    token_type = tokens.get(token);

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

  midi_file->write_track_end();

  return 0;
}

int Song::parse_song(Tokens &tokens)
{
  int token_type;
  char token[TOKEN_LEN];

  token_type = tokens.get(token);

  if (strcmp(token, "{") != 0)
  {
    if (token_type == TOKEN_ALPHA)
    {
      song_info.set_song_name(token);

      token_type = tokens.get(token);

      if (strcmp(token, "{") != 0)
      {
        print_error(tokens, "{", token);
        return -1;
      }
    }
      else
    {
      printf("Error: Expecting song name or '{' but got '%s' at %s:%d.\n",
        token, tokens.get_filename(), tokens.get_line());
      return -1;
    }
  }

  midi_file->write_header();

  while (true)
  {
    token_type = tokens.get(token);

    if (strcmp(token, "}") == 0)
    {
      break;
    }

    if (strcmp(token, "play") == 0)
    {
      if (parse_play(tokens) != 0) { return -1; }
    }
      else
    if (strcmp(token, "melody") == 0)
    {
      if (parse_melody(tokens) == -1) { return -1; }
    }
  }

  return 0;
}

int Song::play_section(std::string &name)
{
  if (sections.find(name) == sections.end())
  {
    printf("Could not find section %s\n", name.c_str());
    return -1;
  }

  Section &section = sections[name];

  if (interactive)
  {
    printf("Section: %s\n", name.c_str());
  }

#ifdef DEBUG
printf("playing section: %s\n", name.c_str());
#endif

  std::vector<int> &patterns = section.get_patterns();

  for (auto it : patterns)
  {
    if (pattern_names.find(it) == pattern_names.end())
    {
      printf("Error: Unknown pattern '%d'.\n", it);
      return -1;
    }

    std::string &pattern_name = pattern_names[it];

    play_pattern(pattern_name);
  }

  return 0;
}

void Song::play_pattern(std::string &name)
{
  int index, count;
  Note note;

  Pattern &pattern = patterns[name];

  if (interactive)
  {
    printf("Pattern: %s", name.c_str());
  }

#ifdef DEBUG
printf("  Playing pattern %s\n", name.c_str());
printf("[ ");
#endif

  count = pattern.get_count();

  for (index = 0; index < count; index++)
  {
    MidiData &midi_data = pattern.get_data(index);

#ifdef DEBUG
printf("%x %x %x, ",
  0x90 + midi_data.channel, midi_data.value, midi_data.volume);
#endif

    note.value        = midi_data.value;
    note.volume       = midi_data.volume;
    note.duration     = midi_data.duration;
    note.midi_channel = midi_data.channel;

    midi_file->write_note(song_info, note);
  }

#ifdef DEBUG
printf(" ]\n");
#endif
}

int Song::play_phrase(std::string &name)
{
  if (phrases.find(name) == phrases.end())
  {
    printf("Could not find phrase %s\n", name.c_str());
    return -1;
  }

  Phrase &phrase = phrases[name];

  //phrase.print();

  std::set<NoteOff, CompareNoteOff> note_offs;

  int count = phrase.get_count();
  int index = 0;
  uint32_t location = 0;
  uint32_t location_last_note_off = 0;

  while (index != count || !note_offs.empty())
  {
    if (!note_offs.empty())
    {
      while (true)
      {
        auto note_off = note_offs.begin();

        if (note_off->location != location) { break; }

        midi_file->write_note_off(
          note_off->value,
          note_off->channel,
          location - location_last_note_off);

        note_offs.erase(*note_off);

        location_last_note_off = location;
      }
    }

    while (index < count)
    {
      MidiData &midi_data = phrase.get_data(index);

      if (midi_data.location != location) { break; }

      if (midi_data.value != 0)
      {
        midi_file->write_note_on(
          midi_data.value,
          midi_data.channel,
          0,
          midi_data.volume);
      }

      NoteOff note_off;
      note_off.location = location + midi_data.duration;
      note_off.value    = midi_data.value;
      note_off.channel  = midi_data.channel;

      note_offs.insert(note_off);

      index += 1;
    }

    location += 1;
  }

  return 0;
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

void Song::print_error(Tokens &tokens, const char *expect, const char *got)
{
  printf("Error: Expected '%s' and got '%s' at %s:%d.\n",
    expect, got, tokens.get_filename(), tokens.get_line());
}

