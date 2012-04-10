CC = gcc
CFLAGS += -D_GNU_SOURCE=1 -Wall -std=c99 -pipe \
-g
#-O3 -march=core2 -mtune=core2 -fno-stack-protector -fomit-frame-pointer \
#-U_FORTIFY_SOURCE -msse, -msse2, -msse3 -mfpmath=sse -mftree-vectorize
LDFLAGS = -lSDL -lz -lSDL_image -lGL -lGLU
builddir = ../build/linux
exec = ../cmc
headers = $(wildcard *.h)
sources = $(wildcard *.c)
objects = $(patsubst %.c,$(builddir)/%.o,$(sources))

all: $(exec)

$(exec): $(objects)
	$(CC) -o $(exec) $(objects) $(CFLAGS) $(LDFLAGS)

$(builddir)/%.o: %.c $(headers)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(objects) $(exec) *~
	
.PHONY: clean
