VPATH=../src
CC=gcc
CPP=g++

CFLAGS=-Wall $(DEBUG) -O3
OBJECTS= \
  gettoken.o \
  midi.o \
  parser.o \
  utility.o

default: $(OBJECTS)
	$(CC) -o ../playdpp ../src/playdpp.c \
	  $(OBJECTS) \
	  $(CFLAGS) $(LIBS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $*.o $(CFLAGS)

