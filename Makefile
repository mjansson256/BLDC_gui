#Simple Makefile

PROGRAM = main

ifndef $(INSTALLDIR)
	INSTALLDIR = /usr/local/sbin
endif

CC = gcc

CFLAGS = -ggdb -O0
CFLAGS += -Wl,--export-dyamic
CFLAGS += -lgmodule-export-2.0

SOURCEDIR = src

SOURCES = $(SOURCEDIR)/serialcom.c
SOURCES += $(SOURCEDIR)/main.c
OBJECTS = $(SOURCES:%.c=%.o)


all: clean $(OBJECTS) compile

compile:
	$(CC) $(CLFAGS) $(OBJECTS) -o$(PROGRAM) `pkg-config --cflags --libs gtk+-2.0` $(LIBS)

mtest: test/test_signal.o
	$(CC) $(CLFAGS) test/test_signal.o -omtest `pkg-config --cflags --libs gtk+-2.0` $(LIBS)


%.o: %.c
	$(CC) -c $(CFLAGS) $< -o$@ `pkg-config --cflags --libs gtk+-2.0`

install: $(PROGRAM)
	cp $(PRGRAM) $(INSTALLDIR)

clean:
	rm -f main $(OBJECTS) $(PROGRAM)
