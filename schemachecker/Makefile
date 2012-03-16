MAIN = xchecker
CC = g++
CFLAGS = -g -Wall
$(MAIN):main.cpp
	$(CC) $(CFLAGS) main.cpp -o $(MAIN) -L/usr/lib -lxerces-c -pedantic -I/usr/include

.PHONY:clean
clean:
	@rm $(MAIN)
