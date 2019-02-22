VPATH=../src
CC=gcc
CPP=g++

CFLAGS=-Wall $(DEBUG) -O3
OBJECTS= \
  Tokens.o \
  Midi.o \
  Parser.o \
  Utility.o

default: $(OBJECTS)
	$(CPP) -o ../playdpp ../src/playdpp.cxx \
	  $(OBJECTS) \
	  $(CFLAGS) $(LIBS)

%.o: %.cxx %.h
	$(CPP) -c $< -o $*.o $(CFLAGS)

%.o: %.cxx
	$(CPP) -c $< -o $*.o $(CFLAGS)

