# Diretórios de acesso 

INC=../../include
LIB=../../lib
BIN=../../bin

#FLAGS= -g -Wall 
FLAGS= -O3 -Wall

PROGRAMS = scninfo raw2scene pgm2scene scene2pgm interp sb_interp\
	   rawslice2scene cube mbb3 scnclip scn2shell scn2bshell scnthresh\
	   tensorscalematch equalize equalize3 matchhisto matchhisto3 \
	   matchhistomasks3 interplabels watergray3

all: $(PROGRAMS)

$(LIB)/libift.a:
	cd ../../; make; cd util/src/

$@.c: $(LIB)/libift.a $@.c 
	cd ../../; make; cd util/src
	gcc $(FLAGS) $@.c -o $(BIN)/$@ -I$(INC) -L$(LIB) -lift -lm

clean:
	rm $(BIN)/*; rm *.pgm; rm *~










