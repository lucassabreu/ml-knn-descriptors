/* pswrdef.h -- private definitions for pswr.h. */
/* Last edited on 2003-01-08 20:57:03 by stolfi */

#ifndef pswrdef_H
#define pswrdef_H

#include <stdio.h>

typedef struct PSWriterPrivate  /* A PSWriter is a Postscript file plus some state: */
  { /* For all kinds of files: */
    FILE *psfile;          /* The file which this state refers to. */
    int eps;               /* Kind of Postscript file (TRUE = encapsulated). */
    int verbose;           /* TRUE prints a trace of some commands. */
    /* Local state (not available in Postscript file): */
    double xmin, xmax;     /* Client X plotting range (in client coords). */
    double ymin, ymax;     /* Client Y plotting range (in client coords). */
    double xscale, yscale; /* Scale factors (actual_pt/client_unit). */
    double fillColor[3];   /* Fill color for subsequent graphics ops. */
    int npages;            /* Number of pages already started (non-EPS). */
    int pagestarted;       /* Called pswr_begin_page w/o pswr_end_page (non-EPS). */
    /* These values mirror Postscript variables: */
    double hmin, hmax;     /* Actual X plotting range (in pt, page coords). */
    double vmin, vmax;     /* Actual Y plotting range (in pt, page coords). */
    int xn, yn;            /* Number of grid cells in each axis. */
  } PSWriterPrivate;

#endif
