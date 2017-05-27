# Producing Postscript figures from C programs
# Last edited on 2003-01-14 15:43:54 by stolfi

LIBNAME = libps

HFILES := \
  js.h \
  ps.h \
  pswr.h \
  pswrdef.h \
  pswraux.h 


CFILES := \
  js.c \
  ps.c \
  pswr.c \
  pswrvis.c 

HOFILES := ${subst .h,.ho,${HFILES}}

OFILES := ${subst .c,.o,${CFILES}}
  
LIBFILE = \
  ${LIBNAME}.a

all: ${LIBFILE}

# Library dependencies: 
  
${LIBFILE}: ${OFILES} ;\
  rm -f $*.a && ar crv $*.a ${OFILES} && ranlib $*.a

# Compilation rules: 
  
GCCFLAGS = \
  -I. \
  -g \
  -ansi \
  -Wall  -Wpointer-arith -Wmissing-prototypes
  
%.o: %.c ;\
  gcc -c ${GCCFLAGS} $*.c
  
%.ho: %.h ;\
  gcc -o $*.ho -c ${GCCFLAGS} -x c $*.h \
  || /bin/rm -f $*.ho
  
# Dependencies of .h files: 
js.ho: js.h  
ps.ho: ps.h
  
pswr.ho: pswr.h pswrdef.ho
pswrdef.ho: pswrdef.h
pswraux.ho: pswraux.h pswr.ho
pswrvis.ho: pswrvis.h pswr.ho
  

# Dependencies for .c files:

js.o: js.c js.ho
ps.o: ps.c ps.ho pswr.ho js.ho 

pswr.o: pswr.c pswr.ho pswraux.ho pswrdef.ho pswrvis.ho \
              js.ho 

pswrvis.o: pswrvis.c pswrvis.ho pswr.ho

