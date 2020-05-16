CC = gcc

TARGET = main
OBJECTS = tetris.o treap.o
# CFLAGS = -g -fsanitize=address -fno-optimize-sibling-calls
CFLAGS = -O2
LDFLAGS = -lncurses -lm

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS) 

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS) 


.PHONY: clean
clean:
	rm $(TARGET) $(OBJECTS); git rm $(TARGET) $(OBJECTS)
