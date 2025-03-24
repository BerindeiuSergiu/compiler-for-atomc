# name of the compiler
CC = gcc

# flags to be added for the compiler
CFLAGS = -Wall -I./utils -I./lexer -I./parser

# source files
SRCS = main.c ./utils/utils.c ./lexer/lexer.c ./parser/parser.c

# creating object files
OBJS = main.o utils.o lexer.o parser.o

# name for executable
EXEC = run

# default target rule
all: $(EXEC)

# rule to link the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

# rule for main.c
main.o: main.c
	$(CC) $(CFLAGS) -c main.c

# rule for utils.c
utils.o: ./utils/utils.c
	$(CC) $(CFLAGS) -c ./utils/utils.c

# rule for lexer.c
lexer.o: ./lexer/lexer.c
	$(CC) $(CFLAGS) -c ./lexer/lexer.c

# rule for parser.c
parser.o: ./parser/parser.c
	$(CC) $(CFLAGS) -c ./parser/parser.c

# cleanup
clean:
	del $(OBJS) $(EXEC)