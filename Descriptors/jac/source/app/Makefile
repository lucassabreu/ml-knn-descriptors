INC=../include
LIB=../lib
BIN=../bin
FLAGS= -Wall -g
#CC=/opt/crosstool/gcc-4.1.0-glibc-2.3.6/x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-gcc
CC=gcc
#CC=c:\cygwin\opt\crosstool\gcc-3.3.6-glibc-2.3.2\i386-unknown-linux-gnu\bin\i386-unknown-linux-gnu-gcc
#CC=C:\cygwin\opt\crosstool\gcc-3.3.6-glibc-2.3.2\x86_64-unknown-linux-gnu\bin\x86_64-unknown-linux-gnu-gcc.exe

$@.c: $(LIB)/libcolordescriptors.a $@.c
	cd ../; make; cd app
	$(CC) $(FLAGS) $@.c -o $(BIN)/$@ -I$(INC) -L$(LIB) -lcolordescriptors -lm

clean:
	rm $(BIN)/*

