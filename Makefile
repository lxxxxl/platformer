CC       = gcc
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:.c=.o)
TARGETS  = platformer

# Windows specific flags
ifeq ($(OS),Windows_NT)
	RM = del /Q *.exe
	CFLAGS  = -m32 -msse2
	LDFLAGS = ../lib/win32/Tilengine.lib -s
else
	RM = rm
	name := $(shell uname -s)
	arch := $(shell uname -m)
	
	# Linux specific flags (i686, x64 and arm)
	ifeq ($(name),Linux)
		ifeq ($(arch),i686)
			CFLAGS = -m32 -msse2
		endif
		ifeq ($(arch),x86_64)
			CFLAGS = -m64 -msse2
		endif
		LIBPATH = Tilengine/lib/linux_$(arch)
		LDFLAGS = -L$(LIBPATH) -lTilengine -lm -s -Wl,-rpath,$(LIBPATH)
	endif
	
	# OSX specific flags
	ifeq ($(name),Darwin)
		LDFLAGS = "/usr/local/lib/Tilengine.dylib" -lm
	endif
endif

# common C flags
CFLAGS += -I"Tilengine/include" -std=c99 -O2

.PHONY: all all-before all-after clean clean-custom

all: $(TARGETS)

platformer: Platformer.o
	$(CC) Platformer.o -o platformer $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# clean
clean: clean-custom
	$(RM) $(OBJECTS) $(TARGETS)

