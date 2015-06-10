CFLAGS=-O -Wall
LIBS=-lm

all: tex

# Might want to name it something other 
# than "tex", which Knuth claimed for TeX, but
# I like it, it's short, and I don't use TeX
# all that much anymore.

tex:	tex.o
	$(CC) -o tex $(CFLAGS) tex.o $(LIBS)
