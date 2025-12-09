CC = clang
CFLAGS = -Iinclude
LDFLAGS = -Llib -lraylib \
          -framework CoreVideo \
          -framework IOKit \
          -framework Cocoa \
          -framework GLUT \
          -framework OpenGL
MANUAL = manual
OSC = osc
all: $(MANUAL) $(OSC)

$(MANUAL): manual.cpp
	$(CC) $(CFLAGS) manual.cpp $(LDFLAGS) -o $(MANUAL)

$(OSC): osc.cpp tinyosc.c
	$(CC) $(CFLAGS) tinyosc.c osc.cpp $(LDFLAGS) -o $(OSC)

clean:
	rm -f $(MANUAL) $(OSC)

run-manual: $(MANUAL)
	./$(MANUAL)

run-osc: $(OSC)
	./$(OSC)

.PHONY: all clean run-manual run-osc
