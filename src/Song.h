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

#ifndef DPP_PARSER_H
#define DPP_PARSER_H

#include "MidiFile.h"
#include "Tokens.h"

void error(char *expect, char *got);
int parse_set(Tokens *tokens);
int main_parser(Tokens *tokens, MidiFile *midi_file);

#endif

