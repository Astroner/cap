CC=gcc-13
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=run

.PHONY: all
all: $(EXECUTABLE)
	./$(EXECUTABLE) $(EFLAGS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(CFLAGS) $^

$(OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -Wall -Wextra -std=c99 -pedantic -c -o $@ $<

lib: src/cap.c src/cap.h
	cat src/cap.h > cap.h
	echo "#if defined(CAP_IMPLEMENTATION)" >> cap.h
	tail -n +2 src/cap.c >> cap.h
	echo "#endif // CAP_IMPLEMENTATION" >> cap.h

.PHONY: tests
tests: lib
	$(CC) -o test -Wall -Wextra -std=c99 -pedantic tests/main.spec.c
	./test
	rm -f test

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJECTS) test