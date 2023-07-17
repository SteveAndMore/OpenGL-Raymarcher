CC = g++
CFLAGS = -Wall -Wextra
LDFLAGS = -lGL -lGLEW -lglut -lpthread

main: main.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f main
