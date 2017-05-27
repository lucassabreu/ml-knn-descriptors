/* Plotting graph of a 1-argument function with interval arithmetic. */
/* Last edited on 2002-12-30 01:02:35 by stolfi */

#ifndef iagraph_H
#define iagraph_H

#include <flt.h>
#include <ia.h>
#include <stdio.h>

void iagraph_plot
  ( FILE *psfile,
    Interval f (Interval x),
    Float xmin, Float xmax,
    Float ymin, Float ymax,
    int n
  );
  /* 
    Writes to $psfile$ a PostScript plot of the graph of f(x),
    computed on $n$ equal sub-intervals in $[xmin .. xmax]$.
    Specifiaclly, for each sub-interval $xi$, plots the box
    $(xi,f(xi))$
    
    Assumes that $psfile$ has been properly initialized for a plot
    with the given coordinate ranges. */

#endif
