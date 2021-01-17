CFLAGS  = -std=c99 -pedantic -Wall -Wextra

.SUFFIXES :
.SUFFIXES : .o .c

.c:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

include config.mk

all : charclass
