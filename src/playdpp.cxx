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

#include "general.h"
#include "version.h"
#include "MidiFile.h"
#include "MidiPlayer.h"
#include "Song.h"
#include "SongInfo.h"

// FIXME: OUCH
char defines[MAX_LITERAL_SPACE];
unsigned char pattern[PATTERN_HEAP_SIZE];
unsigned int pattern_duration[PATTERN_HEAP_SIZE];
unsigned char pattern_volume[PATTERN_HEAP_SIZE];
unsigned char pattern_channel[PATTERN_HEAP_SIZE];
int song[MAX_SONG_SIZE];
int patterns_ptr;
char pattern_names[MAX_LITERAL_SPACE];

int main(int argc, char *argv[])
{
  Tokens tokens;
  MidiFile midi_file;
  MidiPlayer midi_player;
  Song *song;
  const char *infile = "";
  const char *outfile = "/dev/midi00";
  int t;
  int interactive = 0;


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

      if (midi_file.open(outfile) != 0)
      {
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
  fflush(stdout);

  if (tokens.open(infile) == 0)
  {
    printf("Error:  Couldn't open file %s\n", infile);
    exit(1);
  }

  if (!midi_file.is_open())
  {
    if (midi_player.open(outfile) != 0)
    {
      tokens.close();
      exit(1);
    }
  }

  printf("Infile: %s\n\n", infile);

  defines[0] = 0;
  defines[1] = 0;
  pattern_names[0] = 0;
  pattern_names[1] = 0;

  //current_filename = infile;

  song = new Song();

  if (interactive == 1) { song->set_interactive(); }

  song->parse(&tokens, &midi_file);

#ifdef DEBUG
  song->Print();
#endif

  tokens.close();

  if (midi_file.is_open())
  {
    midi_file.close();
  }
    else
  {
    midi_player.close();
  }

  delete song;

  return 0;
}

