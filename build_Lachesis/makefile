.SUFFIXES:
	#In order to avoid this situation with the same name as the file, 
	#we can use a special tag ".PHONY" to explicitly indicate that a 
	#target is a "pseudo target", indicating to make that whether 
	#there is this file or not, this target is a "pseudo target". Target".
.PHONY: all clean install run debug test
FLAGS := -Wall 
cc = clang
target =i lac
SRCDIR := src
TESTDIR := test
deps = $(shell find  ./src -name "*.h")
src = $(shell find  ./lib -name "*.c"&& find ./src -name "*.c")
testfile = $(shell find  ./test -name "*.lac")
obj = $(src:%.c=%.o)

debug:
	#-g means gdb
	$(cc) -Wall -g  -lreadline  $(src) -o lac 
install:
	$(cc) -Wall -O3 -lreadline  $(src) -o ./build/lac
test: 
	./lac $(testfile)
clean:
	-@rm -f lac
	-@rm -f build/lac
