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
#include "Song.h"
#include "SongInfo.h"

// FIXME: OUCH
char defines[MAX_LITERAL_SPACE];
struct SongInfo song_info;
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
//const char *current_filename;

#ifdef WINDOWS
HMIDIOUT inHandle;
#endif

int main(int argc, char *argv[])
{
  Tokens tokens;
  const char *infile = "";
  const char *outfile = "/dev/midi00";
  int t;

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
      outfile = argv[++t];
    }
      else
    if (strcmp(argv[t],"-o") == 0)
    {
      outfile = argv[++t];

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
      infile = argv[t];
    }
  }

  if (infile[0] == 0)
  {
    printf("\nError: No input filename specified.\n\n");
    exit(1);
  }

  if (strcmp(infile, outfile) == 0)
  {
    printf("\nError: The input / output filenames cannot be the same.\n\n");
    exit(1);
  }

  printf(DLPLAYER_INFO);
  printf(COPYRIGHT);
  fflush(out);

  if (tokens.open(infile) == 0)
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
      tokens.close();
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

  printf("Infile: %s\n\n", infile);

  defines[0] = 0;
  defines[1] = 0;
  pattern_names[0] = 0;
  pattern_names[1] = 0;
  section_names[0] = 0;
  section_names[1] = 0;
  strcpy((char *)song_name,"undefined");

  //current_filename = infile;

  main_parser(&tokens);

#ifdef DEBUG
song.Print();
#endif

  tokens.close();

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

