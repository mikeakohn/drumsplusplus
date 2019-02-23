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

#ifndef DPP_UTILITY_H
#define DPP_UTILITY_H

int insert_define(char *buffer, char *token, char *value);
int insert_literal(char *buffer, char *token);
void print_all(const char *buffer);
int get_define(char *buffer, char *token, char *value);
int get_literal(char *buffer, char *token);
int is_number(char *myString);
int search_end_buffer(int *buffer);
int find_pattern(int i);
void print_name(char *buffer, int i);

#endif

