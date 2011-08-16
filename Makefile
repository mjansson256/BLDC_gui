#Simple Makefile

PROGRAM = main

ifndef $(INSTALLDIR)
	INSTALLDIR = /usr/local/sbin
endif

CC = gcc

CFLAGS = -ggdb

SOURCEDIR = src

SOURCES = $(SOURCEDIR)/serialcom.c
SOURCES += $(SOURCEDIR)/main.c
OBJECTS = $(SOURCES:%.c=%.o)


all: clean $(OBJECTS) compile

compile:
	$(CC) $(CLFAGS) $(OBJECTS) -o$(PROGRAM) `pkg-config --cflags --libs gtk+-2.0`

%.o: %.c
	$(CC) -c -ggdb $(CLFAGS) $< -o$@ `pkg-config --cflags --libs gtk+-2.0`

install: $(PROGRAM)
	cp $(PRGRAM) $(INSTALLDIR)

clean:
	rm -f main $(OBJECTS) $(PROGRAM)
