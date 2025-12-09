CC = gcc
CFLAGS = -Wall -O2
LIBS = -lX11 -lXft -lXext

all: osc_x11

osc_x11: osc_x11.c tinyosc.c
	$(CC) $(CFLAGS) -o osc_x11 osc_x11.c tinyosc.c $(LIBS)

clean:
	rm -f osc_x11

.PHONY: all clean
