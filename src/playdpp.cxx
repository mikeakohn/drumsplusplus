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
#include "Parser.h"

// FIXME: OUCH
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


#ifdef WINDOWS
HMIDIOUT inHandle;
#endif

int main(int argc, char *argv[])
{
  char infile[1024];
  char outfile[1024];
  int t;
  FILE *in;

  infile[0] = 0;
  strcpy(outfile, "/dev/midi00");
  out = 0;
  interactive = 0;

  if (argc == 1)
  {
    printf(DLPLAYER_INFO);
    printf(COPYRIGHT);
    printf("Usage: playdpp <infile>\n\n");
    printf("       -midi <midi_device: default is /dev/midi00>\n");
    printf("       -o <.midi file to output to: disables /dev/midi>\n");
    printf("       -i [ will show patters as they are played ]\n\n");
    exit(0);
  }

  for (t = 1; t < argc; t++)
  {
    if (strcmp(argv[t], "-i") == 0)
    {
      interactive = 1;
    }
      else
    if (strcmp(argv[t],"-midi") == 0)
    {
      strcpy(outfile, argv[++t]);
    }
      else
    if (strcmp(argv[t],"-o") == 0)
    {
      strcpy(outfile, argv[++t]);
      out = fopen(outfile,"wb");

      if (out == NULL)
      {
        printf("Couldn't open file %s for output.\n", outfile);
        exit(1);
      }
    }
      else
    if (argv[t][0] == '-')
    {
      printf("Unknown option\n");
      exit(1);
    }
      else
    {
      strcpy(infile, argv[t]);
    }
  }

  if (strcmp(infile,outfile) == 0)
  {
    printf("\nProblem:  The input and output filenames cannot be the same.\n\n");
    exit(1);
  }

  printf(DLPLAYER_INFO);
  printf(COPYRIGHT);
  fflush(out);

  in = fopen(infile, "rb");

  if (in == NULL)
  {
    printf("Error:  Couldn't open file %s\n", infile);
    exit(1);
  }

  if (out == NULL)
  {

#ifndef WINDOWS
    midiout = open(outfile, O_WRONLY, 0);

    if (midiout < 0)
    {
      printf("Error opening up sequencer.\n");
      fclose(in);
      exit(1);
    }
#else
    int result = midiOutOpen(&inHandle, (UINT) -1, 0, 0, CALLBACK_NULL);

    if (result)
    {
      printf("Couldn't open MIDI device.\n");
      exit(1);
    }
#endif
  }

  song_info.bpm = 120;
  song_info.defaultvolume = 127;
  song_info.drift = 5;
  song_info.timesignature_beats = 4;
  song_info.timesignature_base = 4;
  song_info.midi_channel = 9;

  printf("Infile: %s\n\n", infile);
  defines[0] = 0;
  defines[1] = 0;
  pattern_names[0] = 0;
  pattern_names[1] = 0;
  section_names[0] = 0;
  section_names[1] = 0;
  line=1;
  strcpy((char *)song_name,"undefined");

  current_filename = infile;

  main_parser(in);

#ifdef DEBUG
printf("BMP: %d\n", song_info.bpm);
printf("Default Volume: %d\n", song_info.defaultvolume);
printf("Drift: %d\n", song_info.drift);
printf("Timesignature: %d/%d\n", song_info.timesignature_beats, song_info.timesignature_base);
printf("MIDI Channel: %d\n", song_info.midi_channel);
#endif

  fclose(in);

  if (out == 0)
  {
#ifndef WINDOWS
    close(midiout);
#else
    midiOutClose(inHandle);
#endif
  }
    else
  {
    fclose(out);
  }

  return 0;
}

