CC = gcc
CFLAGS = -g -Wall -O2 -pg
INCLUDES = -I ./include
SRCS = 	./src/json.c\
		./src/parse.c\
		./src/stringpify.c\
		./test/test.c

TARGETS = $(SRCS:.c=.o)

all: slowjsontest

slowjsontest: $(TARGETS)
	$(CC) $(CFLAGS) $(TARGETS) -o slowjsontest

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm ./src/*.o ./test/*.o slowjsontest
