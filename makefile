#SRCS:=./misc/lex.yy.c ./misc/parser.tab.c ./src/instruction.c ./src/main.c
SRCS:=./src/instruction.c ./misc/lex.yy.c ./misc/parser.tab.c ./src/assembler.cpp ./src/symbTable.cpp \
./src/instrHelp.cpp ./src/Events.cpp ./src/sectionTable.cpp ./src/LPool.cpp ./src/codeGen.cpp ./src/relocTable.cpp
#DEPS:=./inc/instruction.h ./misc/parser.tab.h ./misc/lexer.h
DEPS:=./misc/parser.tab.h ./misc/lexer.h 

all:${DEPS}
	g++ ${SRCS} -g -o runMe

./misc/parser.tab.c ./misc/parser.tab.h:
	bison --defines=./misc/parser.tab.h ./misc/parser.y --output=./misc/parser.tab.c
 

./misc/lex.yy.c ./misc/lexer.h:	
	flex --header-file=./misc/lexer.h --outfile=./misc/lex.yy.c ./misc/lexer.l 

clean:
	rm -rf ./misc/lexer.h ./misc/lex.yy.c ./misc/parser.tab.h ./misc/parser.tab.c runMe

test:
	gcc -c ./src/instruction.c ./misc/lex.yy.c ./misc/parser.tab.c
	g++ -c ./src/main.cpp
	g++ instruction.o lex.yy.o parser.tab.o main.o -o run