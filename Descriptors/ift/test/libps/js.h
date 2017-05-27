/* J. Stolfi's miscellaneous general-purpose definitions */
/* Last edited on 2003-07-07 13:26:09 by stolfi */

#ifndef JS_H
#define JS_H

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

/* HANDY BASIC TYPES */

typedef unsigned int nat;

typedef void * MemP;  /* Pointer to memory area */
typedef int MemSize;  /* Size in bytes of memory area */

/* ERRORS AND ASSERTIONS */

void error (char *msg);
  /* Prints string {*msg} to {stderr} and stops. */

void programerror (char *msg, char *file, unsigned int line, char* proc);
  /* Prints {file ":" line ": (" *proc ")" *msg} to {stderr} and stops.
    Meant for {assert} below. */

#define assert(test, msg) \
  do \
    { if (!(test)) (void)programerror((msg), __FILE__, __LINE__, __FUNCTION__); } \
  while (0)
  /* If test is false, aborts after printing {*msg} and the source location.
    It is declared as a macro, rather than as a procedure, in order to avoid
    evaluating {msg} when {test} is true.  */

void *checknotnull(void * p, char *msg, char *file, unsigned int line, char* proc);
  /* If {p == NULL}, prints {file ":" line ": (" *proc ")" *msg} to {stderr} and
    stops; otherwise returns {p} itself.  Meant for {notnull} below. */

#define notnull(p, msg) \
  checknotnull(p, (msg), __FILE__, __LINE__, __FUNCTION__)
  /* If {p == NULL}, aborts with {*msg} with the program 
    location; otherwise returns {p} itself. */
  
/* STRING UTILITIES */

int isprefix(char *s, char *t);
  /* Returns TRUE iff the string {*s} is a prefix of {*t},
    including the cases when {*s} is empty or is equal to {*t}. */

char *txtcat (char *a, char *b);
  /* Returns a string that is the concatenation of {*a} and {*b}. 
    The result is always a newly allocated string. */
  
char *addext(char *name, char *ext);
  /* Appends the string {*ext} (which should normally start with a
    period) to the given file {*name}. However, if {*name} is empty or
    "-", ignores {*ext} and returns a copy of {*name} itself. In any
    case, the result is a newly allocated string.*/

char *fmtint(int x, unsigned wid);
  /* Returns a newly allocated string containing the integer {x}
    converted to decimal and zero-padded to {wid} characters. */

/* TIME/DATE UNTILITIES */

char *today(void);
  /* Returns a newly allocated string containing 
    today's date in the format "yy-mm-dd hh:mm:ss" */

double now(void);
  /* Returns the current user time of the process, in microseconds. */

/* FILE AND I/O UTILITIES */

FILE *open_write(char *name);
FILE *open_read(char *name);
  /* Opens file for output or input, bombs in case of failure. */
    
char *read_line(FILE *f);
  /* Reads the next line from file {f}, until '\n', NUL, or EOF.
    Returns it as a NUL-terminated string, without the final '\n'.
    Replaces TABs by single spaces. Allocates space for the result with 
    {malloc()}. Returns NULL iff the file is already at EOF. */
  
/* MATH UTILITIES */

#if (defined(SunOS4) || defined(SunOS5))
extern long random(void);
extern int srandom(unsigned seed);
#endif

float frandom(void);
double drandom(void);
  /* Returns a uniform random number in [0.0 __ 1.0). */

float fgaussrand(void);
double dgaussrand(void);
  /* Returns a random number with Gaussian distr, zero mean, 
    unit deviation. */

int ipow(int x, nat y);
  /* Returns {x^y}. */

double rel_diff(double x, double y);
  /* Returns the relative difference between {x} and {y}, namely
    {(x-y)/sqrt(x^2 + y^2)/2}. This number is zero when {x == y}, is
    0.5 when only one of them is zero, and has extremal values -1
    and +1 when {x} and {y} are equal and opposite). */
    
#endif
