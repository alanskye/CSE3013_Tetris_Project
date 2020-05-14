CC = clang

TARGET = main
OBJECTS = tetris.o treap.o
#CFLAGS = -g -fsanitize=address -fno-optimize-sibling-calls
CFLAGS = -O3
LDFLAGS = -lncurses -lm

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<


.PHONY: clean
clean:
	rm $(TARGET) $(OBJECTS); git rm $(TARGET) $(OBJECTS)
