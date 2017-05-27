/* See js.h */
/* Last edited on 2003-07-07 13:26:22 by stolfi */

#include "js.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/times.h>
#include <limits.h>
#include <values.h>

void error (char *msg)
  { fprintf (stderr, "*** %s\n", msg);
    exit(1);
  }
  
void programerror (char *msg, char *file, unsigned int line, char* proc)
  { fprintf (stderr, "%s:%u: *** (%s) %s\n", file, line, proc, msg);
    exit(1);
  }

void *checknotnull(void * p, char *msg, char *file, unsigned int line, char* proc)
  { if (p == NULL) { programerror((msg), __FILE__, __LINE__, __FUNCTION__); }
    return p;
  }

int isprefix(char *s, char *t)
  { while (((*s)!='\000') &&((*t)!='\000') && ((*s) == (*t)))
      { s++; t++; }
    return (*s) == '\000';
  }

char *txtcat (char *a, char *b)
  { char *r = malloc(strlen(a)+strlen(b)+1);
    assert (r != NULL, "memory exhausted");
    strcpy(r, a);
    strcat(r, b);
    return(r);
  }

char *addext(char *name, char *ext)
  { 
    if ((strcmp(name, "") == 0) || (strcmp(name, "-") == 0))
      { return txtcat(name, ""); }
    else
      { return txtcat(name, ext); }
  }

char *fmtint(int x, unsigned wid)
  {
#define FMTINT_BUFSIZE 200
    char buf[FMTINT_BUFSIZE];
    int rcode = snprintf(buf, FMTINT_BUFSIZE, "%0*d", wid, x);
    assert (rcode >= 0, "snprintf failed");
    { int n = strlen(buf);
      char *res = (char *)malloc(n+1);
      strcpy(res, buf);
      return res;
    }
#undef FMTINT_BUFSIZE   
  }

char *today(void)
  {
#define TODAY_BUFSIZE 200
    char buf[TODAY_BUFSIZE];
    int rcode;
    time_t today_secs = time(NULL);
    struct tm today;
    today = *localtime(&today_secs);
    rcode = snprintf(buf, TODAY_BUFSIZE,
      "%02d-%02d-%02d %02d:%02d:%02d", 
      today.tm_year % 100, today.tm_mon, today.tm_mday, 
      today.tm_hour, today.tm_min, today.tm_sec
    );
    assert (rcode >= 0, "snprintf failed");
    { int n = strlen(buf);
      char *res = (char *)malloc(n+1);
      strcpy(res, buf);
      return res;
    }
#undef TODAY_BUFSIZE
  }

#if (defined(SunOS4))
  double now(void) 
    { struct rusage ru;
      getrusage(RUSAGE_SELF, &ru);
      return(((double)ru.ru_utime.tv_sec)*1000000.0 + ((double)ru.ru_utime.tv_usec));
    }
#endif

#if (defined(SunOS5))
extern long int __sysconf (int);
double now(void) 
  { struct tms buf;
    clock_t t;
    times(&buf);
    t = buf.tms_utime;
    return(1000000.0L * ((double) t)/((double) _sysconf(3)));
  }
#endif

#if (defined(Linux))
#include <bits/types.h>
extern long int __sysconf (int);
double now(void) 
  { struct tms buf;
    clock_t t;
    times(&buf);
    t = buf.tms_utime;
    return(1000000.0L * ((double) t)/((double) __sysconf(2)));
  }
#endif

#if (defined(OSF1V4))
extern long int __sysconf (int);
double now(void) 
  { struct tms buf;
    clock_t t;
    times(&buf);
    t = buf.tms_utime;
    return(1000000.0L * ((double) t)/((double) CLK_TCK));
  }
#endif

FILE *open_write(char *name)
  {
    FILE *f;
    fprintf(stderr, "opening %s\n", name); fflush(stderr);
    f = fopen(name, "w");
    assert (f != NULL, txtcat("can't create file ", name));
    return (f);
  }

FILE *open_read(char *name)
  {
    FILE *f;
    fprintf(stderr, "opening %s\n", name); fflush(stderr);
    f = fopen(name, "r");
    assert (f != NULL, txtcat("can't find file ", name));
    return (f);
  }

char *read_line(FILE *f)
  {
    int mc = 0;
    int nc = 0;
    char *s = NULL;
    int c;
    do
      { c = getc(f); 
        if ((c == EOF) && (nc == 0)) return(NULL);
        if ((c == EOF) || (c == '\n')) c = '\000';
        if (c == '\t') c = ' ';
	if (nc >= mc)
          { if (mc == 0)
	      { mc = 40; s = (char *) malloc(mc*sizeof(char)); }
	    else 
	      { mc *= 2; s = (char *) realloc ((void *) s, mc*sizeof(char)); }
          }
	assert (s != NULL, "alloc failed");
	s[nc] = c;
	nc++;
      }
    while (c != '\000');
    return (s);
  }

/* 
  The implementations of $frandom$ and $drandom$ below
  are not entirely correct, because normalization may 
  produce trailing zero bits with probability slightly 
  larger than 1/2.
*/
float frandom(void)
  {
    float f = 0.0;
    f = (f + (random()&8388607))/8388608.0;
    return (f);
  }

double drandom(void)
  {
    double d = 0.0;
    d = (d + (random()&536870911))/536870912.0;
    d = (d + (random()&8388607))/8388608.0;
    return (d);
  }

#define SQRT_TWO 1.4142135623730950488016887242

static double MIN_GAUSS_R = 0.0;

float fgaussrand(void)
  {
    return (float)(dgaussrand());
  }

double dgaussrand(void)
  { /* Polar method [Knuth II:3.4.1, Algorithm P] */
    if(MIN_GAUSS_R == 0.0)
      { MIN_GAUSS_R = sqrt(log(MAXDOUBLE))/MAXDOUBLE; }
    while(1)
      { 
        double u = 2.0*drandom() - 1.0;
        double v = 2.0*drandom() - 1.0;
        double r = hypot(u, v);
        if ((r < 1.0) && (r > MIN_GAUSS_R))
          { /* In unit circle, and far enough from overflow: */
            return (SQRT_TWO * (u + v) * sqrt(-log(r))/r);
          }
      }
  }

int ipow(int x, nat y)
  { int p = 1;
    nat b = UINT_MAX/2 + 1;
    while (b > y) { b /= 2; }
    while (b > 0)
      { p = p*p; 
        if (y >= b) { p *= x; y -= b; } 
        b /= 2;
      }
    return p;
  }

double rel_diff(double x, double y)
  { double mx = fabs(x), my = fabs(y);
    double m = (mx > my ? mx : my);
    if (m == 0.0) 
      { return 0.0; }
    else
      { double rx = x/m, ry = y/m;
        return 0.5*(rx - ry)/sqrt(rx*rx + ry*ry);
      }
  }

