PROGS= testsuite

all: $(PROGS)

#Compiladores
CXX=g++
CC=gcc

FLAGS= -O3 -Wall
#FLAGS= -g  -Wall -D _DEBUG


#Bibliotecas
IFT_DIR = /home/afalcao/ift
IFTLIB  = -L$(IFT_DIR)/lib -L$(IFT_DIR)/test/libps -lift -lm -lps
IFTFLAGS  = -I$(IFT_DIR)/include


#Rules


libift:
	$(MAKE) -C $(IFT_DIR)

$@.c: libift $@.c
	$(CC) $(FLAGS) $(IFTFLAGS) $@.c $(IFTLIB) -o $@

registerimages.c: libift registerimages.c
	$(CC) $(FLAGS) $(IFTFLAGS) registerimages.c $(IFTLIB) -o registerimages

clean:
	$(RM) *~  $(PROGS) -f



