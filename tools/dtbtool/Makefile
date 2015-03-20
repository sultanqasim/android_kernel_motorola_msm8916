# Simple standalone Makefile for Motorola dtbtool
# Copyright (C) 2015 Sultan Qasim Khan <sultanqasim@gmail.com>
# All rights reserved.
#
# Licensed to all under the BSD 2-Clause License
# http://opensource.org/licenses/BSD-2-Clause

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -I.
LFLAGS = -lc -lm

ifeq ($(DEBUG),1)
    CFLAGS += -g -O0
else
    CLFAGS += -O3
endif

all: dtbtool

dtbtool: dtbtool.o
	$(CC) $(LFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o dtbtool
