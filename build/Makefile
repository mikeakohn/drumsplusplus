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
	$(CPP) -o ../playdpp ../src/playdpp.cpp \
	  $(OBJECTS) \
	  $(CFLAGS) $(LIBS)

%.o: %.cpp %.h
	$(CPP) -c $< -o $*.o $(CFLAGS)

%.o: %.cpp
	$(CPP) -c $< -o $*.o $(CFLAGS)

