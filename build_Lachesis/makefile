.SUFFIXES:
	#In order to avoid this situation with the same name as the file, 
	#we can use a special tag ".PHONY" to explicitly indicate that a 
	#target is a "pseudo target", indicating to make that whether 
	#there is this file or not, this target is a "pseudo target". Target".
.PHONY: all clean install run
FLAGS := -Wall 
cc = clang
target = Lachesis
SRCDIR := src
TESTDIR := test
deps = $(shell find  ./src -name "*.h")
src = $(shell find  ./lib -name "*.c"&& find ./src -name "*.c")
obj = $(src:%.c=%.o)


debug:
	#-g means gdb
	$(cc) -Wall -g  -lreadline  $(src) 
	mv a.out Lachesis
install:
	$(cc) -Wall  -lreadline  $(src) 
	mv a.out build/Lachesis
run: 
	./build/Lachesis
clean:
	-@rm -f Lachesis
	-@rm -f src/*.o test/*.o
