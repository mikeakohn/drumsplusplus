VPATH=../src
CC=gcc
CPP=g++

#DEBUG=-DDEBUG
CFLAGS=-Wall -O3 -std=c++11 $(DEBUG)
OBJECTS= \
  Tokens.o \
  MidiFile.o \
  MidiPlayer.o \
  Pattern.o \
  Section.o \
  Song.o \
  SongInfo.o \
  Utility.o

default: $(OBJECTS)
	$(CPP) -o ../playdpp ../src/playdpp.cxx \
	  $(OBJECTS) \
	  $(CFLAGS) $(LIBS)

%.o: %.cxx %.h
	$(CPP) -c $< -o $*.o $(CFLAGS)

%.o: %.cxx
	$(CPP) -c $< -o $*.o $(CFLAGS)

