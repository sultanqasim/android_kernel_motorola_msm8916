CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu11 -I. -I../../include
LFLAGS = -lc -lm

ifeq ($(DEBUG),1)
    CFLAGS += -g -O0
else
    CLFAGS += -O3
endif

all: mkbootfs

mkbootfs: mkbootfs.c android_filesystem_config.h android_filesystem_capability.h
	$(CC) $(CFLAGS) $(LFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o mkbootfs
