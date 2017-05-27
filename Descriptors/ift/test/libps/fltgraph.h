/* Plotting the graph of a 1-argument function. */
/* Last edited on 2003-01-02 03:58:25 by stolfi */

#ifndef fltgraph_H
#define fltgraph_H

#include <flt.h>
#include <ia.h>
#include <stdio.h>

/* These procedures assume that $*psfile$ is a Postscript output file,
  properly initialized for a plot with the given coordinate ranges. */

void fltgraph_plot
  ( FILE *psfile,
    Float f (Float x),
    Float xmin, Float xmax,
    Float ymin, Float ymax,
    int m
  );
  /* Draws the graph of $f(x)$ for $x \in [xmin .. xmax]$,
    approximated by $m$ straight segments. */

void fltgraph_draw_axes
  ( FILE *psfile,
    Float xmin, Float xmax,
    Float ymin, Float ymax
  );
  /* Draws the parts of the coordinates axes that lie
    in the specified rectiangle. */
  
#endif
