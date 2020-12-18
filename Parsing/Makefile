CC=g++
CFLAGS=-Wall -Werror
DEBUG_FLAG=-g
FILE=parser.cpp
OUTPUT=parser_exec
OUTPUT_DEBUG=parser_exec_dbg

parser: 
	$(CC) $(CFLAGS) $(FILE) -o $(OUTPUT)

debug:
	$(CC) $(CFLAGS) $(DEBUG_FLAG) $(FILE) -o $(OUTPUT_DEBUG)

rm:
	rm parser_exec parser_exec_dbg *.csv
