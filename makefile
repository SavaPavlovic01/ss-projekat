#SRCS:=./misc/lex.yy.c ./misc/parser.tab.c ./src/instruction.c ./src/main.c
SRCS:=./src/instruction.c ./misc/lex.yy.c ./misc/parser.tab.c ./src/assembler.cpp ./src/symbTable.cpp \
./src/instrHelp.cpp ./src/Events.cpp ./src/sectionTable.cpp ./src/LPool.cpp ./src/codeGen.cpp ./src/relocTable.cpp
#DEPS:=./inc/instruction.h ./misc/parser.tab.h ./misc/lexer.h
DEPS:=./misc/parser.tab.h ./misc/lexer.h 

all:${DEPS}
	g++ ${SRCS} -g -o assembler

./misc/parser.tab.c ./misc/parser.tab.h:
	bison --defines=./misc/parser.tab.h ./misc/parser.y --output=./misc/parser.tab.c
 

./misc/lex.yy.c ./misc/lexer.h:	
	flex --header-file=./misc/lexer.h --outfile=./misc/lex.yy.c ./misc/lexer.l 

clean:
	rm -rf ./misc/lexer.h ./misc/lex.yy.c ./misc/parser.tab.h ./misc/parser.tab.c runMe

test:
	./assembler ./tests/emuTest.txt -o emu.o
	./linker emu.o -place=text@0x40000000 -hex -o emu.hex
	./emulator emu.hex

obj:
	rm -rf objdump
	g++ ./src/objdump.cpp -g -o objdump


link:
	g++ ./src/linker/linker.cpp ./src/symbTable.cpp ./src/sectionTable.cpp ./src/relocTable.cpp -g -o linker ./src/LPool.cpp

em:
	g++ -pthread ./src/emu.cpp ./src/Emulator.cpp -g -o emulator

ra:
	./assembler -o main.o ./testA/main.s
	./assembler -o math.o ./testA/math.s
	./assembler -o ivt.o ./testA/ivt.s
	./assembler -o isr_reset.o ./testA/isr_reset.s
	./assembler -o isr_terminal.o ./testA/isr_terminal.s
	./assembler -o isr_timer.o ./testA/isr_timer.s
	./assembler -o isr_user0.o ./testA/isr_user0.s
	./linker -hex -o program.hex ivt.o math.o main.o -place=my_code@0x40000000 isr_reset.o isr_terminal.o isr_timer.o isr_user0.o
	./emulator program.hex

sad:
	./assembler -o inter1.o ./tests/test1.txt
	./assembler -o inter2.o ./tests/test2.txt
	./linker -hex inter1.o inter2.o -place=my_code_main@0x40000000 -place=my_code_handler@0xc0000000 -o inter.hex
	./emulator inter.hex